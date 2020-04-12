#include <map>
#include <string>
#include <iostream>
#include <shared_mutex>
#include "sysStructure.h"
#include "globals.h"

namespace littleBadger {
  // create a map with 100 pairs, ex. 0:0, 1:1, ..., 99:99
  const void initMapStructure() {
    for (int i = 0; i < 100; i++) {
      mapStructure.emplace(i, std::to_string(i));
      lockMap.emplace(i, LockWrapper());
    }
  }

  const void readMap(int key) {
    std::cout << "read " <<  mapStructure.find(key)->second << " finish" << std::endl;
  }

  const void writeMap(int key, std::string value) {
    std::cout << "write " << value << " finish" << std::endl;
    mapStructure.find(key)->second = value;
  }

  const void deleteMap(int key) {
    mapStructure.erase(key);
  }
}