#include "ws_client.h"
#include "receiver.h"
#include "sender.h"
#include "ws_connection.h"

#include <thread>

namespace WsOrder {

WsClient::WsClient(ExecutionContext &ctx, WsOptions options)
    : ctx_(ctx), options_(options), id_(generateRandomId()) {
  makeConnection();

  // init Sender
  ptrSender_ = std::make_unique<Sender>(*this, ctx);
  ptrSender_->connectSendRawMessageSignal([this](RawMessage msg) {
    // we need to make sure that ptrCon_ always contains a valid object
    ptrCon_->sendData(msg);
  });

  // init Receiver
  ptrReceiver_ = std::make_unique<Receiver>(*this);
  ptrReceiver_->connectOnClientMessage(
      [this](ClientMessage msg) { clientMsgSignal(msg); });
}

std::string WsClient::generateRandomId() {
  std::hash<std::thread::id> hashFn;
  return "magicStr" + std::to_string(hashFn(std::this_thread::get_id()));
}

void WsClient::makeConnection() {
  ptrCon_.reset(nullptr);

  ptrCon_ = std::make_unique<WsConnection>(options_, ctx_);
  ptrCon_->onMessage([this](RawMessage msg) { onRawMessage(msg); });
  ptrCon_->onDisconnect([this] { onConnectionReset(); });

  // this can be used to implement "sliding window" feature
  ptrCon_->onSentComplete([] {
    // accumulate ack logic
  });
}

void WsClient::onRawMessage(RawMessage msg) {
  switch (msg.type_) {
  case MessageType::Open:
  case MessageType::Close:
    handleControlMessage(msg);
  case MessageType::Ack:
    ackSignal_(msg);
    break;
  case MessageType::Normal:
    msgSignal_(msg);
    break;
  }
}

void WsClient::onConnectionReset() {
  resetSignal_();
  makeConnection();
}

void WsClient::handleControlMessage(RawMessage message) {
  // keep track state of "Logical" connection
  // Logic about how Logical connection is established,
  // teardown and maintain is implement here

  //  state_ = WsClientState::Established;
}

void WsClient::connectConnectionResetSignal(boost::function<void(void)> fn) {
  resetSignal_.connect(fn);
}

void WsClient::connectOnAckMessage(boost::function<void(RawMessage)> fn) {
  ackSignal_.connect(fn);
}

void WsClient::connectOnNormalMessage(boost::function<void(RawMessage)> fn) {
  msgSignal_.connect(fn);
}

void WsClient::onMessage(std::function<void(ClientMessage)> fn) {
  clientMsgSignal.connect(fn);
}

WsClient::~WsClient() = default;

void WsClient::sendMessage(WsOrder::ClientMessage msg) {
  if (ptrSender_)
    ptrSender_->send(msg);
}

} // namespace WsOrder
