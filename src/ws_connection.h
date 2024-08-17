#ifndef __WS_CONNECTION_H__
#define __WS_CONNECTION_H__

#include "decls.h"
#include "raw_message.h"
#include "ws_options.h"

#include <functional>
#include <list>
#include <string>

namespace WsOrder {
using SslStreamType = ssl::stream<asio::ip::tcp::socket>;
using WsStreamType = websocket::stream<SslStreamType>;

/*!
 * WsConnection provide following features
 * - Sends and receives complete messages one at a time
 * - Notifies upper-layer when data is successfully handed over to the OS for
 * sending.
 * - On disconnection, discards unprocessed receive data and notifies the
 * upper-layer.
 */
class WsConnection {
public:
  WsConnection(WsOptions ops, asio::io_context &ctx);
  ~WsConnection();

  /*!
   * Send message to Server
   */
  void sendData(const RawMessage &);

  /*!
   *  Register message handler callback
   */
  void onMessage(std::function<void(RawMessage)>);

  /*!
   * Register message sent complete callback
   */
  void onSentComplete(std::function<void()>);

  /*!
   * Register WebSocket disconnection callback
   */
  void onDisconnect(std::function<void()>);

private:
  std::unique_ptr<WsStreamType> initWebSocket();
  void doWrite();
  void teardown();
  void connectToServer();
  void doRead();
  void onRead();
  void copyIncomingData();

private:
  BufferType inBuf_{};
  BufferType outBuf_{};

  WsOptions options_;
  asio::io_context::strand ioCtx_;
  std::unique_ptr<WsStreamType> ptrWsStream_ = nullptr;
  beast::flat_buffer readBuf_{};

  std::function<void()> sentCompleteFn_{};
  std::function<void()> disconnectFn_{};
  std::function<void(RawMessage)> msgHandlerFn_{};

  static size_t RetryCount;

}; // class WsConnection

} // namespace WsOrder

#endif //__WS_CONNECTION_H__
