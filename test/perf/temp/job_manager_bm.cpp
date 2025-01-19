#include <fstream>
#include <iostream>
#include <string>

#include "test/perf/temp/job_manager.h"

signed main() {
  std::FILE *file =
      std::fopen("/home/vspm/dev/hermes/test/perf/temp/job.txt", "r");

  JobManager manager;
  manager.Init(file);
  manager.Report();
}
