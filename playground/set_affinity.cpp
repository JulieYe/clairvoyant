#include <sched.h>
#include <cstdlib>
using namespace std;

int main(int argc, char *argv[]) {
  int cpuAffinity = argc > 1 ? atoi(argv[1]) : -1;

  if (cpuAffinity > -1) {
    cpu_set_t mask;
    int status;
    CPU_ZERO(&mask);
    CPU_SET(cpuAffinity, &mask);
    status = sched_setaffinity(0, sizeof(mask), &mask);
    if (status != 0) {
      perror("sched_setaffinity");
    }
  }
  
  return 0;
}
