#pragma once

#include "collision_event.h"
#include "../../dynamic_objects/particle.h"
#include "../../dynamic_objects/wall.h"

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

  void process() override;
};
