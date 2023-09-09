//
// Created by juvenile on 2023-09-09.
//

#pragma once

#ifndef LEARN_FFMPEG_COMMON_H
#define LEARN_FFMPEG_COMMON_H

#define check_ret(ret)                                                          \
    if (ret != 0) {                                                             \
        av_strerror(ret, errbuf, sizeof(errbuf) - 1);                           \
        printf("at %s(%s:%d)\t%s\n", __FUNCTION__, __FILE__, __LINE__, errbuf); \
        return ret;                                                             \
    }

#define check_label(ret, lab)                                                   \
    if (ret != 0) {                                                             \
        av_strerror(ret, errbuf, sizeof(errbuf) - 1);                           \
        printf("at %s(%s:%d)\t%s\n", __FUNCTION__, __FILE__, __LINE__, errbuf); \
        goto lab;                                                               \
    }

#endif//LEARN_FFMPEG_COMMON_H
