#pragma once

#include <array>

#include "dynamic_object.h"
#include "vec.h"

class Particle: public DynamicObject
{
public:
  Vec position;
  Vec velocity;
  double radius;
};
