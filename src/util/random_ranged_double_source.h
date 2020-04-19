#pragma once

#include "value_source.h"
#include "uniform_unit_random_source.h"

///
/// Provides uniformly distributed random number for a given range.
///
class RandomRangedDoubleSource: public ValueSource<double>
{
private:

  double base;
  double extend;

public:

  RandomRangedDoubleSource(double lower, double upper): base(lower), extend(upper-lower) {}

  double next() override final
  {
    UniformUnitRandomSource uniformUnitRandomSource;
    return base + extend * uniformUnitRandomSource.next();
  }
};
