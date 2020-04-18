#pragma once

#include <optional>

class CollisionPredictor
{
private:

  // TODO this could become a property of particle, not a class constant
  double _r;

public:

  CollisionPredictor(double r) : _r(r) {}

  std::optional<WallParticleCollision> ofNext(Wall &wall1, Particle &part1)
  {
    double x1 = part1.position[0];
    double x2 = part1.position[1];
    double x3 = part1.position[2];
    double w1 = wall1.norm[0];
    double w2 = wall1.norm[1];
    double w3 = wall1.norm[2];
    double wo = wall1.offset;
    double k1 = wall1.norm * part1.velocity;
    if (k1 >= 0) return std::nullopt;
    double k2 = w1*x1 + w2*x2 + w3*x3 + wo - _r;
    double k3 = -k2/k1;
    if (k3 <= 0) return std::nullopt;
    return WallParticleCollision(k3, wall1, part1);
  }

  std::optional<ParticleParticleCollision> ofNext(Particle &part1, Particle &part2)
  {
    double x1 = part1.position[0] - part2.position[0];
    double x2 = part1.position[1] - part2.position[1];
    double x3 = part1.position[2] - part2.position[2];
    double v1 = part1.velocity[0] - part2.velocity[0];
    double v2 = part1.velocity[1] - part2.velocity[1];
    double v3 = part1.velocity[2] - part2.velocity[2];
    double k1 = v1*x1 + v2*x2 + v3*x3;
    if (k1 >= 0) return std::nullopt;
    double k2 = v1*v1 + v2*v2 + v3*v3;
    double k3 = x1*x1 + x2*x2 + x3*x3 - 4*_r*_r;
    double k4 = 1./k2;
    double k5 = -k1*k4;
    double k6 = -k3*k4;
    double k7 = (k5*k5 + k6);
    if (k7 <= 0) return std::nullopt;
    double k8 = k5 - sqrt(k7);
    if (k8 <= 0) return std::nullopt;
    return ParticleParticleCollision(k8, part1, part2);
  }
};
