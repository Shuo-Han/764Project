#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
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
  // int is the key of a record, so lockManager tracks the lock mode of each record 
  std::map<int, LockWrapper> lockManager; 

  // constructor of a lockWrapper
  LockWrapper::LockWrapper():semantic(UNLOCKED), sharedRefCount(0) {};

  /**
   * init all lock modes for all in memory records
   */
  const void initLockManager(int numRecord) {
    for (int i = 0; i < numRecord; i++) {
      lockManager.emplace(std::piecewise_construct,
          std::forward_as_tuple(i),
          std::forward_as_tuple());
    }
  }

  //TODO: timeout machanism
  /**
   * this method lets a txn/thread to acquire a lock for the given key and semantic
   */
  const bool acquire(int key, Semantic s) {
    LockWrapper* curLock = &lockManager.find(key)->second;
    if(alg == TRAD)
      return acquireTRAD(curLock, s);
    else if(alg == DLE)
      return acquireDLE(curLock, s);
    
    std::cout << "unreachable code block in acquire()\n";
    return false;
  }

  /**
   * this method lets a txn/thread to acquire traditional locks
   */
  const bool acquireTRAD(LockWrapper* cur, Semantic s) {
    if (s == SHARED) {
      cur->m.lock_shared();
      cur->semantic = SHARED;
      cur->sharedRefCount++;
      return true;
    } else if (s == EXCLUSIVE) {
      cur->m.lock();
      cur->semantic = EXCLUSIVE;
      return true;
    }
    std::cout << Semantic(s) << " lock not allowed under TRAD mode\n";
    return false;
  }

  /**
   * this method lets a txn/thread to acquire DLE locks
   */
  const bool acquireDLE(LockWrapper* cur, Semantic s) {
    switch (s) {
    case UNLOCKED:
      throw std::runtime_error(std::string("illegal acquires: UNLOCKED"));
    // when acquiring SHARED, the transition can be UNLOCKED -> SHARED, RESERVED -> RESERVED,
    // and PENDING/EXCLUSIVE -> SHARED (after the EXCLUSIVE is released)
    case SHARED:
      cur->m.lock_shared();
      if (cur->semantic != RESERVED) {
        cur->semantic = SHARED;
      }
      cur->sharedRefCount++;
      return true;
    case RESERVED:
      cur->r.lock();
      cur->semantic = RESERVED;
      return true;
    case PENDING:
      throw std::runtime_error(
        std::string("illegal acquire: PENDING is implicitly handled while acquiring EXCLUSIVE"));
    // an EXCLUSIVE lock is acquired in a chain from RESERVED to PENDING, then EXCLUSIVE
    case EXCLUSIVE:
      while (cur->semantic != RESERVED) {
        throw std::runtime_error(
          std::string("illegal acquire: expected RESERVED but", Semantic(cur->semantic)));
      }
      // cur->semantic = PENDING;
      cur->m.lock();
      cur->semantic = EXCLUSIVE;
      return true;
    }

    throw std::runtime_error(std::string("unreachable code block in aquireDLE()"));
  }

  /**
   * this method lets a txn/thread to release DLE locks
   */
  const bool release(int key) {
    LockWrapper* curLock = &lockManager.find(key)->second;
    Semantic hardenSemantic = curLock->semantic;
    switch (hardenSemantic) {
      case UNLOCKED: 
        throw std::runtime_error(std::string("illegal release: UNLOCKED " + std::to_string(key)));
      case SHARED:
        if (curLock->sharedRefCount-- == 0) {
          curLock->semantic = UNLOCKED;
        }
        curLock->m.unlock_shared();
        return true;
      case RESERVED:
        curLock->m.unlock_shared();
        return true;
        // throw std::runtime_error(std::string("illegal release: RESERVED ") + std::to_string(key));
      // case PENDING:
      //   curLock->m.unlock_shared();
      //   return true;
        // throw std::runtime_error(std::string("illegal release: PENDING ") + std::to_string(key));
      // DLE uses r for RESERVED, and m for SHARED, PENDING, and EXCLUSIVE 
      // TRAD only uses m for SHARED and EXCLUSIVE
      case EXCLUSIVE:
        curLock->semantic = UNLOCKED;
        curLock->m.unlock();
        if (alg == DLE) {
          curLock->r.unlock();
        }
        return true;
      default:
        std::cout << "Unhandled lock mode " << Semantic(curLock->semantic) << " \n";
        return false;
    }

    return true;
  }

  /**
   * check the lock mode of a pair in lockManager based on the given key
   */
  const void check(int key) {
    LockWrapper* curLock = &lockManager.find(key)->second;
    std::cout << "key: " << key << ", lock: " << Semantic(curLock->semantic) << std::endl;
  }
}