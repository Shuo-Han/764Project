#pragma once

// concurrency control algroithm: deferredLockEnforecment, traditional
enum CCAlg {DLE, TRAD};
// solution for dead lock: waitForever, killImediately
enum DLSol {WAIT, KILL};

extern CCAlg alg;
extern DLSol sol;
extern int numKeys; // control the number of records
extern int ratio; // ratio = 5 means 5 reads and 5 writes, ratio = 7 means 7 reads and 3 writes
extern int numThread; // number of threads in thread pool
extern int readCount; // a simulator for real duration in read phase
extern int writeCount; // a simulator for real duration in write phase

