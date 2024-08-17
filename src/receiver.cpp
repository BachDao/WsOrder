#include "receiver.h"
#include "raw_message_parser.h"
#include "ws_client.h"

namespace WsOrder {

Receiver::Receiver(WsClient &client) : client_(client) {
  client_.connectOnNormalMessage(
      [this](RawMessage msg) { onRawMessage(std::move(msg)); });
}

void Receiver::connectOnClientMessage(
    ClientMessageSignal::slot_function_type fn) {
  clientMsgSignal_.connect(fn);
}

void Receiver::onRawMessage(WsOrder::RawMessage msg) {
  if (msg.type_ !=
      MessageType::Normal) // Receiver doesn't care about ctrl message
    return;

  auto seqNo = msg.seqNo_;

  if (seqNo > seqNo_ + 1) {
    // this means we lost one message, or other does the wrong thing
    // as our protocol running on top of TCP, this should never happen
    std::cout << "smt bad happend" << std::endl;
    abort();
  }

  // response with ack

  if (seqNo <= seqNo_) {
    // we received an old message, drop it
    std::cout << "outdated message is dropped" << std::endl;
    return;
  }

  // deliver message to upper layer
  ClientMessage clientMessage;
  clientMessage.ParseFromArray(msg.payload_.data(), msg.payload_.size());
  clientMsgSignal_(clientMessage);
}

bool Receiver::isValidSequenceNo(uint64_t seqNo) {
  // Decide if seqNo is valid
  return seqNo == (seqNo_ + 1);
}

} // namespace WsOrder