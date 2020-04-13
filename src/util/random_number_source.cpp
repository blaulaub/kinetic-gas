#include <random>
#include <chrono>

#include "random_number_source.h"

RandomNumberSource::RandomNumberSource()
: _rng(), _unif(0,1)
{
  uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
  std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
  _rng.seed(ss);
}
