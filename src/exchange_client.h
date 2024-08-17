#ifndef __EXCHANGE_CLIENT_H__
#define __EXCHANGE_CLIENT_H__

#include "decls.h"
#include "exchange.pb.h"
#include "ws_options.h"
#include <memory>

namespace WsOrder {
class WsClient;
/*!
 * ExchangeClient provide API for connecting to ExchangeServer, create and
 * monitoring status of Orders This class utilize WsClient and contains only
 * business related to Trading
 */
class ExchangeClient {
public:
  ExchangeClient(ExecutionContext& ctx, WsOptions options);
  ~ExchangeClient();

  void makeOrder(Order_Type, double price, uint64_t quantity);

  Order getOrder(const std::string &) const;
  std::vector<Order> getOrders() const;

private:

  /*!
   * Do our client need to create id_ for each Order,
   * if yes, can we do it ourself or any kind of collaboration with
   * server is needed ?
   *
   * @return string represent Order's id
   */
  std::string makeId();

  void sendOrder(const Order &order);

private:
  std::unique_ptr<WsClient> ptrClient_;

}; // class ExchangeClient
} // namespace WsOrder

#endif //__EXCHANGE_CLIENT_H__
