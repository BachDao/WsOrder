#ifndef __WS_OPTIONS_H__
#define __WS_OPTIONS_H__

#include <string>

namespace WsOrder {
struct WsOptions {
  std::string host_;
  std::string port_;
  std::string path_;
};
} // namespace WsOrder
#endif //__WS_OPTIONS_H__
