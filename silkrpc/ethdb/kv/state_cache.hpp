/*
   Copyright 2022 The Silkrpc Authors

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

#ifndef SILKRPC_ETHDB_KV_STATE_CACHE_HPP_
#define SILKRPC_ETHDB_KV_STATE_CACHE_HPP_

#include <cstddef>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <shared_mutex>

#include <silkrpc/config.hpp>

#include <absl/container/btree_set.h>
#include <asio/awaitable.hpp>

#include <silkrpc/common/util.hpp>
#include <silkrpc/ethdb/transaction.hpp>
#include <silkrpc/interfaces/remote/kv.pb.h>
#include <silkworm/common/base.hpp>

namespace silkrpc::ethdb::kv {

class StateView {
public:
    virtual ~StateView() = default;

    virtual asio::awaitable<std::optional<silkworm::Bytes>> get(const silkworm::Bytes& key) = 0;

    virtual asio::awaitable<std::optional<silkworm::Bytes>> get_code(const silkworm::Bytes& key) = 0;
};

class StateCache {
public:
    virtual std::unique_ptr<StateView> get_view(Transaction& txn) = 0;

    virtual void on_new_block(const remote::StateChangeBatch& state_changes) = 0;

    virtual std::size_t size() = 0;
};

struct CoherentStateRoot {
    absl::btree_set<KeyValue> cache;
    absl::btree_set<KeyValue> code_cache;
    bool ready{false};
    bool canonical{false};
};

using StateViewId = uint64_t;

constexpr auto kDefaultMaxViews{5ul};
constexpr auto kDefaultMaxStateKeys{1'000'000u};
constexpr auto kDefaultMaxCodeKeys{10'000u};

struct CoherentCacheConfig {
    uint64_t max_views{kDefaultMaxViews};
    bool with_storage{true};
    uint32_t max_state_keys{kDefaultMaxStateKeys};
    uint32_t max_code_keys{kDefaultMaxCodeKeys};
};

class CoherentStateCache;

class CoherentStateView : public StateView {
public:
    explicit CoherentStateView(Transaction& txn, CoherentStateCache* cache);

    CoherentStateView(const CoherentStateView&) = delete;
    CoherentStateView& operator=(const CoherentStateView&) = delete;

    asio::awaitable<std::optional<silkworm::Bytes>> get(const silkworm::Bytes& key) override;

    asio::awaitable<std::optional<silkworm::Bytes>> get_code(const silkworm::Bytes& key) override;

private:
    Transaction& txn_;
    CoherentStateCache* cache_;
};

class CoherentStateCache : public StateCache {
public:
    explicit CoherentStateCache(CoherentCacheConfig config = {});

    CoherentStateCache(const CoherentStateCache&) = delete;
    CoherentStateCache& operator=(const CoherentStateCache&) = delete;

    std::unique_ptr<StateView> get_view(Transaction& txn) override;

    void on_new_block(const remote::StateChangeBatch& batch) override;

    std::size_t size() override;

    uint64_t state_hit_count() const { return state_hit_count_; }
    uint64_t state_miss_count() const { return state_miss_count_; }
    uint64_t state_key_count() const { return state_key_count_; }
    uint64_t state_eviction_count() const { return state_eviction_count_; }
    uint64_t code_hit_count() const { return code_hit_count_; }
    uint64_t code_miss_count() const { return code_miss_count_; }
    uint64_t code_key_count() const { return code_key_count_; }
    uint64_t code_eviction_count() const { return code_eviction_count_; }

private:
    friend class CoherentStateView;

    void process_upsert_change(CoherentStateRoot* root, StateViewId view_id, const remote::AccountChange& change);
    void process_code_change(CoherentStateRoot* root, StateViewId view_id, const remote::AccountChange& change);
    void process_delete_change(CoherentStateRoot* root, StateViewId view_id, const remote::AccountChange& change);
    void process_storage_change(CoherentStateRoot* root, StateViewId view_id, const remote::AccountChange& change);
    KeyValue* add(KeyValue&& kv, CoherentStateRoot* root, StateViewId view_id);
    KeyValue* add_code(KeyValue&& kv, CoherentStateRoot* root, StateViewId view_id);
    asio::awaitable<std::optional<silkworm::Bytes>> get(const silkworm::Bytes& key, Transaction& txn);
    asio::awaitable<std::optional<silkworm::Bytes>> get_code(const silkworm::Bytes& key, Transaction& txn);
    CoherentStateRoot* get_root(StateViewId view_id);
    CoherentStateRoot* advance_root(StateViewId view_id);
    void evict_roots();

    CoherentCacheConfig config_;

    std::map<StateViewId, std::unique_ptr<CoherentStateRoot>> state_view_roots_;
    StateViewId latest_state_view_id_{0};
    CoherentStateRoot* latest_state_view_{nullptr};
    std::list<KeyValue> state_evictions_;
    std::list<KeyValue> code_evictions_;
    std::shared_mutex rw_mutex_;

    uint64_t state_hit_count_{0};
    uint64_t state_miss_count_{0};
    uint64_t state_key_count_{0};
    uint64_t state_eviction_count_{0};
    uint64_t code_hit_count_{0};
    uint64_t code_miss_count_{0};
    uint64_t code_key_count_{0};
    uint64_t code_eviction_count_{0};
};

}  // namespace silkrpc::ethdb::kv

#endif  // SILKRPC_ETHDB_KV_STATE_CACHE_HPP_
