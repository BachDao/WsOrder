#ifndef __RECEIVER_H__
#define __RECEIVER_H__

#include "decls.h"
#include "exchange.pb.h"
#include "raw_message.h"

#include <boost/signals2.hpp>

namespace WsOrder {
class WsClient;

class Receiver {
public:
  using ClientMessageSignal = boost::signals2::signal<void(ClientMessage)>;

  explicit Receiver(WsClient &);

  /*!
   * Connect a slot, used to notify when new ClientMessage is received.
   */
  void connectOnClientMessage(ClientMessageSignal::slot_function_type);

private:
  /*!
   * Processes a RawMessage from the WsConnection and decide if it should
   * be forwarded to the upper layer.
   * @param msg
   */
  void onRawMessage(RawMessage msg);

  bool isValidSequenceNo(uint64_t);
private:
  WsClient &client_;
  uint64_t seqNo_ = 0;
  ClientMessageSignal clientMsgSignal_;
}; // class receiver

} // namespace WsOrder
#endif //__RECEIVER_H__
