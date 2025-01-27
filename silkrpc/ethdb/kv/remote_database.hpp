/*
   Copyright 2020 The Silkrpc Authors

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

#ifndef SILKRPC_ETHDB_KV_REMOTE_DATABASE_HPP_
#define SILKRPC_ETHDB_KV_REMOTE_DATABASE_HPP_

#include <cstddef>
#include <memory>
#include <vector>
#include <utility>

#include <asio/io_context.hpp>
#include <grpcpp/grpcpp.h>

#include <silkrpc/common/log.hpp>
#include <silkrpc/ethdb/database.hpp>
#include <silkrpc/ethdb/kv/remote_transaction.hpp>
#include <silkrpc/ethdb/kv/state_cache.hpp>
#include <silkrpc/ethdb/kv/tx_streaming_client.hpp>

namespace silkrpc::ethdb::kv {

template<typename Client = TxStreamingClient>
class RemoteDatabase: public Database {
public:
    RemoteDatabase(
        asio::io_context& io_context,
        std::shared_ptr<grpc::Channel> channel,
        grpc::CompletionQueue* queue,
        StateCache* state_cache)
    : io_context_(io_context),
      stub_{remote::KV::NewStub(channel)},
      queue_(queue),
      state_cache_(state_cache) {
        SILKRPC_TRACE << "RemoteDatabase::ctor " << this << "\n";
    }

    ~RemoteDatabase() {
        SILKRPC_TRACE << "RemoteDatabase::dtor " << this << "\n";
    }

    RemoteDatabase(const RemoteDatabase&) = delete;
    RemoteDatabase& operator=(const RemoteDatabase&) = delete;

    asio::awaitable<std::unique_ptr<Transaction>> begin() override {
        SILKRPC_TRACE << "RemoteDatabase::begin " << this << " start\n";
        auto txn = std::make_unique<RemoteTransaction<Client>>(io_context_, stub_, queue_);
        co_await txn->open();
        SILKRPC_TRACE << "RemoteDatabase::begin " << this << " txn: " << txn.get() << " end\n";
        co_return txn;
    }

private:
    asio::io_context& io_context_;
    std::unique_ptr<remote::KV::StubInterface> stub_;
    grpc::CompletionQueue* queue_;
    StateCache* state_cache_;
};

} // namespace silkrpc::ethdb::kv

#endif  // SILKRPC_ETHDB_KV_REMOTE_DATABASE_HPP_
