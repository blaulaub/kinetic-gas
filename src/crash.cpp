#include <optional>
#include <tuple>

#include "./dynamic_objects/particle.h"
#include "./dynamic_objects/wall.h"
#include "./timed_events/collision_events/particle_particle_collision.h"
#include "./timed_events/collision_events/wall_particle_collision.h"
#include "./util/value_sources/constant_source.h"
#include "./util/value_sources/random_ranged_double_source.h"
#include "./util/value_sources/random_unit_velocity_source.h"
#include "./util/value_sources/uniform_unit_random_source.h"
#include "particle_factory.h"
#include "wall_factory.h"
#include "frame_event.h"
#include "collision_predictor.h"
#include "renderer/renderer.h"

using namespace std;

const double max_v = 1.;

const bool twoDee = true;


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

enum Event { WALL_PARTICLE, PARTICLE_PARTICLE, FRAME };

typedef tuple<Event, double> TE;

int main(int argc, char** args)
{
  const auto extend = 1.;

  vector<Wall> walls = wallFactory.originCubicWalls(extend);
  vector<Particle> particles = ParticleFactory(twoDee).inOriginCubicle(
    extend,
    ConstantSource(0.02),
    RandomUnitVelocitySource(max_v, twoDee),
    100
  );

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

    TE deltaT;

    deltaT = nextParticleCollision && nextParticleCollision.value().time < nextWallParticleCollision.value().time
      ? (TE){ WALL_PARTICLE, nextParticleCollision.value().time }
      : (TE){ PARTICLE_PARTICLE, nextWallParticleCollision.value().time };

    double timeToNextFrame = nextFrameTime - time;
    deltaT = timeToNextFrame < get<1>(deltaT)
      ? (TE){ FRAME, timeToNextFrame }
      : deltaT;

    // advance particles
    advance(particles, get<1>(deltaT));
    time += get<1>(deltaT);

    // update collision
    switch(get<0>(deltaT))
    {
      case WALL_PARTICLE:
        nextParticleCollision.value().process();
        break;
      case PARTICLE_PARTICLE:
        nextWallParticleCollision.value().process();
        break;
      case FRAME:
        FrameEvent(time, renderer, particles).process();
        nextFrameTime += 1./FPS;
        break;
      default:
        exit(-1);
    }
  }

  return 0;
}
