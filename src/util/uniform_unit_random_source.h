#pragma once

#include <random>

#include "random_source.h"

///
/// Provides uniformly distributed random numbers in the range [0., 1.)
///
class UniformUnitRandomSource: public RandomSource<double>
{
private:

  std::mt19937_64 _rng;
  std::uniform_real_distribution<double> _unif;

public:

  UniformUnitRandomSource();

  double next() override
  {
    return _unif(_rng);
  }
};
