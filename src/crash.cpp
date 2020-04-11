
#include <array>
#include <random>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <list>
#include <optional>

#include <cmath>

using namespace std;

mt19937_64 rng;
uniform_real_distribution<double> unif(0, 1);

struct Particle
{
  array<double,3> position;
  array<double,3> velocity;
};

struct Wall
{
  array<double,3> norm;
  double offset;
};

struct Collision
{
  double time;
  Collision(double time): time(time) {}
};

bool operator <(const Collision &a, const Collision &b)
{
  return a.time < b.time;
};

struct WallParticleCollision : public Collision
{
  Wall &wall;
  Particle &particle;
  WallParticleCollision(
    double time,
    Wall &wall,
    Particle &particle
  ): Collision(time), wall(wall), particle(particle) {}
};

optional<WallParticleCollision> ofNext()
{
  return nullopt;
}

struct ParticleParticleCollision : public Collision
{
  Particle &particle1;
  Particle &particle2;
  ParticleParticleCollision(
    double time,
    Particle &particle1,
    Particle &particle2
  ): Collision(time), particle1(particle1), particle2(particle2) {}
};

void collide(Wall wall1, Particle part1)
{
  double v1 = part1.velocity[0];
  double v2 = part1.velocity[1];
  double v3 = part1.velocity[2];
  double w1 = wall1.norm[0];
  double w2 = wall1.norm[1];
  double w3 = wall1.norm[2];
  double k1 = v1*w1 + v2*w2 + v3*w3;
  part1.velocity = {
    v1 - 2*k1*w1,
    v2 - 2*k1*w2,
    v3 - 2*k1*w3
  };
  // TODO update next collisions for part1
}

void collide(Particle part1, Particle part2)
{
  double v11 = part1.velocity[0];
  double v12 = part1.velocity[1];
  double v13 = part1.velocity[2];
  double v21 = part2.velocity[0];
  double v22 = part2.velocity[1];
  double v23 = part2.velocity[2];
  double x1 = part1.position[0] - part2.position[0];
  double x2 = part1.position[1] - part2.position[1];
  double x3 = part1.position[2] - part2.position[2];
  double k1 = x1*x1 + x2*x2 + x3*x3;
  double k2 = 1./sqrt(k1);
  double w1 = x1*k2;
  double w2 = x2*k2;
  double w3 = x3*k2;
  double k4 = w1*(v11-v21);
  double k5 = w2*(v12-v22);
  double k6 = w3*(v13-v23);
  // TODO the signs could be wrong
  part1.velocity = {
    v11 - k4,
    v12 - k5,
    v13 - k6
  };
  part2.velocity = {
    v21 - k4,
    v22 - k5,
    v23 - k6
  };
  // TODO update next collisions for part1
  // TODO update next collisions for part2
}

int main(int argc, char** args)
{
  uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
  std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
  rng.seed(ss);

  const double max_x = 1.;
  const double max_y = 1.;
  const double max_z = 1.;
  const double max_v = 1.;
  const double r = 0.02;

  vector<Wall> walls = {
    { { 1., 0., 0.}, 0. },
    { {-1., 0., 0.}, 1. },
    { { 0., 1., 0.}, 0. },
    { { 0.,-1., 0.}, 1. },
    { { 0., 0., 1.}, 0. },
    { { 0., 0.,-1.}, 1. }
  };

  vector<Particle> particles(100);

  // initialize
  for(int i = 0; i < particles.size(); i++)
  {
    auto &part1 = particles[i];
    while (true) {
      part1.position[0] = r + (max_x-2*r) * unif(rng);
      part1.position[1] = r + (max_y-2*r) * unif(rng);
      part1.position[2] = r + (max_z-2*r) * unif(rng);
      bool accept = true;
      for(int j = 0; j < i; j++)
      {
        auto& part2 = particles[j];
        double x1 = part1.position[0] - part2.position[0];
        double x2 = part1.position[1] - part2.position[1];
        double x3 = part1.position[2] - part2.position[2];
        double k3 = x1*x1 + x2*x2 + x3*x3 - 4*r*r;
        if (k3 < 0)
        {
          accept = false;
          break;
        }
      }
      if (accept) break;
    }

    while (true)
    {
      double alpha = M_PI * unif(rng);
      double r = sin(alpha);
      double b = unif(rng);
      if (b < r) {
        double beta = b/r*2*M_PI;
        part1.velocity[0] = max_v * r * cos(beta);
        part1.velocity[1] = max_v * r * sin(beta);
        part1.velocity[2] = max_v * cos(alpha);
        break;
      }
    }
  }

  // dump
  for(auto particle = particles.begin(); particle != particles.end(); particle++)
  {
    cout
      << setprecision(2)
      << particle->position[0] << " ; "
      << particle->position[1] << " ; "
      << particle->position[2] << "   "
      << particle->velocity[0] << " ; "
      << particle->velocity[1] << " ; "
      << particle->velocity[2] << "   "
      << endl;
  }

  // init wall collisions
  for(int i = 0; i < particles.size(); i++)
  {
    auto& part1 = particles[i];
    for(int j = 0; j < walls.size(); j++)
    {
      auto &wall1 = walls[j];
      double x1 = part1.position[0];
      double x2 = part1.position[1];
      double x3 = part1.position[2];
      double v1 = part1.velocity[0];
      double v2 = part1.velocity[1];
      double v3 = part1.velocity[2];
      double w1 = wall1.norm[0];
      double w2 = wall1.norm[1];
      double w3 = wall1.norm[2];
      double wo = wall1.offset;
      double k1 = w1*v1 + w2*v2 + w3*v3;
      double k2 = w1*x1 + w2*x2 + w3*x3 + wo - r;
      double k3 = -k2/k1;
      if (k3 < 0) continue;
      cout << "collision " << i << " w " << j << " in " << k3 << " sec." << endl;
      WallParticleCollision collison = { k3, wall1, part1 };
      // TODO only the earliest wall collision is of interest
    }
  }

  // init particle collisions
  for(int i = 0; i < particles.size()-1; i++)
  {
    auto& part1 = particles[i];
    for(int j = i+1; j < particles.size(); j++)
    {
      auto &part2 = particles[j];
      double x1 = part1.position[0] - part2.position[0];
      double x2 = part1.position[1] - part2.position[1];
      double x3 = part1.position[2] - part2.position[2];
      double v1 = part1.velocity[0] - part2.velocity[0];
      double v2 = part1.velocity[1] - part2.velocity[1];
      double v3 = part1.velocity[2] - part2.velocity[2];
      double k1 = v1*x1 + v2*x2 + v3*x3;
      double k2 = v1*v1 + v2*v2 + v3*v3;
      double k3 = x1*x1 + x2*x2 + x3*x3 - 4*r*r;
      double k4 = 1./k2;
      double k5 = -k1*k4;
      double k6 = -k3*k4;
      double k7 = (k5*k5 + k6);
      if (k7 < 0) continue;
      double k8 = k5 - sqrt(k7);
      if (k8 < 0) continue;
      cout << "collision " << i << " x " << j << " in " << k8 << " sec." << endl;
      ParticleParticleCollision collision = { k8, part1, part2 };
    }
  }

  return 0;
}
