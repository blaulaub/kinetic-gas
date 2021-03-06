#pragma once

#include <array>

#include "dynamic_object.h"
#include "../util/math_types/vec.h"

class Wall: public DynamicObject
{
public:
  Vec norm;
  double offset;
public:
  Wall(Vec norm, double offset)
  : norm(norm), offset(offset)
  {}
};
