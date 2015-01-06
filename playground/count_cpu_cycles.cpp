#include <stdint.h>
#include <iostream>

uint64_t rdtsc() {
  unsigned int lo,hi;
  __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
  return ((uint64_t)hi << 32) | lo;
}

int main() {
  std::cout << rdtsc() << std::endl;
  std::cout << rdtsc() << std::endl;
  std::cout << rdtsc() << std::endl;
  auto a = rdtsc();
  auto b = rdtsc();
  printf("%llu\n", b-a);
  return 0;
}
