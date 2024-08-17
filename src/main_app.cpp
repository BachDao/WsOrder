#include "main_app.h"

namespace WsOrder {

MainApp::MainApp(size_t threadNums) : threadNum_(threadNums) {}

MainApp::~MainApp() {
  for (auto &w : workers_) {
    if (w.joinable())
      w.join();
  }
}

ExecutionContext &MainApp::getContext() { return ctx_; }

void MainApp::run() {
  for (int i = 0; i < threadNum_; ++i) {
    workers_.push_back(std::thread([this] { ctx_.run(); }));
  }
}

void MainApp::stop() {
  // std::atomic<bool> stopRequested_ = false;
  // .....
  // worker checking stopRequested_ for each round
  //
  // stop as necessary
}

} // namespace WsOrder
