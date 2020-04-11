#include <map>
#include <string>
#include <vector>
#include "lockManager.h"

namespace littleBadger {
  std::map<int, LockWrapper> lockManager; 

  const bool requireLock(Semantic s, int key) {
    return true;
  }

  const bool releaseLock(int key) {
    return true;
  }

  const bool changeSemantic(int key) {
    return true;
  } 

  const bool purgeAllShared() {
    return true;
  }
}