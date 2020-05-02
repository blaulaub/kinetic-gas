#pragma once

#include <vector>

#include "./timed_events/timed_event.h"

class Renderer;
class Particle;

class FrameEvent: public TimedEvent
{
private:
  Renderer &renderer;
  std::vector<Particle> &particles;

public:

  FrameEvent(
    double time,
    Renderer &renderer,
    std::vector<Particle> &particles
  )
  : TimedEvent(time),
    renderer(renderer),
    particles(particles)
  {}

  void process() override;
};
