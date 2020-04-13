#pragma once

#include <stdint.h>

extern "C" {
  #include <cairo/cairo.h>
}

#include <vector>
#include "particle.h"

class BitmapRenderer
{
private:

  int _scale;

  cairo_surface_t *surface;
  cairo_t * const cr;

public:

  BitmapRenderer(int width, int height, int scale);

  // TODO a desctructor is needed

  uint8_t* render(
    // TODO radius r should be a particle property
    const double r,
    const std::vector<Particle> &particles
  );
};
