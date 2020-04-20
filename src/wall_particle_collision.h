#pragma once

#include "particle.h"
#include "collision_event.h"
#include "wall.h"

class WallParticleCollision : public CollisionEvent
{
public:
  Wall &wall;
  Particle &particle;
  WallParticleCollision(
    double time,
    Wall &wall,
    Particle &particle
  ): CollisionEvent(time), wall(wall), particle(particle) {}
};
