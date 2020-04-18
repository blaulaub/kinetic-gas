#pragma once

#include <array>

class Vec: public std::array<double,3>
{
public:
  friend double operator*(const Vec &v1, const Vec &v2)
  {
    return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
  }
  friend Vec operator*(double k1, const Vec &v2)
  {
    return {k1*v2[0], k1*v2[1], k1*v2[2]};
  }
  friend Vec operator*(const Vec &v1, double k2)
  {
    return {v1[0]*k2, v1[1]*k2, v1[2]*k2};
  }
  friend Vec operator-(const Vec &v1, const Vec &v2)
  {
    return {v1[0]-v2[0], v1[1]-v2[1], v1[2]-v2[2]};
  }
};
