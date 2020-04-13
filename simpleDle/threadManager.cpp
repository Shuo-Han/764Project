#include <iostream>
#include <chrono>
#include "threadManager.h"
#include "lockManager.h"
#include "sysStructure.h"

using namespace littleBadger;

int main(int argc, char **argv) {
  std::cout << "start project" << std::endl;

  // init mapStructure
  initMapStructure();
  initLockManager(100);

  // create thread pool with 4 worker threads
  ThreadManager pool(4);
  std::this_thread::sleep_for (std::chrono::seconds (3));

  // load txns.txt and transform them to Txn objects
  std::cout << "build txns" << std::endl;
  buildTxnSet(); 
  std::vector<Txn> txnSet = readTxnSet();
  std::cout << txnSet.size() << " finish building txnSet" << std::endl;

  // use Txn objects to init BadgerThread objects and enqueue them into thread pool
  for (size_t i = 0; i < txnSet.size(); i++) {
    BadgerThread expThd(txnSet[i]);
    // std::cout << expThd.actions.size() << " ====" << std::endl;
    pool.enqueueObj(expThd);
  }

  // end threads
  // pool.~ThreadPool();
  std::this_thread::sleep_for (std::chrono::seconds (3));
  std::cout << "finish project" << std::endl; 
}
