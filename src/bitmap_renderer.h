#pragma once

#include <math.h>
#include <vector>
#include "particle.h"

class BitmapRenderer
{
private:

  int _scale;

  cairo_surface_t *surface;
  cairo_t * const cr;

public:

  BitmapRenderer(int width, int height, int scale)
  : _scale(scale),
    surface(cairo_image_surface_create(CAIRO_FORMAT_RGB24, width, height)),
    cr(cairo_create(surface))
  {}

  uint8_t* render(
    // TODO radius r should be a particle property
    const double r,
    const std::vector<Particle> &particles
  )
  {
    cairo_set_source_rgb(cr, 1., 1., 1.);
    cairo_paint(cr);

    cairo_set_source_rgb(cr, 1., 0., 0.);
    cairo_set_line_width(cr, 1);
    for(int i = 0; i < particles.size(); i++)
    {
      cairo_arc(cr,
        particles[i].position[0]*_scale,
        particles[i].position[1]*_scale,
        r*_scale, 0, 2*M_PI);
      cairo_close_path(cr);
      cairo_stroke_preserve(cr);
      cairo_fill(cr);
    }

    return cairo_image_surface_get_data(surface);
  }

};
