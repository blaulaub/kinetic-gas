#include "math.h"

extern "C" {
  #include <cairo/cairo.h>
}

#include "bitmap_renderer.h"

BitmapRenderer::BitmapRenderer(int width, int height, int scale)
: _scale(scale),
  surface(cairo_image_surface_create(CAIRO_FORMAT_RGB24, width, height)),
  cr(cairo_create(surface))
{}

uint8_t* BitmapRenderer::render(const std::vector<Particle> &particles)
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
      particles[i].radius*_scale, 0, 2*M_PI);
    cairo_close_path(cr);
    cairo_stroke_preserve(cr);
    cairo_fill(cr);
  }

  return cairo_image_surface_get_data(surface);
}
