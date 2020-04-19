#pragma once

#include <vector>
#include <random>
#include <memory>

#include "particle.h"
#include "util/uniform_unit_random_source.h"

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

      while (true)
      {
        double alpha = twoDee ? M_PI/2 : M_PI * uniformUnitRandomSource.next();
        double r = sin(alpha);
        double b = uniformUnitRandomSource.next();
        if (b < r) {
          double beta = b/r*2*M_PI;
          part1.velocity[0] = max_v * r * cos(beta);
          part1.velocity[1] = max_v * r * sin(beta);
          part1.velocity[2] = max_v * cos(alpha);
          break;
        }
      }
    }

    return particles;
  }

};
