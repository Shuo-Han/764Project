
#include <iostream>
#include <chrono>
#include <map>
#include <mutex>
#include <condition_variable>
#include "badgerThread.h"
#include "sysStructure.h"

namespace littleBadger {
  /**
   * this function decides the way of how a BadgerThread cooperates with the others.
   */
  const void BadgerThread::run() {
    if (cc_alg == TRAD) {
      dleRun();
    } else if (cc_alg == DLE) {
      tradRun();
    }
  };

  /**
   * TODO:
   * how to run traditiondal lock based on lockManager
   */
  const void BadgerThread::tradRun() {
    // if (action == READ_O) {
    //   readRecord();
    // } else if (action == WRITE) {
    //   writeRecord();
    // }
  };

  /**
   * TODO:
   * how to run dle based on lockManager
   * int key = keys[actIndex];
   * std::string value = values[actIndex];
   */
  const void BadgerThread::dleRun() {
    // get locks for all actions
    for (int actIndex = 0; actIndex < actions.size(); actIndex++) {
      TxnAction act = actions[actIndex];
      if (act == READ_RW) {
        if (requireLock(keys[actIndex], RESERVED)) {
          reocrd_to_lock.insert(std::pair<int, Semantic>(keys[actIndex], RESERVED));
        } else {
          // KILL thread when failing to require a lock
          return;
        }
      } else if (act == WRITE) {
        if (requireLock(keys[actIndex], RESERVED)) {
          reocrd_to_lock.insert(std::pair<int, Semantic>(keys[actIndex], EXCLUSIVE));
        } else {
          // KILL thread when failing to require a lock
          return;
        }
      }
    }

    // do somthing
    std::cout << "do something" << std::endl;
    std::map<int, Semantic>::iterator it;
    for (it = reocrd_to_lock.begin(); it != reocrd_to_lock.end(); it++) {
      if (it->second == RESERVED) {
        std::cout << it->first << std::endl;
      }
    }
    // do somthing

    // change RESERVED to PENDDING
    for (it = reocrd_to_lock.begin(); it != reocrd_to_lock.end(); it++) {
      if (it->second == RESERVED) {
        changeSemantic(it->first);
        it->second = PENDDING;
      }
    }

    // wait until all SHARED locks are released 
    // when it return false, then condition.wait(lock, bool) will keep waiting.
    // when it return true, then we can move to next line if the thread receives a notify
    std::unique_lock<std::mutex> lk(sharedLock);
    condition.wait(lk, purgeAllShared());

    // change PENDDING to EXCLUSIVE
    for (it = reocrd_to_lock.begin(); it != reocrd_to_lock.end(); it++) {
      if (it->second == PENDDING) {
        changeSemantic(it->first);
        it->second = EXCLUSIVE;
      }
    }

    // finally, write something
    for (int actIndex = 0; actIndex < actions.size(); actIndex++) {
      if (actions[actIndex] == WRITE) {
        writeRecord(keys[actIndex], values[actIndex]);
      }
    }
  };

  const void BadgerThread::wakeup() {
    allSHAREDpurged = true;
    condition.notify_one();
  }

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
