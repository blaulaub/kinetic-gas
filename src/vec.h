#pragma once

#include <array>

class Vec: public std::array<double,3>
{
public:
  friend double operator*(Vec &v1, Vec &v2)
  {
    return v1[0]*v2[0] + v1[1]*v2[1]+ v1[2]*v2[2];
  }
  friend Vec operator-(Vec &v1, Vec &v2)
  {
    return {v1[0]-v2[0], v1[1]-v2[1], v1[2]-v2[2]};
  }
};
