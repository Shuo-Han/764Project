
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
  void getKey(std::vector<int> *visit) {
    int actKey; 
    actKey = rand() % 100; // key
    // if key is already visited, change to another key
    while (std::find(visit->begin(), visit->end(), actKey) != visit->end()) {
      actKey = rand() % 100; 
    }
    visit->push_back(actKey);

    return;
  }

  /**
   * this methold builds a data set for experiment
   * each lin of txns.txt is like: txnID READ_O key value
   */
  const void bulildDataSet() {
    std::cout << "start building data set" << std::endl;
    std::ofstream myfile ("txns.txt");

    if (myfile.is_open()) {
      // there are 5000 txns and 100 records
      for (int txnID = 0; txnID < 5000; txnID++) {
        int txnType = 1; // rand() % 2; // 0: read_only case, 1: read_write case
        std::vector<int> visit;

        // get 10 keys and make them in order
        for (int i = 0; i < 10; i++) {
          getKey(&visit);  
        }
        std::sort(visit.begin(), visit.end());

        // a read only transactoin has 10 read actions on different keys
        if (txnType == 0) {
          for (int i = 0; i < 10; i++) {
            char buffer[30];
            sprintf(buffer, "%d %s %d %d", txnID, "READ_O", visit[i], 0); 
            myfile << buffer << std::endl;;
          }
        // a read write transactoin has 7 reads and 3 writes
        } else {
          for (int i = 0; i < 7; i++) {
            char buffer[30];
            sprintf(buffer, "%d %s %d %d", txnID, "READ_RW", visit[i], 0); 
            myfile << buffer << std::endl;;
          }
          for (int i = 7; i < 10; i++) {
            char buffer[30];
            sprintf(buffer, "%d %s %d %d", txnID, "WRITE", visit[i], visit[i] + 1); 
            myfile << buffer << std::endl;;
          }
        }

        visit.clear();
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
