
#include <array>
#include <random>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <list>

#include <cmath>

using namespace std;

mt19937_64 rng;
uniform_real_distribution<double> unif(0, 1);

struct Particle
{
  array<double,3> position;
  array<double,3> velocity;
};


int main(int argc, char** args)
{
  uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
  std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
  rng.seed(ss);

  vector<Particle> particles(100);

  const double max_x = 1.;
  const double max_y = 1.;
  const double max_z = 1.;
  const double max_v = 1.;
  const double r = 0.02;

  // initialize
  for(auto particle = particles.begin(); particle != particles.end(); particle++)
  {
    particle->position[0] = r + (max_x-2*r) * unif(rng);
    particle->position[1] = r + (max_y-2*r) * unif(rng);
    particle->position[2] = r + (max_z-2*r) * unif(rng);
    // TODO avoid overlap

    while (true)
    {
      double alpha = M_PI * unif(rng);
      double r = sin(alpha);
      double b = unif(rng);
      if (b < r) {
        double beta = b/r*2*M_PI;
        particle->velocity[0] = max_v * r * cos(beta);
        particle->velocity[1] = max_v * r * sin(beta);
        particle->velocity[2] = max_v * cos(alpha);
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

  // init collisions
  for(int i = 0; i < particles.size()-1; i++)
  {
    auto& part1 = particles[i];
    for(int j = i+1; j < particles.size(); j++)
    {
      auto& part2 = particles[j];
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
    }
  }



  return 0;
}
