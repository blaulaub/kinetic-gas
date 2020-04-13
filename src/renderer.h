#pragma once

#include <vector>
#include <string>

extern "C" {
  #include <libavcodec/avcodec.h>
  #include <libswscale/swscale.h>
}

#include "bitmap_renderer.h"
#include "video_file_output.h"
#include "particle.h"

class Renderer
{
private:

  VideoFileOutput videoFileOutput;
  BitmapRenderer bitmapRenderer;
  AVFrame *rgbpic;
  AVFrame *yuvpic;
  SwsContext *sws;

public:

  Renderer(const std::string& filename, int width, int height, int scale, int fps);

  ~Renderer();

  void render(
    // TODO radius r should be a particle property
    double r,
    const std::vector<Particle> &particles
  );
};
