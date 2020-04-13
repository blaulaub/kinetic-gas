
#include <array>
#include <random>
#include <chrono>
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
#include "particle_factory.h"
#include "random_number_source.h"

using namespace std;

RandomNumberSource rns;

const double max_x = 1.;
const double max_y = 1.;
const double max_z = 1.;
const double max_v = 1.;
const double r = 0.02;

const bool twoDee = true;

ParticleFactory particleFactory(max_v, r, twoDee);

vector<Wall> originCubicWalls(double extend)
{
  return {
    { { 1., 0., 0.},     0. },
    { {-1., 0., 0.}, extend },
    { { 0., 1., 0.},     0. },
    { { 0.,-1., 0.}, extend },
    { { 0., 0., 1.},     0. },
    { { 0., 0.,-1.}, extend }
  };
}

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

optional<WallParticleCollision> ofNext(Wall &wall1, Particle &part1)
{
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
  if (k1 >= 0) return nullopt;
  double k2 = w1*x1 + w2*x2 + w3*x3 + wo - r;
  double k3 = -k2/k1;
  if (k3 <= 0) return nullopt;
  return WallParticleCollision(k3, wall1, part1);
}

optional<WallParticleCollision> ofNext(vector<Wall> &walls, vector<Particle> &particles)
{
  tuple<bool, double, Wall*, Particle*> next = make_tuple(false, 0, nullptr, nullptr);
  for(int i = 0; i < particles.size(); i++)
  {
    for(int j = 0; j < walls.size(); j++)
    {
      auto optCollision = ofNext(walls[j], particles[i]);
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

optional<ParticleParticleCollision> ofNext(Particle &part1, Particle &part2)
{
  double x1 = part1.position[0] - part2.position[0];
  double x2 = part1.position[1] - part2.position[1];
  double x3 = part1.position[2] - part2.position[2];
  double v1 = part1.velocity[0] - part2.velocity[0];
  double v2 = part1.velocity[1] - part2.velocity[1];
  double v3 = part1.velocity[2] - part2.velocity[2];
  double k1 = v1*x1 + v2*x2 + v3*x3;
  if (k1 >= 0) return nullopt;
  double k2 = v1*v1 + v2*v2 + v3*v3;
  double k3 = x1*x1 + x2*x2 + x3*x3 - 4*r*r;
  double k4 = 1./k2;
  double k5 = -k1*k4;
  double k6 = -k3*k4;
  double k7 = (k5*k5 + k6);
  if (k7 <= 0) return nullopt;
  double k8 = k5 - sqrt(k7);
  if (k8 <= 0) return nullopt;
  return ParticleParticleCollision(k8, part1, part2);
}

optional<ParticleParticleCollision> ofNext(vector<Particle> &particles)
{
  tuple<bool, double, Particle*, Particle*> next = make_tuple(false, 0, nullptr, nullptr);
  for(int i = 0; i < particles.size()-1; i++)
  {
    for(int j = i+1; j < particles.size(); j++)
    {
      auto optCollision = ofNext(particles[i], particles[j]);
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
  // TODO the signs could be wrong
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
  vector<Wall> walls = originCubicWalls(1.);

  vector<Particle> particles = particleFactory.inOriginCubicle(rns, 1., 100);

  const auto FPS = 50;

  // avcodec_register_all();
  AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_MPEG2VIDEO);
  if (!codec) {
    fprintf(stderr, "codec not found\n");
    exit(1);
  }

  AVCodecContext *c = avcodec_alloc_context3(codec);

  c->bit_rate = 400000;
  c->width = 800;
  c->height = 800;
  c->time_base = (AVRational){1,FPS};
  c->framerate = (AVRational){FPS,1};
  c->gop_size = 10; /* emit one intra frame every ten frames */
  c->max_b_frames=1;
  c->pix_fmt = AV_PIX_FMT_YUV420P;

  AVFrame *yuvpic = av_frame_alloc();
  yuvpic->format = c->pix_fmt;
  yuvpic->width  = c->width;
  yuvpic->height = c->height;
  if (av_frame_get_buffer(yuvpic, 32) < 0) {
    fprintf(stderr, "could not alloc the YUV frame data\n");
    exit(1);
  }

  AVFrame *rgbpic = av_frame_alloc();
  rgbpic->format = AV_PIX_FMT_RGB24;
  rgbpic->width  = c->width;
  rgbpic->height = c->height;
  if (av_frame_get_buffer(rgbpic, 32) < 0) {
    fprintf(stderr, "could not alloc the RGB frame data\n");
    exit(1);
  }

  SwsContext *sws = sws_getContext(
    yuvpic->width, yuvpic->height, AV_PIX_FMT_RGB24,
    yuvpic->width, yuvpic->height, AV_PIX_FMT_YUV420P,
    0, 0, 0, 0
  );

  if (avcodec_open2(c, codec, NULL) < 0) {
    fprintf(stderr, "could not open codec\n");
    exit(1);
  }

  FILE *f = fopen("crash.mpg", "wb");
  if (!f) {
    fprintf(stderr, "could not open %s\n", "crash.mpg");
    exit(1);
  }

  AVPacket *avpkt = av_packet_alloc();
  if (!avpkt) {
    fprintf(stderr, "could not alloc pkt\n");
    exit(1);
  }

  cairo_surface_t *surface = cairo_image_surface_create(
    CAIRO_FORMAT_RGB24, rgbpic->width, rgbpic->height);
  cairo_t * const cr = cairo_create(surface);

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
      cairo_set_source_rgb(cr, 1., 1., 1.);
      cairo_paint(cr);

      cairo_set_source_rgb(cr, 1., 0., 0.);
      cairo_set_line_width(cr, 1);
      for(int i = 0; i < particles.size(); i++)
      {
        cairo_arc(cr,
          particles[i].position[0]*800,
          particles[i].position[1]*800,
          r*800, 0, 2*M_PI);
        cairo_close_path(cr);
        cairo_stroke_preserve(cr);
        cairo_fill(cr);
        // cairo_stroke_preserve(cr);
        // cairo_fill(cr);
      }

      uint8_t *s = cairo_image_surface_get_data(surface);
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
      encode(c, yuvpic, avpkt, f);
      nextFrameTime += 1./FPS;
    }
    else
    {
      collide(nextWallParticleCollision.value().wall, nextWallParticleCollision.value().particle);
    }
  }

  encode(c, NULL, avpkt, f);
  fclose(f);

  avcodec_free_context(&c);
  av_frame_free(&yuvpic);
  av_packet_free(&avpkt);

  return 0;
}
