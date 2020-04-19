#include <iostream>
#include <thread>
#include <chrono>
#include <map>
#include "badgerThread.h"
#include "globals.h"
#include "stats.h"

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
   * this is traditional lock mechanism for concurrency control 
   */
  const void BadgerThread::tradRun() {
    // start time of getting locks
    auto start = std::chrono::system_clock::now();
    // get locks for all actions
    std::vector<int> writeActions;
    for (size_t actIndex = 0; actIndex < actions.size(); actIndex++) {
      TxnAction act = actions[actIndex];
      if (act == READ) {
        acquire(keys[actIndex], SHARED);
        reocrd_to_lock.insert(std::pair<int, Semantic>(keys[actIndex], SHARED));
      } else if (act == WRITE) {
        writeActions.push_back(actIndex);
        acquire(keys[actIndex], EXCLUSIVE);
        reocrd_to_lock.insert(std::pair<int, Semantic>(keys[actIndex], EXCLUSIVE));
      }
    }

    // latency of getting locks
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double, std::ratio<1>> elapsed_seconds = end - start;
    setLatency(elapsed_seconds.count());

    // read phase / logic operation
    std::this_thread::sleep_for(std::chrono::milliseconds(readCount));

    // commit phase 
    for (size_t actIndex = 0; actIndex < writeActions.size(); actIndex++) {
      if (actions[writeActions[actIndex]] == WRITE) {
        writeRecord(keys[writeActions[actIndex]], values[writeActions[actIndex]]);
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(writeCount));

    // release locks
    for (size_t actIndex = 0; actIndex < actions.size(); actIndex++) {
      release(keys[actIndex]);
    }
  }

  /**
   * this is DLE mechanism for concurrency control 
   */
  const void BadgerThread::dleRun() {
    // start time of getting locks
    auto start = std::chrono::system_clock::now();

    // get locks for all actions
    std::vector<int> writeActions;
    for (size_t actIndex = 0; actIndex < actions.size(); actIndex++) {
      TxnAction act = actions[actIndex];
      if (act == READ) {
        acquire(keys[actIndex], SHARED);
        reocrd_to_lock.insert(std::pair<int, Semantic>(keys[actIndex], SHARED));
      } else if (act == WRITE) {
        writeActions.push_back(actIndex);
        acquire(keys[actIndex], RESERVED);
        reocrd_to_lock.insert(std::pair<int, Semantic>(keys[actIndex], RESERVED));
      }
    }

    // latency of getting locks
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double, std::ratio<1>> elapsed_seconds = end - start;
    setLatency(elapsed_seconds.count());

    // read phase / logic operation
    std::this_thread::sleep_for(std::chrono::milliseconds(readCount));
    std::vector<Record> privateBuffer;
    for (size_t index = 0; index < writeActions.size(); index++) {
      int actIndex = writeActions[index];
      Record newRecord(values[actIndex]);
      privateBuffer.push_back(newRecord);
    }

    // change RESERVED to EXCLUSIVE, note that PENDING is implicitly handled during the transition
    for (size_t index = 0; index < writeActions.size(); index++) {
      int actIndex = writeActions[index];
      acquire(keys[actIndex], EXCLUSIVE);
      reocrd_to_lock.insert(std::pair<int, Semantic>(keys[actIndex], EXCLUSIVE));
    }

    // end of commit phase 
    for (size_t index = 0; index < writeActions.size(); index++) {
      int actIndex = writeActions[index];
      switchRecord(keys[actIndex], privateBuffer[index]);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(writeCount));

    // release locks
    for (size_t actIndex = 0; actIndex < actions.size(); actIndex++) {
      release(keys[actIndex]);
    }
  };

  const void BadgerThread::readRecord(int key) {
    readMap(key);
  };

  const void BadgerThread::writeRecord(int key, std::string value) {
    writeMap(key, value);
  };

  const void BadgerThread::switchRecord(int key, Record record) {
    switchMap(key, &record);
  };

  const void BadgerThread::deleteRecord(int key) {
    deleteMap(key);
  };
}
