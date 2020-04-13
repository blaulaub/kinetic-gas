#pragma once

#include <array>

struct Wall
{
  std::array<double,3> norm;
  double offset;
};
