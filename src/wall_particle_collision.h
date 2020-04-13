#pragma once

#include "particle.h"
#include "timed.h"
#include "wall.h"

struct WallParticleCollision : public Timed
{
  Wall &wall;
  Particle &particle;
  WallParticleCollision(
    double time,
    Wall &wall,
    Particle &particle
  ): Timed(time), wall(wall), particle(particle) {}
};
