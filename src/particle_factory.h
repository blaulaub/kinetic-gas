#pragma once

#include <vector>
#include <random>

#include "particle.h"
#include "util/uniform_unit_random_source.h"
#include "util/value_source.h"
#include "vec.h"

class ParticleFactory
{
private:

  const double max_v;
  const double r;
  const bool twoDee;

public:

  ParticleFactory(
    double initialVelocity,
    double particleRadius,
    bool twoDee = false
  ) : max_v(initialVelocity), r(particleRadius), twoDee(twoDee) {}

  std::vector<Particle> inOriginCubicle(
    double extent,
    ValueSource<Vec> &velocitySource,
    int count)
  {
    UniformUnitRandomSource uniformUnitRandomSource;
    std::vector<Particle> particles(count);

    // initialize
    for(int i = 0; i < count; i++)
    {
      auto &part1 = particles[i];
      part1.radius = r;
      while (true) {
        part1.position[0] = r + (extent-2*r) * uniformUnitRandomSource.next();
        part1.position[1] = r + (extent-2*r) * uniformUnitRandomSource.next();
        part1.position[2] = twoDee ? extent/2 : r + (extent-2*r) * uniformUnitRandomSource.next();
        bool accept = true;
        for(int j = 0; j < i; j++)
        {
          auto& part2 = particles[j];
          auto x1 = part1.position - part2.position;
          auto k4 = part1.radius + part2.radius;
          double k3 = x1.normSquare() - k4*k4;
          if (k3 < 0)
          {
            accept = false;
            break;
          }
        }
        if (accept) break;
      }

      part1.velocity = velocitySource.next();
    }

    return particles;
  }

};
