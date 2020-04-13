#pragma once

#include <vector>

#include "wall.h"

class WallFactory
{
private:

public:

  std::vector<Wall> originCubicWalls(double extend)
  {
    return {
      { { 1., 0., 0.},     0. },
      { {-1., 0., 0.}, extend },
      { { 0., 1., 0.},     0. },
      { { 0.,-1., 0.}, extend },
      { { 0., 0., 1.},     0. },
      { { 0., 0.,-1.}, extend }
    };
  }
};
