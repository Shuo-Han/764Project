#pragma once

#include <map> 
#include <string>
#include <condition_variable>
#include <mutex>
#include "txnManager.h"
#include "lockManager.h"

namespace littleBadger {
  // concurrency control algroithm: deferredLockEnforecment, traditional
  enum CCAlg {DLE, TRAD};
  // solution for dead lock: waitForever, killImediately
  enum DLSol {WAIT, KILL};

  /**
   * BadgerThread is an object that can be called by a thread of the threadManager. 
   * a BadgerThread is corresponing to a transaction by having infomation of a transaction.
   * a BadgerThread has functions to handle the transaction.
   */
  class BadgerThread {
  public:
    // parameters for a BadgerThread to cooperate with the other BadgerThreads
    CCAlg cc_alg; 
    DLSol dl_sol;

    // parameters for a transaction
    std::map<int, Semantic> reocrd_to_lock;
    std::vector<TxnAction> actions;
    std::vector<int> keys;
    std::vector<std::string> values;

    // synchronization
    std::condition_variable condition;
    std::mutex sharedLock;
    bool allSHAREDpurged = false;

    BadgerThread(CCAlg alg, DLSol sol, Txn txn) {
      this->cc_alg = alg;
      this->dl_sol = sol;
      this->actions = txn.actions;
      this->keys = txn.keys;
      this->values = txn.values;
    };

    // when all SHARED locks are released, lockManager call this function to wakeup BadgerThread
    const void wakeup();

    // the basic function for a BadgerThread to exectue a transaction
    const void run();

    // BadgerThread executes a transaction in a traditional lock way
    const void tradRun();

    // BadgerThread executes a transaction in a dle lock way
    const void dleRun();

    // BadgerThread executes a read transaction 
    const void readRecord(int key);

    // BadgerThread executes a write transaction 
    const void writeRecord(int key, std::string value);

    // BadgerThread executes a delete transaction 
    const void deleteRecord(int key);
  };
}