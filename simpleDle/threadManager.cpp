#include <iostream>
#include <chrono>
#include "threadManager.h"
#include "sysStructure.h"

using namespace littleBadger;

int main(int argc, char **argv) {
  std::cout << "start project" << std::endl;
  // default setting
  CCAlg alg = DLE;
  DLSol sol = KILL;

  // init mapStructure
  initMapStructure();

  // create thread pool with 4 worker threads
  ThreadManager pool(4);

  std::this_thread::sleep_for (std::chrono::seconds (3));

  // load txns.txt and transform them to Txn objects
  std::cout << "build txns" << std::endl;
  buildTxnSet(); 
  std::vector<Txn> txnSet = readTxnSet();

  // use Txn objects to init BadgerThread objects and enqueue them into thread pool
  for (int i = 0; i < txnSet.size(); i++) {
    BadgerThread expThd(alg, sol, txnSet[i]);
    pool.enqueueObj(expThd);
  }

  // end threads
  // pool.~ThreadPool();
  std::this_thread::sleep_for (std::chrono::seconds (3));
  std::cout << "finish project" << std::endl; 
}
