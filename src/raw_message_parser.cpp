#include "raw_message_parser.h"

namespace WsOrder {

BufferType RawMessageParser::serialize(const RawMessage &msg) {
  // Allocate enough of memory
  // Append msg.size() to beginning
  // Filling appropriate SessionId, MessageType...
  // Copy the msg's body to the rest
  //
  return WsOrder::BufferType();
}

boost::optional<RawMessage> RawMessageParser::deserialize(const BufferType &) {
  // Check if we received entire size if not return fails
  //
  // Else, From size header,
  // check if we received an entire message, if not return fails
  //
  // Else, Strip the header, copy the payload and return
  // From a result, we know how many bytes already consumed.

  return boost::optional<RawMessage>();
}

} // namespace WsOrder
