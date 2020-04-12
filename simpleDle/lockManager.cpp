#include <map>
#include <string>
#include <vector>
#include <iostream>
#include "lockManager.h"
#include "globals.h"

namespace littleBadger {
  std::map<int, LockWrapper> lockManager; 

  LockWrapper::LockWrapper():semantic(UNLOCKED), sharedRefCount(0) {}

  //TODO: timeout machanism
  const bool acquire(Semantic s, int key) {
    LockWrapper* curLock = &lockMap.find(key)->second;
    if(s == SHARED) {
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
    LockWrapper* curLock = &lockMap.find(key)->second;

  switch(curLock->semantic) {
    case UNLOCKED: 
      std::cout << "ERROR: attemping unlocked records not in-use";
      return false;
    case RESERVED:
      std::cout << "ERROR: attemping release reserved lock";
      return false;
    case EXCLUSIVE:
      curLock->m.unlock();
      curLock->semantic = UNLOCKED;
    case SHARED:
      curLock->m.unlock_shared();
      if(curLock->sharedRefCount-- == 0)
        curLock->semantic = UNLOCKED;
    default:
      std::cout << "Unhandled lock mode";
  }

    return true;
  }

  const bool changeSemantic(int key) {
    return true;
  } 

  const bool purgeAllShared() {
    return true;
  }
}