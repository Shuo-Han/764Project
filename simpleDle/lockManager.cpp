#include <map>
#include <string>
#include <vector>
#include <iostream>
#include "lockManager.h"
#include "globals.h"

/*
Lock transition logic:
    
  |-----------------| 
  |  |---------> SHARED
UNLOCKED
  |  |---------> RESERVED ----0---> PENDING ----1----> EXCLUSIVE
  |--------------------------------------------------------|
0: block SHARED enrollment beyond this point
1: wait until al SHAREDs released
*/
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
    if(alg == TRAD)
      return acquireTRAD(curLock, s);
    else if(alg == DLE)
      return acquireDLE(curLock, s);
    
    std::cout << "unreachable code block in acquire()\n";
    return false;
  }

  const bool acquireTRAD(LockWrapper* cur, Semantic s) {
    if (s == SHARED) {
      cur->m.lock_shared();
      cur->semantic = SHARED;
      cur->sharedRefCount++;
    } else if(s == EXCLUSIVE){
      cur->m.lock();
      cur->semantic = EXCLUSIVE;
      return true;
    }
    std::cout << s << " lock not allowed under TRAD mode\n";
    return false;
  }

  const bool acquireDLE(LockWrapper* cur, Semantic s) {
    switch (s) {
    case UNLOCKED:
      std::cout << "illegal semantics: UNLOCKED\n";
      return false;;
    case SHARED:
      cur->m.lock_shared();
      if(cur->semantic != RESERVED)
        cur->semantic = SHARED;
      cur->sharedRefCount++;
      return true;
    case RESERVED:
      cur->r.lock();
      cur->semantic = RESERVED;
      return true;
    case PENDING:
      std::cout << "PENDING is supposed to be handled implicitly while acquiring EXCLUSIVE\n";
      return false;
    case EXCLUSIVE:
      while(cur->semantic != RESERVED) {
        // sth happened bwtween line 70-71.
        std::cout << "WARNING: expected RESERVED but " << 
          Semantic(cur->semantic) << std::endl;
      }
      cur->semantic = PENDING;
      cur->m.lock();
      cur->semantic = EXCLUSIVE;
      return true;
    }

    std::cout << "unreachable code block in aquireDLE()\n";
    return false;
  }

  const bool release(int key) {
    LockWrapper* curLock = &lockManager.find(key)->second;
    Semantic hardenSemantic = curLock->semantic;
    switch (hardenSemantic) {
      case UNLOCKED: 
        std::cout << "ERROR: attemping unlocked records not in-use\n";
        return false;
      // TODO: force break RESERVED OR PENDING?
      case RESERVED:
      case PENDING:
        std::cout << "ERROR: attemping to release " << 
          Semantic(hardenSemantic) << std::endl;
        return false;
      case EXCLUSIVE:
        curLock->m.unlock();
        if(alg == DLE)
          curLock->r.unlock();
        curLock->semantic = UNLOCKED;
        return true;
      case SHARED:
        curLock->m.unlock_shared();
        if(curLock->sharedRefCount-- == 0)
          curLock->semantic = UNLOCKED;
        return true;
      default:
        std::cout << "Unhandled lock mode " << Semantic(curLock->semantic) << " \n";
        return false;
    }

    return true;
  }

  const void check(int key) {
    LockWrapper* curLock = &lockManager.find(key)->second;
    std::cout << "key: " << key << ", lock: " << Semantic(curLock->semantic) << std::endl;
  }
}