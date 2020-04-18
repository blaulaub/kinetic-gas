#pragma once

#include <vector>
#include <random>

#include "particle.h"
#include "util/random_number_source.h"

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
    // TODO its a bit ugly to have RNG as parameter
    RandomNumberSource &rns,
    double extent,
    int count)
  {
    std::vector<Particle> particles(count);

    // initialize
    for(int i = 0; i < count; i++)
    {
      auto &part1 = particles[i];
      while (true) {
        part1.position[0] = r + (extent-2*r) * rns.unif();
        part1.position[1] = r + (extent-2*r) * rns.unif();
        part1.position[2] = twoDee ? extent/2 : r + (extent-2*r) * rns.unif();
        bool accept = true;
        for(int j = 0; j < i; j++)
        {
          auto& part2 = particles[j];
          auto x1 = part1.position - part2.position;
          double k3 = x1*x1 - 4*r*r;
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
        double alpha = twoDee ? M_PI/2 : M_PI * rns.unif();
        double r = sin(alpha);
        double b = rns.unif();
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
