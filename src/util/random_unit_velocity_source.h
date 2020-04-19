#pragma once

#include "math.h"

#include "value_source.h"
#include "uniform_unit_random_source.h"
#include "../vec.h"

///
/// Provides velocity vectors with equal spatial distribution.
///
class RandomUnitVelocitySource: public ValueSource<Vec>
{
private:

  double velocity;
  bool twoDee;

public:

  RandomUnitVelocitySource(double velocity, bool twoDee): velocity(velocity), twoDee(twoDee) {}

  Vec next() override final
  {
    UniformUnitRandomSource uniformUnitRandomSource;
    while (true)
    {
      double alpha = twoDee ? M_PI/2 : M_PI * uniformUnitRandomSource.next();
      double r = sin(alpha);
      double b = uniformUnitRandomSource.next();
      if (b < r) {
        double beta = b/r*2*M_PI;
        return {
          velocity * r * cos(beta),
          velocity * r * sin(beta),
          velocity * cos(alpha)
        };
      }
    }
  }
};
