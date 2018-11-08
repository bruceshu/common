/*********************************
 * Copyright (c) 2018 Bruceshu 3350207067@qq.com
 * Auther:Bruceshu
 * Date:  2018-11-06
 * Description:
 
*********************************/


#ifndef LIBAVCODEC_UTILS_H
#define LIBAVCODEC_UTILS_H

#include "avcodec.h"

int avcodec_parameters_to_context(AVCodecContext *codec, const AVCodecParameters *par);
void avcodec_parameters_free(AVCodecParameters **ppar);
int av_codec_is_decoder(const AVCodec *codec);
int avcodec_is_open(AVCodecContext *s);


#endif
