#include "raw_message_parser.h"
#include "ws_connection.h"

namespace WsOrder {

// Should we retry infinite?
size_t WsConnection::RetryCount = 5;

WsConnection::WsConnection(WsOptions ops, asio::io_context &ctx)
    : ioCtx_(ctx), options_(ops) {
  connectToServer();
  doRead();
}

WsConnection::~WsConnection() = default;

/// Ref:
/// https://www.boost.org/doc/libs/1_86_0/libs/beast/example/websocket/client/sync-ssl/websocket_client_sync_ssl.cpp
std::unique_ptr<WsStreamType> WsConnection::initWebSocket() {
  // Init websocket connection to "options_.host:options_.port_/options_.path"
  // with ssl support

  //Fake
  return nullptr;
}

void WsConnection::sendData(const RawMessage &msg) {
  // yes, we do copy
  outBuf_ = RawMessageParser::serialize(msg);
  doWrite();
}

void WsConnection::doWrite() {
  asio::const_buffer buf(outBuf_.data(), outBuf_.size());

  ptrWsStream_->async_write(buf, [this](const boost::system::error_code &erc,
                                        size_t bytesTransferred) {
    if (erc || bytesTransferred == 0) {
      teardown();
      return;
    }

    auto bytesRemaining = outBuf_.size() - bytesTransferred;
    if (bytesRemaining == 0) { // we're done with this message
      if (sentCompleteFn_)
        sentCompleteFn_();
      return;
    }

    // partial complete, update outBuf_ and continue
    // "erase" may not be efficient, but it is easy to follow
    outBuf_.erase(outBuf_.begin(), outBuf_.begin() + bytesTransferred);
    doWrite();
  });
}

void WsConnection::teardown() {
  outBuf_.clear();
  inBuf_.clear();

  if (disconnectFn_)
    disconnectFn_();
}

void WsConnection::onSentComplete(std::function<void()> fn) {
  sentCompleteFn_ = std::move(fn);
}

void WsConnection::onDisconnect(std::function<void()> fn) {
  disconnectFn_ = std::move(fn);
}

void WsConnection::connectToServer() {
  auto retry = RetryCount;
  while (retry--) {
    auto ptr = initWebSocket();
    if (ptr) {
      ptrWsStream_ = std::move(ptr);
      break;
    }
  }

  if (!ptrWsStream_) {
    // Fail after "RetryCount" times
    // i'm lazy
    abort();
  }
}

void WsConnection::doRead() {
  // start receiving data from socket
  readBuf_.clear();
  ptrWsStream_->async_read(
      readBuf_,
      [this](const boost::system::error_code &erc, size_t bytesTransferred) {
        if (erc || bytesTransferred == 0) {
          teardown();
          return;
        }
        onRead();
      });
}

void WsConnection::onRead() {
  copyIncomingData();
  auto optionalMsg = RawMessageParser::deserialize(inBuf_);

  if (!optionalMsg.has_value()) // partially received
    doRead();

  // drop consumed bytes
  auto msg = optionalMsg.value();
  inBuf_.erase(inBuf_.begin(), inBuf_.begin() + msg.size());

  // and tell everyone that we just received smt interesting
  if (msgHandlerFn_)
    msgHandlerFn_(std::move(msg));
}

void WsConnection::onMessage(std::function<void(RawMessage)> fn) {
  msgHandlerFn_ = std::move(fn);
}

void WsConnection::copyIncomingData() {
  // insert data from readBuf_ into the end of inBuf_;
  // inBuf_.insert(inBuf_.end(), readBuf_.begin(), readBuf_.end());
}

} // namespace WsOrder