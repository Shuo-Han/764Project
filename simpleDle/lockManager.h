#pragma once

#include <shared_mutex>
#include <mutex>

/**
 * Difference between a class and a struct in C++:
 * structs have default public members and bases and classes have default private members and 
 * bases. Both classes and structs can have a mixture of public and private members, can use 
 * inheritance, and can have member functions. I would recommend using structs as plain-old-data 
 * structures without any class-like features, and using classes as aggregate data structures with 
 * private data and member functions.
 */

namespace littleBadger {
  // semantics for locks, RESERVED and PENDDING are for DLE 
  enum Semantic {UNLOCKED, SHARED, RESERVED, PENDING, EXCLUSIVE};

  /**
   * this class helps us to build DLE lock mechanism, SHARED modes can coexist with RESERVED mode,
   * when RENDING mode is acquired, remaining SHARED modes will be purged. Note that the transition
   * between SHARED modes and EXCLUSIVE mode is actually PENDING mode, once all SHARED modes are
   * left, the EXCLUSIVE mode is immedately acquired.
   */
  class LockWrapper {
  public:
    // indicate the current mode of this lock
    Semantic semantic;
    // serves SHARED, PENDING, and EXCLUSIVE mode
    std::shared_timed_mutex m;
    // serves for RESERVED, block other RESERVED acquisitions while allowing more incoming SHARED
    std::mutex r;
    int sharedRefCount;

    LockWrapper();

  private:
    
  };

  // init lockManager
  const void initLockManager(int numRecord);

  // acquire a lock for a record
  const bool acquire(int key, Semantic s);

  // helpers for acquire() 
  const bool acquireTRAD(LockWrapper* cur, Semantic s);
  const bool acquireDLE(LockWrapper* cur, Semantic s);

  // release a lock of a record
  const bool release(int key, Semantic s);

  // helpers for release() 
  const bool releaseTRAD(int key, Semantic s);
  const bool releaseDLE(int key, Semantic s);

  // check lock status of a record
  const void check(int key);
}