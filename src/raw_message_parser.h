#ifndef __RAW_MESSAGE_PARSER_H__
#define __RAW_MESSAGE_PARSER_H__

#include "decls.h"
#include "raw_message.h"
#include <boost/optional.hpp>

namespace WsOrder {

/*!
 * RawMessageParser handling conversion between raw message object and bytes sequence
 * - serialize : writing content of RawMessage to appropriate position in byte sequence
 * - deserialize: make a RawMessage from bytes sequence
 *
 * Raw message format is:
 *
 *   +------------------+-----------------------+----------------------+--------------------------+-----------+
 *   | Length (4 bytes) |   SessionId (4 byte)  | MessageType (1 byte) | SequenceNumber (8 bytes) | Payload   |
 *   +------------------+-----------------------+----------------------+--------------------------+-----------+
 *
 */

class RawMessageParser {
public:
  static BufferType serialize(const RawMessage &);
  static boost::optional<RawMessage> deserialize(const BufferType &);
}; // class RawMessageParser

} // namespace WsOrder

#endif //__RAW_MESSAGE_PARSER_H__
