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
    double k1 = wall1.norm * part1.velocity;
    if (k1 >= 0) return std::nullopt;
    double k2 = wall1.norm * part1.position + wall1.offset - _r;
    double k3 = -k2/k1;
    if (k3 <= 0) return std::nullopt;
    return WallParticleCollision(k3, wall1, part1);
  }

  std::optional<ParticleParticleCollision> ofNext(Particle &part1, Particle &part2)
  {
    auto x = part1.position - part2.position;
    auto v = part1.velocity - part2.velocity;
    double k1 = v * x;
    if (k1 >= 0) return std::nullopt;
    double k2 = v.norm();
    double k3 = x.norm() - 4*_r*_r;
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
