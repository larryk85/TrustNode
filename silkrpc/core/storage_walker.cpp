/*
   Copyright 2021 The Silkrpc Authors

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/


#include "storage_walker.hpp"

#include <set>
#include <sstream>

#include <boost/endian/conversion.hpp>

#include <silkworm/core/silkworm/common/endian.hpp>
#include <silkworm/node/silkworm/common/rlp_err.hpp>
#include <silkworm/node/silkworm/db/bitmap.hpp>
#include <silkworm/node/silkworm/db/util.hpp>

#include <silkrpc/common/log.hpp>
#include <silkrpc/common/util.hpp>
#include <silkrpc/core/rawdb/chain.hpp>
#include <silkrpc/core/state_reader.hpp>
#include <silkrpc/ethdb/cursor.hpp>
#include <silkrpc/ethdb/tables.hpp>
#include <silkrpc/ethdb/transaction_database.hpp>
#include <silkrpc/json/types.hpp>

namespace silkrpc {

silkworm::Bytes make_key(const evmc::address& address, const evmc::bytes32& location) {
    silkworm::Bytes res(silkworm::kAddressLength + silkworm::kHashLength, '\0');
    std::memcpy(&res[0], address.bytes, silkworm::kAddressLength);
    std::memcpy(&res[silkworm::kAddressLength], location.bytes, silkworm::kHashLength);
    return res;
}

silkworm::Bytes make_key(const evmc::address& address, uint64_t incarnation, const evmc::bytes32& location) {
    silkworm::Bytes res(silkworm::kAddressLength + silkworm::kHashLength + 8, '\0');
    std::memcpy(&res[0], address.bytes, silkworm::kAddressLength);
    boost::endian::store_big_u64(&res[silkworm::kAddressLength], incarnation);
    std::memcpy(&res[silkworm::kAddressLength + 8], location.bytes, silkworm::kHashLength);
    return res;
}

struct StorageItem {
    silkworm::Bytes key;
    silkworm::Bytes sec_key;
    silkworm::Bytes value;
};

bool operator<(const StorageItem& k1, const StorageItem& k2) {
    return k1.key < k2.key;
}

asio::awaitable<silkrpc::ethdb::SplittedKeyValue> next(silkrpc::ethdb::SplitCursor& cursor, uint64_t number) {
    auto kv = co_await cursor.next();
    if (kv.key2.empty()) {
        co_return kv;
    }
    uint64_t block = silkworm::endian::load_big_u64(kv.key3.data());
    while (block < number) {
        kv = co_await cursor.next();
        if (kv.key2.empty()) {
            break;
        }
        block = silkworm::endian::load_big_u64(kv.key3.data());
    }
    co_return kv;
}

asio::awaitable<silkrpc::ethdb::SplittedKeyValue> next(silkrpc::ethdb::SplitCursor& cursor, uint64_t number, uint64_t block, silkworm::Bytes loc) {
    silkrpc::ethdb::SplittedKeyValue skv;
    auto tmp_loc = loc;
    while (!loc.empty() && (tmp_loc == loc || block < number)) {
        skv = co_await cursor.next();
        if (skv.key2.empty()) {
            break;
        }

        loc = skv.key2;
        block = silkworm::endian::load_big_u64(skv.key3.data());
    }
    co_return skv;
}

asio::awaitable<void> StorageWalker::walk_of_storages(uint64_t block_number, const evmc::address& start_address,
        const evmc::bytes32& location_hash, uint64_t incarnation, AccountCollector& collector) {
    auto ps_cursor = co_await transaction_.cursor(db::table::kPlainState);

    auto ps_key{make_key(start_address, incarnation, location_hash)};
    silkrpc::ethdb::SplitCursor ps_split_cursor{*ps_cursor,
        ps_key,
        8 * (silkworm::kAddressLength + 8),
        silkworm::kAddressLength,
        silkworm::kAddressLength + 8,
        silkworm::kAddressLength + 8 + silkworm::kHashLength};

    auto sh_key{make_key(start_address, location_hash)};
    auto sh_cursor = co_await transaction_.cursor(db::table::kStorageHistory);
    silkrpc::ethdb::SplitCursor sh_split_cursor{*sh_cursor,
        sh_key,
        8 * silkworm::kAddressLength,
        silkworm::kAddressLength,
        silkworm::kAddressLength,
        silkworm::kAddressLength + silkworm::kHashLength};

    auto ps_skv = co_await ps_split_cursor.seek();
    auto sh_skv = co_await sh_split_cursor.seek();
    auto h_loc = sh_skv.key2;

    uint64_t block = silkworm::endian::load_big_u64(sh_skv.key3.data());

    auto cs_cursor = co_await transaction_.cursor_dup_sort(db::table::kPlainStorageChangeSet);

    if (block < block_number) {
        sh_skv = co_await next(sh_split_cursor, block_number);
    }

    auto go_on = true;
    while (go_on) {
        if (ps_skv.key1.empty() && sh_skv.key1.empty()) {
            break;
        }
        auto cmp = ps_skv.key1.compare(sh_skv.key1);
        if (cmp == 0) {
            if (ps_skv.key2.empty() && h_loc.empty()) {
                break;
            }
            auto cmp = ps_skv.key2.compare(h_loc);
        }
        if (cmp < 0) {
            auto address = silkworm::to_evmc_address(ps_skv.key1);
            go_on = collector(address, ps_skv.key2, ps_skv.value);
        } else {
            const auto bitmap = silkworm::db::bitmap::read(sh_skv.value);
            const auto found = silkworm::db::bitmap::seek(bitmap, block_number);
            if (found) {
                auto dup_key{silkworm::db::storage_change_key(found.value(), start_address, incarnation)};

                auto data = co_await cs_cursor->seek_both(dup_key, h_loc);
                if (data.length() > silkworm::kHashLength) { // Skip deleted entries
                    data = data.substr(silkworm::kHashLength);
                    auto address = silkworm::to_evmc_address(ps_skv.key1);
                    go_on = collector(address, ps_skv.key2, data);
                }
            } else if (cmp == 0) {
                auto address = silkworm::to_evmc_address(ps_skv.key1);
                go_on = collector(address, ps_skv.key2, ps_skv.value);
            }
        }
        if (go_on) {
            if (cmp <= 0) {
                ps_skv = co_await ps_split_cursor.next();
            }
            if (cmp >= 0) {
                auto block = silkworm::endian::load_big_u64(sh_skv.key3.data());
                sh_skv = co_await next(sh_split_cursor, block_number, block, h_loc);
                h_loc = sh_skv.key2;
            }
        }
    }

    co_return;
}

asio::awaitable<void> StorageWalker::storage_range_at(uint64_t block_number, const evmc::address& address,
        const evmc::bytes32& start_location, int16_t max_result, StorageCollector& collector) {
    ethdb::TransactionDatabase tx_database{transaction_};
    auto account_data = co_await tx_database.get_one(db::table::kPlainState, full_view(address));

    auto [account, err] = silkworm::Account::from_encoded_storage(account_data);
    silkworm::rlp::success_or_throw(err);

    std::set<StorageItem> storage;
    AccountCollector walker = [&](const evmc::address& addr, const silkworm::ByteView loc, const silkworm::ByteView data) {
        if (addr != address) {
            return false;
        }
        if (data.size() == 0) {
            return true;
        }

        auto hash = hash_of(loc);

        StorageItem storage_item;
        storage_item.key = loc;
        storage_item.sec_key = full_view(hash);
        storage_item.value = data;

        if (storage.find(storage_item) != storage.end()) {
            return true;
        }

        storage.insert(storage_item);
        return storage.size() <= max_result;
    };

    StorageWalker storage_walker{transaction_};
    co_await storage_walker.walk_of_storages(block_number + 1, address, start_location, account.incarnation, walker);

    for (auto item : storage) {
        collector(item.key, item.sec_key, item.value);
    }
    co_return;
}

} // namespace silkrpc
