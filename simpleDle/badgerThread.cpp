#include <iostream>
#include <thread>
#include <chrono>
#include <map>
#include "badgerThread.h"
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
   * this is traditional lock mechanism for concurrency control 
   */
  const void BadgerThread::tradRun() {
    // start time of this txn
    clock_t start = clock();

    // get locks for all actions
    std::vector<int> writeActions;
    for (size_t actIndex = 0; actIndex < actions.size(); actIndex++) {
      TxnAction act = actions[actIndex];
      if (act == READ_O || READ_RW) {
        acquire(keys[actIndex], SHARED);
        reocrd_to_lock.insert(std::pair<int, Semantic>(keys[actIndex], SHARED));
      } else if (act == WRITE) {
        writeActions.push_back(actIndex);
        acquire(keys[actIndex], EXCLUSIVE);
        reocrd_to_lock.insert(std::pair<int, Semantic>(keys[actIndex], EXCLUSIVE));
      }
    }

    // read phase / logic operation
    std::this_thread::sleep_for (std::chrono::milliseconds(sleepCount));

    // commit phase 
    for (size_t actIndex = 0; actIndex < writeActions.size(); actIndex++) {
      if (actions[writeActions[actIndex]] == WRITE) {
        writeRecord(keys[writeActions[actIndex]], values[writeActions[actIndex]]);
      }
    }

    // release locks
    for (size_t actIndex = 0; actIndex < actions.size(); actIndex++) {
      release(keys[actIndex]);
    }

    // how long a txn takes
    duration = ((double) (clock() - start)) / CLOCKS_PER_SEC;
    printf("fun() took %f seconds to execute \n", duration);
  };

  /**
   * this is DLE mechanism for concurrency control 
   */
  const void BadgerThread::dleRun() {
    // start time of this txn
    clock_t start = clock();

    // get locks for all actions
    std::vector<int> writeActions;
    for (size_t actIndex = 0; actIndex < actions.size(); actIndex++) {
      TxnAction act = actions[actIndex];
      if (act == READ_O || READ_RW) {
        acquire(keys[actIndex], SHARED);
        reocrd_to_lock.insert(std::pair<int, Semantic>(keys[actIndex], SHARED));
      } else if (act == WRITE) {
        writeActions.push_back(actIndex);
        acquire(keys[actIndex], RESERVED);
        reocrd_to_lock.insert(std::pair<int, Semantic>(keys[actIndex], RESERVED));
      }
    }

    // read phase / logic operation
    std::this_thread::sleep_for (std::chrono::milliseconds(sleepCount));
    std::vector<Record> privateBuffer;
    for (size_t index = 0; index < writeActions.size(); index++) {
      int actIndex = writeActions[index];
      Record newRecord(values[actIndex]);
      privateBuffer.push_back(newRecord);
    }

    // change RESERVED to PENDDING
    for (size_t index = 0; index < writeActions.size(); index++) {
      int actIndex = writeActions[index];
      acquire(keys[actIndex], PENDING);
      reocrd_to_lock.insert(std::pair<int, Semantic>(keys[actIndex], PENDING));
    }

    // change PENDDING to EXCLUSIVE
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

    // release locks
    for (size_t actIndex = 0; actIndex < actions.size(); actIndex++) {
      release(keys[actIndex]);
    }

    // how long a txn takes
    duration = ((double) (clock() - start)) / CLOCKS_PER_SEC;
    printf("txn took %f seconds to execute \n", duration);
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
