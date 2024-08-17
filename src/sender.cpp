#include "sender.h"
#include "ws_client.h"

namespace WsOrder {

/// This Timeout value should be tunable, based on network conditions.
uint32_t Sender::Timeout = 5;

Sender::Sender(WsClient &client, ExecutionContext &ctx)
    : client_(client), ctx_(ctx), timer_(ctx_) {
  client_.connectConnectionResetSignal([this] { onConnectionReset(); });
}

Sender::~Sender() {}

void Sender::connectSendRawMessageSignal(SendSignal::slot_function_type fn) {
  sendSignal_.connect(fn);
}

void Sender::onAckMessage(WsOrder::RawMessage ackMsg) {
  if (!ptrCurMsg_)
    return;

  if (ackMsg.seqNo_ != ptrCurMsg_->seqNo_)
    return;

  {
    std::scoped_lock lck{mutex_};
    // remove acked message
    msgQueue_.pop();
    ptrCurMsg_ = nullptr;
    stopTimer();
  }

  // send the next one
  doSend();
}

void Sender::doSend() {

  {
    std::scoped_lock lck{mutex_};
    if (msgQueue_.empty()) {
      sendInProgress_ = false;
      return;
    }
    ptrCurMsg_ = &msgQueue_.front();
  }

  // update sending status and handover message to a WsConnection object
  sendInProgress_ = true;
  sendSignal_(*ptrCurMsg_);
  startTimer();
}

void Sender::send(ClientMessage clientMsg) {
  RawMessage msg = makeRawMessage(clientMsg);
  {
    std::scoped_lock lck{mutex_};
    msgQueue_.emplace(std::move(msg));
  }

  if (sendInProgress_)
    return;

  doSend();
}

void Sender::onConnectionReset() {
  if (ptrCurMsg_ == nullptr)
    return;

  sendSignal_(*ptrCurMsg_);
}

RawMessage Sender::makeRawMessage(const WsOrder::ClientMessage &msg) {

  RawMessage rawMsg;

  rawMsg.seqNo_ = seqNo_++;
  rawMsg.type_ = MessageType::Normal;
  rawMsg.sessionId_ = client_.id_;

  std::vector<std::byte> buf(msg.ByteSizeLong());
  msg.SerializeToArray(buf.data(), buf.size());
  rawMsg.payload_ = std::move(buf);

  return rawMsg;
}

void Sender::startTimer() {
  timer_.expires_after(std::chrono::seconds(5));
  timer_.async_wait([this](const boost::system::error_code &e) { onTimer(e); });
}

void Sender::stopTimer() { timer_.cancel(); }

void Sender::onTimer(const boost::system::error_code &e) {
  if (e == boost::system::errc::operation_canceled)
    return;

  if (!ptrCurMsg_)
    return;

  sendInProgress_ = true;
  sendSignal_(*ptrCurMsg_);
  startTimer();
}

} // namespace WsOrder