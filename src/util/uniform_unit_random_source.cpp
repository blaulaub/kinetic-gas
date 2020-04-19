#include <random>
#include <chrono>

#include "uniform_unit_random_source.h"

static struct UniformUnitRandomSourceStaticImpl
{
  std::mt19937_64 rng;
  std::uniform_real_distribution<double> unif;

  UniformUnitRandomSourceStaticImpl()
  {
    uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
    rng.seed(ss);
  }

  double next() { return unif(rng); }

} uniformUnitRandomSourceStaticImpl;


double UniformUnitRandomSource::next()
{
  return uniformUnitRandomSourceStaticImpl.next();
}
