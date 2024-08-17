#include "exchange_client.h"
#include "ws_client.h"

namespace WsOrder {

ExchangeClient::ExchangeClient(WsOrder::ExecutionContext& ctx,
                               WsOrder::WsOptions options)
    : ptrClient_(std::make_unique<WsClient>(ctx, options)) {}

ExchangeClient::~ExchangeClient() = default;

void ExchangeClient::makeOrder(Order_Type orderType, double price,
                               uint64_t quantity) {
  Order order;
  order.set_id(makeId());
  order.set_type(orderType);
  order.set_price(price);
  order.set_quantity(quantity);

  sendOrder(order);
}

std::string ExchangeClient::makeId() {
  // Fake
  return "magicalId_";
}

void ExchangeClient::sendOrder(const Order &order) {
  ClientMessage msg;
  auto ptr = msg.mutable_order();
  *ptr = order;
  ptrClient_->sendMessage(msg);
}

Order ExchangeClient::getOrder(const std::string &id) const { return {}; }

std::vector<Order> ExchangeClient::getOrders() const { return {}; }

} // namespace WsOrder
