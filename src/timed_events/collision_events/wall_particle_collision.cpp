#include "wall_particle_collision.h"

void WallParticleCollision::process()
{
  double k1 = particle.velocity * wall.norm;
  particle.velocity = particle.velocity - 2 * k1 * wall.norm;
}
