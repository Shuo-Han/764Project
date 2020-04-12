
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include "txnManager.h"

namespace littleBadger {
  // a set of transactions needed to be executed
  std::vector<Txn> txnSet;

  /**
   * this method is used to create a key for operations in txns
   * keys range from 0-99
   */
  int getKey(std::set<int> visit) {
    int actKey; 
    actKey = rand() % 100; // key
    // if key is already visited, change to another key
    while (visit.find(actKey) != visit.end()) {
      actKey = rand() % 100; 
    }
    visit.insert(actKey);

    return actKey;
  }

  /**
   * this methold builds a data set for experiment
   * each lin of txns.txt is like: txnID READ_O key value
   */
  const void bulildDataSet() {
    std::fstream fileStream;
    fileStream.open("txns.txt");
    if (fileStream.is_open()) {
      std::cout << "txns.txt exists" << std::endl;
      fileStream.close();
      return;
    }

    std::cout << "start building data set" << std::endl;
    std::ofstream myfile ("txns.txt");

    if (myfile.is_open()) {
      // there are 5000 txns and 100 records
      for (int txnID = 0; txnID < 5000; txnID++) {
        int txnType = rand() % 2; // 0: read_only case, 1: read_write case
        std::set<int> visit;

        if (txnType == 0) {
          // a read only transactoin has 10 read actions on different keys
          for (int i = 0; i < 10; i++) {
            int actKey = getKey(visit);

            char buffer[30];
            sprintf(buffer, "%d %s %d %d", txnID, "READ_O", actKey, 0); 
            myfile << buffer << std::endl;;
          }
        } else {
          // a read write transactoin has 5 reads
          for (int i = 0; i < 5; i++) {
            int actKey = getKey(visit);

            char buffer[30];
            sprintf(buffer, "%d %s %d %d", txnID, "READ_RW", actKey, 0); 
            myfile << buffer << std::endl;;
          }
          // a read write transactoin has 5 writes
          for (int i = 0; i < 5; i++) {
            int actKey = getKey(visit);

            char buffer[30];
            sprintf(buffer, "%d %s %d %d", txnID, "WRITE", actKey, actKey + 1); 
            myfile << buffer << std::endl;;
          }
        }
        visit.clear();
        std::cout << "finished " << txnID << std::endl;
      }
    } else {
      std::cout << "unable to open txns.txt" << std::endl;
    }
    
    std::cout << "finish building data set" << std::endl;
    myfile.close();
  }

  /**
   * this method reads txns.txt to build txnSet 
   */
  const void buildTxnSet() {
    std::string line;
    std::ifstream myfile("txns.txt");

    if (myfile.is_open()) {
      Txn* currentTxn;
      int preTxnID = -1;

      // read each line of txns.txt, a line is like: txnID READ_O key value
      while (getline(myfile, line)) {
        std::string parameters[4];
        int i = 0;
        // stringstream object separates line by space
        std::stringstream ssin(line);
        while (ssin.good() && i < 4) {
          ssin >> parameters[i];
          i++;
        }

        // check whether a transaction is valid
        TxnAction action;
        if (parameters[1].compare("READ_O") == 0) {
          action = READ_O;
        } else if (parameters[1].compare("READ_RW") == 0) {
          action = READ_RW;
        } else if (parameters[1].compare("WRITE") == 0) {
          action = WRITE;
        } else {
          std::cout << "transaction has no action" << std::endl;
          continue;
        }

        // new txn is coming
        if (std::stoi(parameters[0], nullptr) != preTxnID) {
          // store current txn in txnSet
          if (preTxnID != -1) {
            txnSet.push_back(*currentTxn);
          }

          // create new txn
          Txn txn;
          currentTxn = &txn; 
          preTxnID = std::stoi(parameters[0], nullptr);
        }

        // add an action to current txn
        currentTxn->addOperation(action, std::stoi(parameters[2], nullptr), parameters[3]);
      }

      // push the last txn to txnSet
      txnSet.push_back(*currentTxn);
      myfile.close();
    }
  }

  /**
   * this method is used to read txnSet 
   */
  const std::vector<Txn> readTxnSet() {
    return txnSet;
  }
}
