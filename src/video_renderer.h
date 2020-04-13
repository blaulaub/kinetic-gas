#pragma once

#include <string>

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

  VideoRenderer(const std::string& filename, int width, int height, int fps)
  : _filename(filename), _width(width), _height(height)
  {

    codec = avcodec_find_encoder(AV_CODEC_ID_MPEG2VIDEO);
    if (!codec) {
      fprintf(stderr, "codec not found\n");
      exit(1);
    }

    c = avcodec_alloc_context3(codec);

    c->bit_rate = 400000;
    c->width = width;
    c->height = height;
    c->time_base = (AVRational){1,fps};
    c->framerate = (AVRational){fps,1};
    c->gop_size = 10; /* emit one intra frame every ten frames */
    c->max_b_frames=1;
    c->pix_fmt = AV_PIX_FMT_YUV420P;

    if (avcodec_open2(c, codec, NULL) < 0) {
      fprintf(stderr, "could not open codec\n");
      exit(1);
    }

    f = fopen("crash.mpg", "wb");
    if (!f) {
      fprintf(stderr, "could not open %s\n", "crash.mpg");
      exit(1);
    }

    avpkt = av_packet_alloc();
    if (!avpkt) {
      fprintf(stderr, "could not alloc pkt\n");
      exit(1);
    }

  }

  ~VideoRenderer()
  {
    encode(NULL);
    fclose(f);

    avcodec_free_context(&c);
    av_packet_free(&avpkt);
  }

  void encode(AVFrame *yuvpic)
  {
    int ret = avcodec_send_frame(c, yuvpic);
    while (ret >= 0)
    {
      ret = avcodec_receive_packet(c, avpkt);
      if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;
      if (ret < 0) {
        fprintf(stderr, "error while encoding\n");
        exit(1);
      }
      fwrite(avpkt->data, 1, avpkt->size, f);
      av_packet_unref(avpkt);
    }
  }
};
