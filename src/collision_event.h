#pragma once

#include "./timed_events/timed_event.h"

class CollisionEvent: public TimedEvent
{
public:

  CollisionEvent(double time): TimedEvent(time) {}

  void process() override = 0;
};
