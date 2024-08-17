#include <exchange_client.h>
#include <main_app.h>

using namespace WsOrder;

int main() {
  MainApp mainApp(1);

  WsOptions options{.host_ = "localhost", .port_ = "443", .path_ = "order"};
  ExchangeClient client(mainApp.getContext(), options);

  mainApp.run();
  client.makeOrder(Order_Type_Buy, 42.24, 1000);

  return 0;
}