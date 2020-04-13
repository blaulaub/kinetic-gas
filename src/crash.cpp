
#include <iostream>
#include <iomanip>
#include <list>
#include <optional>
#include <tuple>

#include <cmath>

#include <cstdlib>
#include <cstdio>
#include <cstring>

extern "C" {
  #include <libavcodec/avcodec.h>
  #include <libswscale/swscale.h>

  #include <cairo/cairo.h>
}

#include "particle.h"
#include "wall.h"
#include "random_number_source.h"
#include "particle_factory.h"
#include "wall_factory.h"
#include "timed.h"
#include "particle_particle_collision.h"
#include "wall_particle_collision.h"
#include "collision_predictor.h"
#include "video_file_output.h"
#include "bitmap_renderer.h"

using namespace std;

RandomNumberSource rns;

const double max_v = 1.;
const double r = 0.02;

const bool twoDee = true;

ParticleFactory particleFactory(max_v, r, twoDee);

WallFactory wallFactory;

CollisionPredictor collisionPredictor(r);

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
}

void collide(Particle &part1, Particle &part2)
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
  double v1 = v11 - v21;
  double v2 = v12 - v22;
  double v3 = v13 - v23;
  double k3 = v1*w1 + v2*w2 + v3*w3;
  double k4 = w1*k3;
  double k5 = w2*k3;
  double k6 = w3*k3;
  part1.velocity = {
    v11 - k4,
    v12 - k5,
    v13 - k6
  };
  part2.velocity = {
    v21 + k4,
    v22 + k5,
    v23 + k6
  };
}

void encode(AVCodecContext *c, AVFrame *yuvpic, AVPacket *avpkt, FILE *f)
{
  int ret = avcodec_send_frame(c, yuvpic);
  while (ret >= 0)
  {
    ret = avcodec_receive_packet(c, avpkt);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;
    if (ret < 0) {
      fprintf(stderr, "error while encoding\n");
      exit(1);
    }
    fwrite(avpkt->data, 1, avpkt->size, f);
    av_packet_unref(avpkt);
  }
}

enum Event { WALL_PARTICLE, PARTICLE_PARTICLE, FRAME };

int main(int argc, char** args)
{
  vector<Wall> walls = wallFactory.originCubicWalls(1.);

  vector<Particle> particles = particleFactory.inOriginCubicle(rns, 1., 100);

  const auto FPS = 50;

  const int width = 800;
  const int height = 800;
  VideoFileOutput videoFileOutput("crash.mpg", width, height, FPS);

  AVFrame *yuvpic = av_frame_alloc();
  yuvpic->format = AV_PIX_FMT_YUV420P;
  yuvpic->width  = width;
  yuvpic->height = height;
  if (av_frame_get_buffer(yuvpic, 32) < 0) {
    fprintf(stderr, "could not alloc the YUV frame data\n");
    exit(1);
  }

  AVFrame *rgbpic = av_frame_alloc();
  rgbpic->format = AV_PIX_FMT_RGB24;
  rgbpic->width  = width;
  rgbpic->height = height;
  if (av_frame_get_buffer(rgbpic, 32) < 0) {
    fprintf(stderr, "could not alloc the RGB frame data\n");
    exit(1);
  }

  SwsContext *sws = sws_getContext(
    width, height, AV_PIX_FMT_RGB24,
    rgbpic->width, rgbpic->height, AV_PIX_FMT_YUV420P,
    0, 0, 0, 0
  );

  BitmapRenderer bitmapRenderer(rgbpic->width, rgbpic->height, 800);

  double time = 0.;
  double nextFrameTime = 0.;
  while (time < 20.)
  {
    auto nextWallParticleCollision = ofNext(walls, particles);
    auto nextParticleCollision = ofNext(particles);

    // TODO computing next collision each time over is a) inefficient and b) could be inaccurate

    tuple<Event, double> deltaT;

    bool particleBeforeWall = nextParticleCollision && nextParticleCollision.value().time < nextWallParticleCollision.value().time;
    if (particleBeforeWall)
    {
      deltaT = { WALL_PARTICLE, nextParticleCollision.value().time };
    }
    else
    {
      deltaT = { PARTICLE_PARTICLE, nextWallParticleCollision.value().time };
    }

    double timeToNextFrame = nextFrameTime - time;
    if (timeToNextFrame < get<1>(deltaT))
    {
      deltaT = { FRAME, timeToNextFrame };
    }

    // advance particles
    for(int i = 0; i < particles.size(); i++)
    {
      particles[i].position[0] += particles[i].velocity[0] * get<1>(deltaT);
      particles[i].position[1] += particles[i].velocity[1] * get<1>(deltaT);
      particles[i].position[2] += particles[i].velocity[2] * get<1>(deltaT);
    }
    time += get<1>(deltaT);

    // update collision
    if (get<0>(deltaT) == WALL_PARTICLE)
    {
      collide(nextParticleCollision.value().particle1, nextParticleCollision.value().particle2);
    }
    else if (get<0>(deltaT) == FRAME )
    {
      uint8_t *s = bitmapRenderer.render(r, particles);
      uint8_t *d = rgbpic->data[0];
      for (int i = 0; i<rgbpic->height; ++i)
      {
        for (int j = 0; j<rgbpic->width; ++j)
        {
          *(d+0) = *(s+2);
          *(d+1) = *(s+1);
          *(d+2) = *(s+0);
          s+=4;
          d+=3;
        }
      }

      sws_scale(sws, rgbpic->data, rgbpic->linesize, 0, rgbpic->height, yuvpic->data, yuvpic->linesize);
      videoFileOutput.encode(yuvpic);
      nextFrameTime += 1./FPS;
    }
    else
    {
      collide(nextWallParticleCollision.value().wall, nextWallParticleCollision.value().particle);
    }
  }

  av_frame_free(&yuvpic);

  return 0;
}
