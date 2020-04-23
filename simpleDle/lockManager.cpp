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

  // concurrency control for updating lock semantics
  std::mutex sematic_update;

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
    else if(alg == DLE) {
      return acquireDLE(curLock, s);
    }
      
    std::cout << "unreachable code block in acquire()\n";
    return false;
  }

  /**
   * this method lets a txn/thread to release locks
   */
  const bool release(int key, Semantic s) {
    if(alg == TRAD)
      return releaseTRAD(key, s);
    else if(alg == DLE) {
      return releaseDLE(key, s);
    } 
    
    std::cout << "unreachable code block in release()\n";
    return false;
  }

  /**
   * this method lets a txn/thread to acquire traditional locks
   */
  const bool acquireTRAD(LockWrapper* cur, Semantic s) {
    if (s == SHARED) {
      cur->m.lock_shared();
      return true;
    } else if (s == EXCLUSIVE) {
      cur->m.lock();
      return true;
    }
    std::cout << Semantic(s) << " lock not allowed under TRAD mode\n";
    return false;
  }

  /**
   * this method lets a txn/thread to release DLE locks
   */
  const bool releaseTRAD(int key, Semantic s) {
    LockWrapper* curLock = &lockManager.find(key)->second;
    switch (s) {
      case SHARED:
        curLock->m.unlock_shared();
        return true;
      case EXCLUSIVE:
        curLock->m.unlock();
        return true;
      default:
        std::cout << "Unhandled lock mode " << Semantic(curLock->semantic) << " \n";
        return false;
    }
  }
  
  /**
   * this method lets a txn/thread to acquire DLE locks
   * the transition can be UNLOCKED -> SHARED, RESERVED -> RESERVED, 
   * and PENDING/EXCLUSIVE -> UNLOCKED (after the EXCLUSIVE is released)
   */
  const bool acquireDLE(LockWrapper* cur, Semantic s) {
    switch (s) {
    case UNLOCKED:
      throw std::runtime_error(std::string("illegal acquires: UNLOCKED"));
    case SHARED:
      cur->m.lock_shared();
      return true;
    case RESERVED:
      cur->r.lock();
      return true;
    case PENDING:
      throw std::runtime_error(
        std::string("illegal acquire: PENDING is implicitly handled while acquiring EXCLUSIVE"));
    case EXCLUSIVE:
      cur->m.lock();
      return true;
    }

    return false;
  }

  /**
   * this method lets a txn/thread to release DLE locks
   * DLE uses r for RESERVED, and m for SHARED, PENDING, and EXCLUSIVE 
   */
  const bool releaseDLE(int key, Semantic s) {
    LockWrapper* curLock = &lockManager.find(key)->second;
    switch (s) {
      case SHARED:
        curLock->m.unlock_shared();
        return true;
      case EXCLUSIVE:
        curLock->m.unlock();
        curLock->r.unlock();
        return true;
      default:
        std::cout << "Unhandled lock mode " << Semantic(curLock->semantic) << " \n";
        return false;
    }
  }

  /**
   * check the lock mode of a pair in lockManager based on the given key
   */
  const void check(int key) {
    LockWrapper* curLock = &lockManager.find(key)->second;
    std::cout << "key: " << key << ", lock: " << Semantic(curLock->semantic) << std::endl;
  }
}