/*********************************
 * Copyright (c) 2018 Bruceshu 3350207067@qq.com
 * Auther:Bruceshu
 * Date:  2018-10-08
 * Description:
 
*********************************/


#ifndef AVUTIL_VERSION_H
#define AVUTIL_VERSION_H

#define AV_VERSION_INT(a, b, c) ((a)<<16 | (b)<<8 | (c))
#define AV_VERSION_DOT(a, b, c) a ##.## b ##.## c
#define AV_VERSION(a, b, c) AV_VERSION_DOT(a, b, c)


#define LIBAVUTIL_VERSION_MAJOR  56
#define LIBAVUTIL_VERSION_MINOR  19
#define LIBAVUTIL_VERSION_MICRO 101

#define LIBAVUTIL_VERSION_INT   AV_VERSION_INT(LIBAVUTIL_VERSION_MAJOR, \
                                               LIBAVUTIL_VERSION_MINOR, \
                                               LIBAVUTIL_VERSION_MICRO)
#define LIBAVUTIL_VERSION       AV_VERSION(LIBAVUTIL_VERSION_MAJOR,     \
                                           LIBAVUTIL_VERSION_MINOR,     \
                                           LIBAVUTIL_VERSION_MICRO)
#define LIBAVUTIL_BUILD         LIBAVUTIL_VERSION_INT
#define LIBAVUTIL_IDENT         "Lavu" AV_STRINGIFY(LIBAVUTIL_VERSION)

#ifndef FF_API_VAAPI
#define FF_API_VAAPI                    (LIBAVUTIL_VERSION_MAJOR < 57)
#endif
#ifndef FF_API_FRAME_QP
#define FF_API_FRAME_QP                 (LIBAVUTIL_VERSION_MAJOR < 57)
#endif
#ifndef FF_API_PLUS1_MINUS1
#define FF_API_PLUS1_MINUS1             (LIBAVUTIL_VERSION_MAJOR < 57)
#endif
#ifndef FF_API_ERROR_FRAME
#define FF_API_ERROR_FRAME              (LIBAVUTIL_VERSION_MAJOR < 57)
#endif
#ifndef FF_API_PKT_PTS
#define FF_API_PKT_PTS                  (LIBAVUTIL_VERSION_MAJOR < 57)
#endif
#ifndef FF_API_CRYPTO_SIZE_T
#define FF_API_CRYPTO_SIZE_T            (LIBAVUTIL_VERSION_MAJOR < 57)
#endif
#ifndef FF_API_FRAME_GET_SET
#define FF_API_FRAME_GET_SET            (LIBAVUTIL_VERSION_MAJOR < 57)
#endif
#ifndef FF_API_PSEUDOPAL
#define FF_API_PSEUDOPAL                (LIBAVUTIL_VERSION_MAJOR < 57)
#endif

#endif
