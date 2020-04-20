#pragma once

#include "particle.h"
#include "timed_event.h"
#include "wall.h"

class WallParticleCollision : public TimedEvent
{
public:
  Wall &wall;
  Particle &particle;
  WallParticleCollision(
    double time,
    Wall &wall,
    Particle &particle
  ): TimedEvent(time), wall(wall), particle(particle) {}
};
