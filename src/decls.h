
#ifndef __DECLS_H__
#define __DECLS_H__

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>

#include <cstddef>
#include <vector>

namespace WsOrder {

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace asio = boost::asio;
namespace ssl = boost::asio::ssl;

using BufferType = std::vector<std::byte>;

using ExecutionContext = asio::io_context;
using Segment = std::vector<std::byte>;
} // namespace WsOrder
#endif //__DECLS_H__
