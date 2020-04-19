#pragma once

#include <string>
#include <vector>

namespace littleBadger {
  // a transaction has action for READ and WRITE
  // note that if a txn is not read only, then it is a read-write txn
  enum TxnAction {READ, WRITE};

  /**
   * Txn stores basic info of a transaction
   */
  class Txn {
  public:
    std::vector<TxnAction> actions;
    std::vector<int> keys;
    std::vector<std::string> values;

    void addOperation(TxnAction action, int k, std::string val) {
      this->actions.push_back(action);
      this->keys.push_back(k);
      this->values.push_back(val);
    }
  };

  // create a random txnSet and store it as txns.txt 
  const void bulildDataSet();

  // load txns.txt and build a txnSet needed to be executed by DBMS
  const void buildTxnSet();

  // list some info of txns in txnSet
  const std::vector<Txn> readTxnSet();
}