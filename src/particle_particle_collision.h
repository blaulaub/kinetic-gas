#pragma once

#include "collision_event.h"

class Particle;

class ParticleParticleCollision : public CollisionEvent
{
public:
  Particle &particle1;
  Particle &particle2;

  ParticleParticleCollision(
    double time,
    Particle &particle1,
    Particle &particle2
  ): CollisionEvent(time), particle1(particle1), particle2(particle2) {}

  void process() override;
};
