#pragma once

namespace littleBadger {
  // concurrency control algroithm: deferredLockEnforecment, traditional
  enum CCAlg {DLE, TRAD};
  // solution for dead lock: waitForever, killImediately
  enum DLSol {WAIT, KILL};

  CCAlg alg = TRAD;
  DLSol sol = KILL;
  int sleepCount = 10;
}