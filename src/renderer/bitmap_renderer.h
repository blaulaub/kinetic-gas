#pragma once

#include <stdint.h>

extern "C" {
  #include <cairo/cairo.h>
}

#include <vector>
#include "../dynamic_objects/particle.h"

class BitmapRenderer
{
private:

  int _scale;

  cairo_surface_t *surface;
  cairo_t * const cr;

public:

  BitmapRenderer(int width, int height, int scale);

  // TODO a desctructor is needed

  uint8_t* render(const std::vector<Particle> &particles);
};
