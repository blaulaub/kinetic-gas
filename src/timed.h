#pragma once

struct Timed
{
  const double time;
  Timed(double time): time(time) {}
};

bool operator <(const Timed &a, const Timed &b)
{
  return a.time < b.time;
};
