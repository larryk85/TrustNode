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

#ifndef SILKRPC_TEST_MOCK_STATE_CACHE_HPP_
#define SILKRPC_TEST_MOCK_STATE_CACHE_HPP_

#include <cstddef>
#include <memory>
#include <optional>

#include <asio/awaitable.hpp>
#include <gmock/gmock.h>

#include <silkrpc/common/util.hpp>
#include <silkrpc/ethdb/kv/state_cache.hpp>
#include <silkrpc/ethdb/transaction.hpp>
#include <silkworm/common/util.hpp>

namespace silkrpc::test {

class MockStateView : public ethdb::kv::StateView {
  public:
    MOCK_METHOD((asio::awaitable<std::optional<silkworm::Bytes>>), get, (const silkworm::Bytes&));
    MOCK_METHOD((asio::awaitable<std::optional<silkworm::Bytes>>), get_code, (const silkworm::Bytes&));
};

class MockStateCache : public ethdb::kv::StateCache {
  public:
    MOCK_METHOD((std::unique_ptr<ethdb::kv::StateView>), get_view, (ethdb::Transaction&), (override));
    MOCK_METHOD((void), on_new_block, (const remote::StateChangeBatch&), (override));
    MOCK_METHOD((std::size_t), latest_data_size, (), (override));
    MOCK_METHOD((std::size_t), latest_code_size, (), (override));
    MOCK_METHOD((uint64_t), state_hit_count, (), (const));
    MOCK_METHOD((uint64_t), state_miss_count, (), (const));
    MOCK_METHOD((uint64_t), state_key_count, (), (const));
    MOCK_METHOD((uint64_t), state_eviction_count, (), (const));
    MOCK_METHOD((uint64_t), code_hit_count, (), (const));
    MOCK_METHOD((uint64_t), code_miss_count, (), (const));
    MOCK_METHOD((uint64_t), code_key_count, (), (const));
    MOCK_METHOD((uint64_t), code_eviction_count, (), (const));
};

}  // namespace silkrpc::test

#endif  // SILKRPC_TEST_MOCK_STATE_CACHE_HPP_
