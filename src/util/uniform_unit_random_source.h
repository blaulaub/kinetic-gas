#pragma once

#include "value_source.h"

///
/// Provides uniformly distributed random numbers in the range [0., 1.)
///
class UniformUnitRandomSource: public ValueSource<double>
{
public:
  double next() override final;
};
