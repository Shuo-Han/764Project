#pragma once

// concurrency control algroithm: deferredLockEnforecment, traditional
enum CCAlg {DLE, TRAD};
// solution for dead lock: waitForever, killImediately
enum DLSol {WAIT, KILL};

extern CCAlg alg;
extern DLSol sol;
extern int numKeys;
extern int sleepCount; // a simulator for real duration of read phase
