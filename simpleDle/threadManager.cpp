#include <iostream>
#include <chrono>
#include <ctime>  
#include "threadManager.h"
#include "lockManager.h"
#include "sysStructure.h"
#include "globals.h"

using namespace littleBadger;

  CCAlg alg;
  int numKeys;
  int ratio;
  int numThread;
  int readCount;
  int writeCount;

int main(int argc, char **argv) {
  // global setting
  alg = TRAD;
  numKeys = 50;
  ratio = 5;
  numThread = 50;
  readCount = 1;
  writeCount = 1;

  // init mapStructure
  initMapStructure(numKeys);
  initLockManager(numKeys);

  // create thread pool with numThread threads
  std::cout << "start project" << std::endl;
  ThreadManager pool(numThread);
  std::this_thread::sleep_for (std::chrono::seconds (3));

  std::cout << "build txns" << std::endl;
  // bulildDataSet(); // uncomment this line to build a new txns.txt with ratiom
  buildTxnSet(); 
  std::vector<Txn> txnSet = readTxnSet();
  std::cout << txnSet.size() << " finish building txnSet" << std::endl;

  // use Txn objects to init BadgerThread objects and enqueue them into thread pool
  for (size_t i = 0; i < txnSet.size(); i++) {
    BadgerThread expThd(txnSet[i]);
    pool.enqueueObj(expThd);
  }

  std::cout << "start hanlding txns" << std::endl;
  pool.startThreadManager();
}
