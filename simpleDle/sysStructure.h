#pragma once

#include <string>

namespace littleBadger {
  /**
   * a record is the value of the key:value pair in this DBMS
   */
  class Record {
  public:
    // parameters of a record
    std::string value; 

    Record(std::string value) {
      this->value = value;
    };
  };

  // this method initialize a in memory database
  const void initMapStructure(int numKeys);

  // read the value based on the given key
  const std::string readMap(int key);

  // traditional lock mechanism uses this fun to update a key:value pair 
  const void writeMap(int key, std::string value);

  // dle uses to update a key:value pair
  const void switchMap(int key, Record* record);

  // remove a pair based on the given key
  const void deleteMap(int key);
}