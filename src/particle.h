#pragma once

#include <array>

#include "dynamic_object.h"

class Particle: public DynamicObject
{
public:
  std::array<double,3> position;
  std::array<double,3> velocity;
};
