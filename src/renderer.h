#pragma once

#include <vector>
#include <string>

#include "particle.h"

class Renderer
{
  class Impl;

private:

  Impl  *impl;

public:

  Renderer(const std::string& filename, int width, int height, int scale, int fps);

  ~Renderer();

  void render(
    // TODO radius r should be a particle property
    double r,
    const std::vector<Particle> &particles
  );
};
