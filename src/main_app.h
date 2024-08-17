#ifndef __MAIN_APP_H__
#define __MAIN_APP_H__

#include "decls.h"

namespace WsOrder {

/*!
 * Maintain an ExecutionContext, also decide how many thread will be used
 * to process application logic
 */
class MainApp {
public:
  /*!
   * MainApp ctor
   * @param threadNums Specify number of thread using to run application
   */
  explicit MainApp(size_t threadNums);

  /*!
   * MainApp dtor
   * Try to call teardown on ExecutionContext, wait util it finish and return
   */
  ~MainApp();

  /// start application
  void run();

  /*!
   * Stop the execution of MainApp
   * - Remove all network handlers
   * - Clear message queue
   * - Wait util all of worker threads finish, then return
   */
  void stop();

  ExecutionContext &getContext();

private:
  ExecutionContext ctx_;
  size_t threadNum_;
  std::vector<std::thread> workers_;
}; // class MainApp
} // namespace WsOrder

#endif //__MAIN_APP_H__
