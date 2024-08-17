#ifndef __WS_CLIENT_H__
#define __WS_CLIENT_H__

#include "exchange.pb.h"
#include "raw_message.h"
#include "ws_options.h"

#include <boost/signals2.hpp>

#include <functional>
#include <memory>

namespace WsOrder {
class WsConnection;
class Sender;
class Receiver;

/*!
 * WsClient is coordinator, it connect underlying WsConnection with Sender &
 * Receiver, provide upper layer clean & easy to use API.
 *
 * WsClient also is "Logical WsConnection" between our module and Exchange
 * server, it has unique "SessionId" and is alive even Physical Websocket's
 * connection is interrupted
 */
class WsClient {
  friend class Sender;
  friend class Receiver;
  using ConnectionResetSignal = boost::signals2::signal<void()>;
  using AckReceivedSignal = boost::signals2::signal<void(RawMessage)>;
  using NormalMsgReceivedSignal = boost::signals2::signal<void(RawMessage)>;
  using ClientMessageSignal = boost::signals2::signal<void(ClientMessage)>;

  enum class WsClientState { Init = 0, Established, Teardown, Invalid };

public:
  WsClient(ExecutionContext &ctx, WsOptions options);
  ~WsClient();

  void sendMessage(ClientMessage msg);
  void onMessage(std::function<void(ClientMessage)>);

private:
  /*!
   * Notify when underlying websocket connection is reset
   * Both Sender & Receiver should subscribe to this signal and
   * handle appropriately
   */
  void connectConnectionResetSignal(ConnectionResetSignal::slot_function_type);

  /*!
   * Notify when Ack packet is received from WsConnection
   */
  void connectOnAckMessage(AckReceivedSignal::slot_function_type);

  /*!
   * Notify when normal message is receive from WsConnection
   */
  void connectOnNormalMessage(NormalMsgReceivedSignal::slot_function_type);

  /*!
   * Assumes we can magically generate a random connection ID without needing
   * to negotiate with the server!
   * @return
   */
  static std::string generateRandomId();

  /*!
   * Handling RawMessage received from WsConnection
   * - If it's an Ack message, send it to the Sender.
   * - If it's a Normal message, send it to the Receiver.
   * - If it's a Control message, handle it directly and update 'state_' accordingly.
   */
  void onRawMessage(RawMessage);

  /*!
   * Handling Websocket connection is interrupted
   */
  void onConnectionReset();

  /*!
   * Handling connection level control message
   * - Open connection
   * - Close connection
   * - Update connection state
   * - ...
   * @param message
   */
  void handleControlMessage(RawMessage);

  void makeConnection();

private:
  ExecutionContext &ctx_;
  WsOptions options_;
  std::unique_ptr<WsConnection> ptrCon_;
  std::unique_ptr<Sender> ptrSender_;
  std::unique_ptr<Receiver> ptrReceiver_;
  ClientMessageSignal clientMsgSignal;

  const std::string id_;

  ConnectionResetSignal resetSignal_{};
  AckReceivedSignal ackSignal_{};
  NormalMsgReceivedSignal msgSignal_{};

  WsClientState state_ = WsClientState::Invalid;
}; // class WsClient

} // namespace WsOrder
#endif //__WS_CLIENT_H__
