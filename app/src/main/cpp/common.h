//
// Created by XiaokaiZhan on 2018/5/31.
//

#ifndef AAC_CODEC_FDK_COMMON_H
#define AAC_CODEC_FDK_COMMON_H

#include <android/log.h>
#include "stdio.h"
#include <sys/time.h>

#ifdef __ANDROID__
#include <android/log.h>
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#elif defined(__APPLE__)	// IOS or OSX
#define LOGI(...)  printf("  ");printf(__VA_ARGS__); printf("\t -  <%s> \n", LOG_TAG);
#define LOGE(...)  printf(" Error: ");printf(__VA_ARGS__); printf("\t -  <%s> \n", LOG_TAG);
#endif
typedef unsigned char byte;

static inline long long currentTimeMills(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}


#endif //AAC_CODEC_FDK_COMMON_H
