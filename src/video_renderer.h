#pragma once

#include <stdio.h>
#include <string>

class AVCodec;
class AVCodecContext;
class AVFrame;
class AVPacket;

class VideoRenderer
{
private:

  const std::string _filename;
  const int _width;
  const int _height;

  AVCodec *codec;
  AVCodecContext *c;
  AVFrame *yuvpic;
  FILE *f;
  AVPacket *avpkt;

public:

  VideoRenderer(const std::string& filename, int width, int height, int fps);

  ~VideoRenderer();

  void encode(AVFrame *yuvpic);
};
