#pragma once

#include <string>

namespace littleBadger {
  class Record {
  public:
    // parameters of a record
    std::string value; 

    Record(std::string value) {
      this->value = value;
    };
  };

  const void initMapStructure();

  const std::string readMap(int key);

  const void writeMap(int key, std::string value);

  const void switchMap(int key, Record* record);

  const void deleteMap(int key);
}