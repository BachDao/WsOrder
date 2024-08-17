#ifndef __SENDER_H__
#define __SENDER_H__

#include "decls.h"
#include "exchange.pb.h"
#include "raw_message.h"

#include <boost/signals2.hpp>

#include <atomic>
#include <mutex>
#include <queue>

namespace WsOrder {
class WsClient;

/*!
 * The Sender uses the WsConnection object to send messages over a WebSocket
 * connection. It provides:
 * - A queue of outbound messages, transmitted one by one.
 * - Re-transmits messages if an AckMessage is not received within a specified
 * time.
 */
class Sender {

public:
  using SendSignal = boost::signals2::signal<void(const RawMessage &)>;
  Sender(WsClient &, ExecutionContext&);
  ~Sender();

  /*!
   * API to send WsClient message with guarantee that eventually it will be sent
   * to other side
   * @param msg
   */
  void send(ClientMessage msg);

  /*!
   * Provide Sender a function to call when it has RawMessage to send,
   * should be provide by WsClient
   */
  void connectSendRawMessageSignal(SendSignal::slot_function_type);

private:
  void doSend();

  /*!
   * When connection is reset, we simply re-transmit on-fly message that
   * not Ack yet
   */
  void onConnectionReset();

  /*!
   * Acked, let's remove pending message from Q
   */
  void onAckMessage(RawMessage);

  /*!
   * Make RawMessage from Application's level message, most important
   * information need to fill is seqNo and sessionId
   * @return RawMessage, wrap around Application's level message
   */
  RawMessage makeRawMessage(const ClientMessage &);

  void startTimer();
  void stopTimer();
  void onTimer(const boost::system::error_code&);

private:
  WsClient &client_;
  std::queue<RawMessage> msgQueue_{};
  uint64_t seqNo_ = 0;
  std::atomic<bool> sendInProgress_ = false;
  std::mutex mutex_{};
  SendSignal sendSignal_{};
  RawMessage *ptrCurMsg_ = nullptr;
  ExecutionContext& ctx_;
  boost::asio::steady_timer timer_;

  static uint32_t Timeout;

}; // class Sender
} // namespace WsOrder

#endif //__SENDER_H__
