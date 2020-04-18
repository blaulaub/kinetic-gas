#pragma once

#include <array>

#include "dynamic_object.h"

class Wall: public DynamicObject
{
public:
  std::array<double,3> norm;
  double offset;
public:
  Wall(std::array<double,3> norm, double offset)
  : norm(norm), offset(offset)
  {}
};
