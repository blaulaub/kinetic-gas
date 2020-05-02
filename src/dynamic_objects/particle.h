#pragma once

#include <array>

#include "dynamic_object.h"
#include "../util/math_types/vec.h"

class Particle: public DynamicObject
{
public:
  Vec position;
  Vec velocity;
  double radius;
};
