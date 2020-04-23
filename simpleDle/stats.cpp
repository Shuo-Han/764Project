#include <string>
#include <chrono>
#include "stats.h"

namespace littleBadger {
  int countTxn = 0;
  double latency = 0; 
  double maxLatency = 0;
  double minLatency = 10;

  const void setLatency(double duration) {
    latency += duration;
    maxLatency = std::max(maxLatency, duration);
    minLatency = std::min(minLatency, duration);
  }

  const void getLatency() {
    printf("ttlLatency %fs, maxLatency: %fs, minLatency %fs \n", latency, maxLatency, minLatency);
  }

  double exectuionTime = 0;
  const void setThroughput(double duration) {
    countTxn++;
    exectuionTime += duration;
  }

  const void getThroughput() {
    printf("#txn: %d, totalExecutionSeconds %f ", countTxn, exectuionTime);
    // printf("throughput: %f txn/second \n", countTxn / exectuionTime);
  }
}