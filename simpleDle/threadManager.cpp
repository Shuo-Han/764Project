#include <iostream>
#include <chrono>
#include <ctime>  
#include "threadManager.h"
#include "lockManager.h"
#include "sysStructure.h"
#include "globals.h"

using namespace littleBadger;

  CCAlg alg;
  DLSol sol;
  int numKeys;
  int ratio;
  int numThread;
  int readCount;
  int writeCount;
  extern int numKeys; 

int main(int argc, char **argv) {
  auto start = std::chrono::system_clock::now();
  std::time_t start_time = std::chrono::system_clock::to_time_t(start);
  std::cout << "start project: " << std::ctime(&start_time) << std::endl;

  // global setting
  alg = TRAD;
  sol = KILL;
  numKeys = 50;
  ratio = 5;
  numThread = 4;
  readCount = 1;
  writeCount = 1;

  // init mapStructure
  initMapStructure(numKeys);
  initLockManager(numKeys);

  // create thread pool with 4 worker threads
  ThreadManager pool(numThread);
  std::this_thread::sleep_for (std::chrono::seconds (3));

  std::cout << "build txns" << std::endl;
  bulildDataSet(); // uncomment this line to build a new txns.txt with ratiom
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

  // end threads
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
}
