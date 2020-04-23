#include <map>
#include <string>
#include <iostream>
#include <shared_mutex>
#include "sysStructure.h"

namespace littleBadger {
  /**
   * a map structure with pointer as value
   */

  std::map<int, Record*> mapPointerStructure;

  /**
   * create a map with 100 pairs, ex. 0:Record0, 1:Record1, ..., 99:Record99
   */
  const void initMapStructure(int numKeys) {
    for (int i = 0; i < numKeys; i++) {
      Record* newRecord = new Record(std::to_string(i));
      mapPointerStructure[i] = newRecord;
      std::cout << mapPointerStructure.find(i)->second->value << std::endl;
    }
  }

  const std::string readMap(int key) {
    return mapPointerStructure.find(key)->second->value;
  }

  const void writeMap(int key, std::string value) {
    mapPointerStructure.find(key)->second->value = value;
  }

  const void switchMap(int key, Record* newRecord) {
    mapPointerStructure.find(key)->second = newRecord;
  }

  const void freeAllMapValue(){
    for ( std::map<int, Record*>::iterator it = mapPointerStructure.begin(); 
      it != mapPointerStructure.end(); ++it ) {
        // delete it->second;
        std::cout << it -> second -> value << "\n"; 
    }
  }

  const void deleteMap(int key) {
    mapPointerStructure.erase(key);
  }

  /**
   * a simple map structure with no pointer, which is not used by other classes 
   */

  std::map<int, std::string> mapSimpleStructure;
  
  const void initMapSimpleStructure() {
    for (int i = 0; i < 100; i++) {
      mapSimpleStructure.emplace(i, std::to_string(i));
    }
  }

  const void readSimpleMap(int key) {
    std::cout << "read " <<  key << " finish" << std::endl;
  }

  const void writeSimpleMap(int key, std::string value) {
    std::cout << "write " << value << " finish" << std::endl;
    mapSimpleStructure.find(key)->second = value;
  }

  const void deleteSimpleMap(int key) {
    mapSimpleStructure.erase(key);
  }
}