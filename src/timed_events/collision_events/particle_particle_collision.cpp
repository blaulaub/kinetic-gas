#include "particle_particle_collision.h"

#include <math.h>

#include "../../dynamic_objects/particle.h"

void ParticleParticleCollision::process()
{
  auto positionDelta = particle1.position - particle2.position;
  double k1 = positionDelta.normSquare();
  double k2 = 1./sqrt(k1);
  auto w1 = positionDelta * k2;
  auto v1 = particle1.velocity - particle2.velocity;
  double k3 = v1 * w1;
  auto k4 = w1 * k3;
  particle1.velocity = particle1.velocity - k4;
  particle2.velocity = particle2.velocity + k4;
}
