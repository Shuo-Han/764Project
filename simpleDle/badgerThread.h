#pragma once

#include <map> 
#include <string>
#include <condition_variable>
#include <time.h>
#include <chrono>
#include "txnManager.h"
#include "lockManager.h"
#include "sysStructure.h"

namespace littleBadger {
  /**
   * BadgerThread is an object that can be called by a thread of the threadManager. 
   * a BadgerThread is corresponing to a transaction by having infomation of a transaction.
   * a BadgerThread has functions to handle the transaction.
   */
  class BadgerThread {
  public:
    // parameters of a transaction
    std::vector<TxnAction> actions;
    std::vector<int> keys;
    std::vector<std::string> values;
    double duration;

    BadgerThread(Txn txn) {
      this->actions = txn.actions;
      this->keys = txn.keys;
      this->values = txn.values;
      this->duration = 0;
    };

    // the basic function for a BadgerThread to exectue a transaction
    const void run(int threadId);

    // BadgerThread executes a transaction in a traditional lock way
    const void tradRun();

    // BadgerThread executes a transaction in a dle lock way
    const void dleRun(int threadId);

    // BadgerThread executes a read transaction 
    const void readRecord(int key);

    // BadgerThread executes a write transaction 
    const void writeRecord(int key, std::string value);

    // BadgerThread executes a pointer switch for updating record by private buffer
    const void switchRecord(int key, Record* record);

    // BadgerThread executes a delete transaction 
    const void deleteRecord(int key);
  };
}