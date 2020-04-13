#include "renderer.h"

Renderer::Renderer(const std::string& filename, int width, int height, int scale, int fps)
: videoFileOutput(filename, width, height, fps),
  bitmapRenderer(width, height, 800)
{
  yuvpic = av_frame_alloc();
  yuvpic->format = AV_PIX_FMT_YUV420P;
  yuvpic->width  = width;
  yuvpic->height = height;
  if (av_frame_get_buffer(yuvpic, 32) < 0) {
    fprintf(stderr, "could not alloc the YUV frame data\n");
    exit(1);
  }

  rgbpic = av_frame_alloc();
  rgbpic->format = AV_PIX_FMT_RGB24;
  rgbpic->width  = width;
  rgbpic->height = height;
  if (av_frame_get_buffer(rgbpic, 32) < 0) {
    fprintf(stderr, "could not alloc the RGB frame data\n");
    exit(1);
  }

  sws = sws_getContext(
    yuvpic->width, yuvpic->height, AV_PIX_FMT_RGB24,
    rgbpic->width, rgbpic->height, AV_PIX_FMT_YUV420P,
    0, 0, 0, 0
  );
}

Renderer::~Renderer()
{
  av_frame_free(&yuvpic);
  av_frame_free(&rgbpic);
  // TODO free sws;
}

void Renderer::render(
  // TODO radius r should be a particle property
  double r,
  const std::vector<Particle> &particles
)
{
  uint8_t *s = bitmapRenderer.render(r, particles);
  uint8_t *d = rgbpic->data[0];
  for (int i = 0; i<rgbpic->height; ++i)
  {
    for (int j = 0; j<rgbpic->width; ++j)
    {
      *(d+0) = *(s+2);
      *(d+1) = *(s+1);
      *(d+2) = *(s+0);
      s+=4;
      d+=3;
    }
  }

  sws_scale(sws, rgbpic->data, rgbpic->linesize, 0, rgbpic->height, yuvpic->data, yuvpic->linesize);
  videoFileOutput.encode(yuvpic);
}
