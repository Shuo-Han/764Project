
#include <iostream>
#include <chrono>
#include <map>
#include "badgerThread.h"
#include "sysStructure.h"
#include "globals.h"

namespace littleBadger {
  /**
   * this function decides the way of how a BadgerThread cooperates with the others.
   */
  const void BadgerThread::run() {
    if (alg == DLE) {
      dleRun();
    } else if (alg == TRAD) {
      tradRun();
    }
  };

  /**
   * TODO:
   * how to run traditiondal lock based on lockManager
   */
  const void BadgerThread::tradRun() {
    // get locks for all actions
    for (int actIndex = 0; actIndex < actions.size(); actIndex++) {
      TxnAction act = actions[actIndex];
      if (act == READ_O) {
        acquire(keys[actIndex], SHARED);
        reocrd_to_lock.insert(std::pair<int, Semantic>(keys[actIndex], SHARED));
      } else if (act == READ_RW || act == WRITE) {
        acquire(keys[actIndex], EXCLUSIVE);
        reocrd_to_lock.insert(std::pair<int, Semantic>(keys[actIndex], EXCLUSIVE));
      }
    }

    // read phase / logic operation
    std::map<int, Semantic>::iterator it;
    for (it = reocrd_to_lock.begin(); it != reocrd_to_lock.end(); it++) {
      readRecord(it->first);
    }
    // read phase / logic operation

    // commit phase 
    for (int actIndex = 0; actIndex < actions.size(); actIndex++) {
      if (actions[actIndex] == WRITE) {
        writeRecord(keys[actIndex], values[actIndex]);
      }
    }
    // commit phase 

    // release lock
    for (int actIndex = 0; actIndex < actions.size(); actIndex++) {
      release(keys[actIndex]);
    }
  };

  /**
   * TODO:
   * how to run dle based on lockManager
   * int key = keys[actIndex];
   * std::string value = values[actIndex];
   */
  const void BadgerThread::dleRun() {
    // get locks for all actions

    // read phase / logic operation
    // read phase / logic operation

    // change RESERVED to PENDDING

    // change PENDDING to EXCLUSIVE

    // finally, write something
  };

  const void BadgerThread::readRecord(int key) {
    readMap(key);
  };

  const void BadgerThread::writeRecord(int key, std::string value) {
    writeMap(key, value);
  };

  const void BadgerThread::deleteRecord(int key) {
    deleteMap(key);
  };
}
