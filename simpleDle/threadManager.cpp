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

/*
arg1: alg, 0 - TRAD, 1 - DLE
arg2: # keys
arg3: ratio
arg4: # threads
arg5: readcount
arg6: writecount
*/
int main(int argc, char **argv) {
  // if (argc != 7){
  //   std::cout << "wrong argc\n";
  //   return 1;
  // }
  
  // // global setting
  // if (argv[1][0] == '0'){
  //   alg = TRAD;
  // } else {
  //   alg = DLE;
  // }
  
  // numKeys = atoi(argv[2]);
  // ratio = atoi(argv[3]);
  // numThread = atoi(argv[4]);
  // readCount = atoi(argv[5]);
  // writeCount = atoi(argv[6]);
  alg = DLE;
  numKeys = 50;
  ratio = 0;
  numThread = 10;
  readCount = 5;
  writeCount = 1;

  // init mapStructure
  initMapStructure(numKeys);
  initLockManager(numKeys);

  // create thread pool with numThread threads
  // std::cout << "start project" << std::endl;
  ThreadManager pool(numThread);
  std::this_thread::sleep_for (std::chrono::seconds (3));

  // std::cout << "build txns" << std::endl;
  bulildDataSet(); // uncomment this line to build a new txns.txt with ratiom
  buildTxnSet(); 
  std::vector<Txn> txnSet = readTxnSet();
  // std::cout << txnSet.size() << " finish building txnSet" << std::endl;

  // use Txn objects to init BadgerThread objects and enqueue them into thread pool
  for (size_t i = 0; i < txnSet.size(); i++) {
    BadgerThread expThd(txnSet[i]);
    pool.enqueueObj(expThd);
  }

  // std::cout << "start hanlding txns" << std::endl;
  pool.startThreadManager();
}
