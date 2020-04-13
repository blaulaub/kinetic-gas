#pragma once

#include <random>

class RandomNumberSource
{
private:

  std::mt19937_64 _rng;
  std::uniform_real_distribution<double> _unif;

public:

  RandomNumberSource();

  double unif()
  {
    return _unif(_rng);
  }
};
