#pragma once

#include "particle.h"
#include "timed.h"

struct ParticleParticleCollision : public Timed
{
  Particle &particle1;
  Particle &particle2;
  ParticleParticleCollision(
    double time,
    Particle &particle1,
    Particle &particle2
  ): Timed(time), particle1(particle1), particle2(particle2) {}
};
