#pragma once

#include "particle.h"
#include "timed_event.h"

class ParticleParticleCollision : public TimedEvent
{
public:
  Particle &particle1;
  Particle &particle2;
  ParticleParticleCollision(
    double time,
    Particle &particle1,
    Particle &particle2
  ): TimedEvent(time), particle1(particle1), particle2(particle2) {}
};
