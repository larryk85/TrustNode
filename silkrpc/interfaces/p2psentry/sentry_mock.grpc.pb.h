// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: p2psentry/sentry.proto

#include "p2psentry/sentry.pb.h"
#include "p2psentry/sentry.grpc.pb.h"

#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/sync_stream.h>
#include <gmock/gmock.h>
namespace sentry {

class MockSentryStub : public Sentry::StubInterface {
 public:
  MOCK_METHOD3(PenalizePeer, ::grpc::Status(::grpc::ClientContext* context, const ::sentry::PenalizePeerRequest& request, ::google::protobuf::Empty* response));
  MOCK_METHOD3(AsyncPenalizePeerRaw, ::grpc::ClientAsyncResponseReaderInterface< ::google::protobuf::Empty>*(::grpc::ClientContext* context, const ::sentry::PenalizePeerRequest& request, ::grpc::CompletionQueue* cq));
  MOCK_METHOD3(PrepareAsyncPenalizePeerRaw, ::grpc::ClientAsyncResponseReaderInterface< ::google::protobuf::Empty>*(::grpc::ClientContext* context, const ::sentry::PenalizePeerRequest& request, ::grpc::CompletionQueue* cq));
  MOCK_METHOD3(PeerMinBlock, ::grpc::Status(::grpc::ClientContext* context, const ::sentry::PeerMinBlockRequest& request, ::google::protobuf::Empty* response));
  MOCK_METHOD3(AsyncPeerMinBlockRaw, ::grpc::ClientAsyncResponseReaderInterface< ::google::protobuf::Empty>*(::grpc::ClientContext* context, const ::sentry::PeerMinBlockRequest& request, ::grpc::CompletionQueue* cq));
  MOCK_METHOD3(PrepareAsyncPeerMinBlockRaw, ::grpc::ClientAsyncResponseReaderInterface< ::google::protobuf::Empty>*(::grpc::ClientContext* context, const ::sentry::PeerMinBlockRequest& request, ::grpc::CompletionQueue* cq));
  MOCK_METHOD3(SendMessageByMinBlock, ::grpc::Status(::grpc::ClientContext* context, const ::sentry::SendMessageByMinBlockRequest& request, ::sentry::SentPeers* response));
  MOCK_METHOD3(AsyncSendMessageByMinBlockRaw, ::grpc::ClientAsyncResponseReaderInterface< ::sentry::SentPeers>*(::grpc::ClientContext* context, const ::sentry::SendMessageByMinBlockRequest& request, ::grpc::CompletionQueue* cq));
  MOCK_METHOD3(PrepareAsyncSendMessageByMinBlockRaw, ::grpc::ClientAsyncResponseReaderInterface< ::sentry::SentPeers>*(::grpc::ClientContext* context, const ::sentry::SendMessageByMinBlockRequest& request, ::grpc::CompletionQueue* cq));
  MOCK_METHOD3(SendMessageById, ::grpc::Status(::grpc::ClientContext* context, const ::sentry::SendMessageByIdRequest& request, ::sentry::SentPeers* response));
  MOCK_METHOD3(AsyncSendMessageByIdRaw, ::grpc::ClientAsyncResponseReaderInterface< ::sentry::SentPeers>*(::grpc::ClientContext* context, const ::sentry::SendMessageByIdRequest& request, ::grpc::CompletionQueue* cq));
  MOCK_METHOD3(PrepareAsyncSendMessageByIdRaw, ::grpc::ClientAsyncResponseReaderInterface< ::sentry::SentPeers>*(::grpc::ClientContext* context, const ::sentry::SendMessageByIdRequest& request, ::grpc::CompletionQueue* cq));
  MOCK_METHOD3(SendMessageToRandomPeers, ::grpc::Status(::grpc::ClientContext* context, const ::sentry::SendMessageToRandomPeersRequest& request, ::sentry::SentPeers* response));
  MOCK_METHOD3(AsyncSendMessageToRandomPeersRaw, ::grpc::ClientAsyncResponseReaderInterface< ::sentry::SentPeers>*(::grpc::ClientContext* context, const ::sentry::SendMessageToRandomPeersRequest& request, ::grpc::CompletionQueue* cq));
  MOCK_METHOD3(PrepareAsyncSendMessageToRandomPeersRaw, ::grpc::ClientAsyncResponseReaderInterface< ::sentry::SentPeers>*(::grpc::ClientContext* context, const ::sentry::SendMessageToRandomPeersRequest& request, ::grpc::CompletionQueue* cq));
  MOCK_METHOD3(SendMessageToAll, ::grpc::Status(::grpc::ClientContext* context, const ::sentry::OutboundMessageData& request, ::sentry::SentPeers* response));
  MOCK_METHOD3(AsyncSendMessageToAllRaw, ::grpc::ClientAsyncResponseReaderInterface< ::sentry::SentPeers>*(::grpc::ClientContext* context, const ::sentry::OutboundMessageData& request, ::grpc::CompletionQueue* cq));
  MOCK_METHOD3(PrepareAsyncSendMessageToAllRaw, ::grpc::ClientAsyncResponseReaderInterface< ::sentry::SentPeers>*(::grpc::ClientContext* context, const ::sentry::OutboundMessageData& request, ::grpc::CompletionQueue* cq));
  MOCK_METHOD3(SetStatus, ::grpc::Status(::grpc::ClientContext* context, const ::sentry::StatusData& request, ::sentry::SetStatusReply* response));
  MOCK_METHOD3(AsyncSetStatusRaw, ::grpc::ClientAsyncResponseReaderInterface< ::sentry::SetStatusReply>*(::grpc::ClientContext* context, const ::sentry::StatusData& request, ::grpc::CompletionQueue* cq));
  MOCK_METHOD3(PrepareAsyncSetStatusRaw, ::grpc::ClientAsyncResponseReaderInterface< ::sentry::SetStatusReply>*(::grpc::ClientContext* context, const ::sentry::StatusData& request, ::grpc::CompletionQueue* cq));
  MOCK_METHOD2(ReceiveMessagesRaw, ::grpc::ClientReaderInterface< ::sentry::InboundMessage>*(::grpc::ClientContext* context, const ::google::protobuf::Empty& request));
  MOCK_METHOD4(AsyncReceiveMessagesRaw, ::grpc::ClientAsyncReaderInterface< ::sentry::InboundMessage>*(::grpc::ClientContext* context, const ::google::protobuf::Empty& request, ::grpc::CompletionQueue* cq, void* tag));
  MOCK_METHOD3(PrepareAsyncReceiveMessagesRaw, ::grpc::ClientAsyncReaderInterface< ::sentry::InboundMessage>*(::grpc::ClientContext* context, const ::google::protobuf::Empty& request, ::grpc::CompletionQueue* cq));
  MOCK_METHOD2(ReceiveUploadMessagesRaw, ::grpc::ClientReaderInterface< ::sentry::InboundMessage>*(::grpc::ClientContext* context, const ::google::protobuf::Empty& request));
  MOCK_METHOD4(AsyncReceiveUploadMessagesRaw, ::grpc::ClientAsyncReaderInterface< ::sentry::InboundMessage>*(::grpc::ClientContext* context, const ::google::protobuf::Empty& request, ::grpc::CompletionQueue* cq, void* tag));
  MOCK_METHOD3(PrepareAsyncReceiveUploadMessagesRaw, ::grpc::ClientAsyncReaderInterface< ::sentry::InboundMessage>*(::grpc::ClientContext* context, const ::google::protobuf::Empty& request, ::grpc::CompletionQueue* cq));
  MOCK_METHOD2(ReceiveTxMessagesRaw, ::grpc::ClientReaderInterface< ::sentry::InboundMessage>*(::grpc::ClientContext* context, const ::google::protobuf::Empty& request));
  MOCK_METHOD4(AsyncReceiveTxMessagesRaw, ::grpc::ClientAsyncReaderInterface< ::sentry::InboundMessage>*(::grpc::ClientContext* context, const ::google::protobuf::Empty& request, ::grpc::CompletionQueue* cq, void* tag));
  MOCK_METHOD3(PrepareAsyncReceiveTxMessagesRaw, ::grpc::ClientAsyncReaderInterface< ::sentry::InboundMessage>*(::grpc::ClientContext* context, const ::google::protobuf::Empty& request, ::grpc::CompletionQueue* cq));
};

} // namespace sentry
