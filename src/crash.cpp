#include <optional>
#include <tuple>

#include "particle.h"
#include "wall.h"
#include "util/uniform_unit_random_source.h"
#include "util/random_unit_velocity_source.h"
#include "particle_factory.h"
#include "wall_factory.h"
#include "particle_particle_collision.h"
#include "wall_particle_collision.h"
#include "collision_predictor.h"
#include "renderer/renderer.h"

using namespace std;

const double max_v = 1.;

const bool twoDee = true;

ParticleFactory particleFactory(max_v, 0.02, twoDee);

WallFactory wallFactory;

CollisionPredictor collisionPredictor;

void advance(vector<Particle> &particles, double deltaT)
{
  for(int i = 0; i < particles.size(); i++)
  {
    particles[i].position += particles[i].velocity * deltaT;
  }
}

// TODO since this computes over all elements, it is only ok for initialization but bad for book-keeping
optional<WallParticleCollision> ofNext(vector<Wall> &walls, vector<Particle> &particles)
{
  tuple<bool, double, Wall*, Particle*> next = make_tuple(false, 0, nullptr, nullptr);
  for(int i = 0; i < particles.size(); i++)
  {
    for(int j = 0; j < walls.size(); j++)
    {
      auto optCollision = collisionPredictor.ofNext(walls[j], particles[i]);
      if (optCollision)
      {
        WallParticleCollision collision = optCollision.value();
        if (!get<0>(next) || get<1>(next) > collision.time)
        {
          next = make_tuple(true, collision.time, &collision.wall, &collision.particle);
        }
      }
    }
  }
  if (get<0>(next)) return WallParticleCollision(get<1>(next), *(get<2>(next)), *(get<3>(next)));
  return nullopt;
}

// TODO since this computes over all elements, it is only ok for initialization but bad for book-keeping
optional<ParticleParticleCollision> ofNext(vector<Particle> &particles)
{
  tuple<bool, double, Particle*, Particle*> next = make_tuple(false, 0, nullptr, nullptr);
  for(int i = 0; i < particles.size()-1; i++)
  {
    for(int j = i+1; j < particles.size(); j++)
    {
      auto optCollision = collisionPredictor.ofNext(particles[i], particles[j]);
      if (optCollision)
      {
        ParticleParticleCollision collision = optCollision.value();
        if (!get<0>(next) || get<1>(next) > collision.time)
        {
          next = make_tuple(true, collision.time, &collision.particle1, &collision.particle2);
        }
      }
    }
  }
  if (get<0>(next)) return ParticleParticleCollision(get<1>(next), *(get<2>(next)), *(get<3>(next)));
  return nullopt;
}

void collide(Wall &wall1, Particle &part1)
{
  double k1 = part1.velocity * wall1.norm;
  part1.velocity = part1.velocity - 2 * k1 * wall1.norm;
}

void collide(Particle &part1, Particle &part2)
{
  auto x = part1.position - part2.position;
  double k1 = x.normSquare();
  double k2 = 1./sqrt(k1);
  auto w1 = x * k2;
  auto v1 = part1.velocity - part2.velocity;
  double k3 = v1 * w1;
  auto k4 = w1 * k3;
  part1.velocity = part1.velocity - k4;
  part2.velocity = part2.velocity + k4;
}

enum Event { WALL_PARTICLE, PARTICLE_PARTICLE, FRAME };

typedef tuple<Event, double> TimedEvent;

int main(int argc, char** args)
{
  const auto extend = 1.;

  vector<Wall> walls = wallFactory.originCubicWalls(extend);
  vector<Particle> particles = particleFactory.inOriginCubicle(extend, RandomUnitVelocitySource(max_v, twoDee), 100);

  const auto FPS = 50;
  const int width = 800;
  const int height = 800;
  Renderer renderer("crash.mpg", width, height, 800, FPS);

  double time = 0.;
  double nextFrameTime = 0.;
  while (time < 20.)
  {
    auto nextWallParticleCollision = ofNext(walls, particles);
    auto nextParticleCollision = ofNext(particles);

    // TODO computing next collision each time over is a) inefficient and b) could be inaccurate

    TimedEvent deltaT;

    deltaT = nextParticleCollision && nextParticleCollision.value().time < nextWallParticleCollision.value().time
      ? (TimedEvent){ WALL_PARTICLE, nextParticleCollision.value().time }
      : (TimedEvent){ PARTICLE_PARTICLE, nextWallParticleCollision.value().time };

    double timeToNextFrame = nextFrameTime - time;
    deltaT = timeToNextFrame < get<1>(deltaT)
      ? (TimedEvent){ FRAME, timeToNextFrame }
      : deltaT;

    // advance particles
    advance(particles, get<1>(deltaT));
    time += get<1>(deltaT);

    // update collision
    switch(get<0>(deltaT))
    {
      case WALL_PARTICLE:
        collide(nextParticleCollision.value().particle1, nextParticleCollision.value().particle2);
        break;
      case PARTICLE_PARTICLE:
        collide(nextWallParticleCollision.value().wall, nextWallParticleCollision.value().particle);
        break;
      case FRAME:
        renderer.render(particles);
        nextFrameTime += 1./FPS;
        break;
      default:
        exit(-1);
    }
  }

  return 0;
}
