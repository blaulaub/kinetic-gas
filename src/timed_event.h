#pragma once

class TimedEvent
{
public:
  const double time;

  TimedEvent (double time): time(time) {}

  virtual ~TimedEvent() {}
};
