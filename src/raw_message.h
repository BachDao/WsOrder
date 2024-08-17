#ifndef __RAW_MESSAGE_H__
#define __RAW_MESSAGE_H__

#include "decls.h"

#include <string>

namespace WsOrder {

enum class MessageType : uint8_t { Open = 0, Close, Ack, Normal };

struct RawMessage {
  uint64_t seqNo_;
  MessageType type_;
  std::string sessionId_;
  BufferType payload_;

  size_t size() const {
    return sizeof(seqNo_) + sizeof(type_) + sessionId_.length() +
           payload_.size();
  }
};

} // namespace WsOrder

#endif //__RAW_MESSAGE_H__
