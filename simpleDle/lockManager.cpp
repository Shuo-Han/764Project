#include <map>
#include <string>
#include <vector>
#include <iostream>
#include "lockManager.h"

namespace littleBadger {
  std::map<int, LockWrapper> lockManager; 

  LockWrapper::LockWrapper():semantic(UNLOCKED), sharedRefCount(0) {};

  const void initLockManager(int numRecord) {
    for (int i = 0; i < numRecord; i++) {
      lockManager.emplace(std::piecewise_construct,
          std::forward_as_tuple(i),
          std::forward_as_tuple());
    }
  }

  //TODO: timeout machanism
  const bool acquire(int key, Semantic s) {
    LockWrapper* curLock = &lockManager.find(key)->second;
    if (s == SHARED) {
      curLock->m.lock_shared();
      curLock->semantic = SHARED;
      curLock->sharedRefCount++;
    } else {
      curLock->m.lock();
      curLock->semantic = EXCLUSIVE;
    }
    return true;
  }

  const bool release(int key) {
    LockWrapper* curLock = &lockManager.find(key)->second;

    switch (curLock->semantic) {
      case UNLOCKED: 
        std::cout << "ERROR: attemping unlocked records not in-use\n";
        return false;
      case RESERVED:
        std::cout << "ERROR: attemping release reserved lock\n";
        return false;
      case EXCLUSIVE:
        curLock->m.unlock();
        curLock->semantic = UNLOCKED;
        return true;
      case SHARED:
        curLock->m.unlock_shared();
        if(curLock->sharedRefCount-- == 0)
          curLock->semantic = UNLOCKED;
        return true;
      default:
        std::cout << "Unhandled lock mode " << curLock->semantic << " \n";
        return false;
    }

    return true;
  }

  const void check(int key) {
    LockWrapper* curLock = &lockManager.find(key)->second;
    std::cout << "key: " << key << ", lock: " << Semantic(curLock->semantic) << std::endl;
  }
}