#pragma once

#include "timed_event.h"

class FrameEvent: public TimedEvent
{
public:

  FrameEvent(double time): TimedEvent(time) {}

  void process() override;
};
