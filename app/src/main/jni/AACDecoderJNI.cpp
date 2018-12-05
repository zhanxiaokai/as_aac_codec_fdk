//
// Created by XiaokaiZhan on 2018/5/31.
//

#include "media_ushow_aac_codec_fdk_AACEncoder.h"
#include "AACDecoder.h"
#include "common.h"

#define LOG_TAG "JNIDecoder"

extern "C"
JNIEXPORT jint JNICALL Java_media_ushow_aac_1codec_1fdk_AACDecoder_decode(JNIEnv * env, jobject obj, jstring aacFilePathParam, jstring pcmFilePathParam){
    const char* aacFilePath = env->GetStringUTFChars(aacFilePathParam, NULL);
    const char* pcmFilePath = env->GetStringUTFChars(pcmFilePathParam, NULL);
    LOGI("aacFilePath is %s", aacFilePath);
    LOGI("pcmFilePath is %s", pcmFilePath);
    long long startTimeMills = currentTimeMills();
    FILE* aacFile = fopen(aacFilePath, "rb+");
    FILE* pcmFile = fopen(pcmFilePath, "wb+");
    if(aacFile) {
        AACDecoder* decoder = new AACDecoder();
//        int audioSpecInfoSize = 2;
//        byte audioSpecInfo[2];
//        memcpy(audioSpecInfo, "\x12\x10", 2);
//        if(decoder->InitWithRAWFormat(audioSpecInfo, audioSpecInfoSize)) {
        if(decoder->InitWithADTSFormat()) {
            while (true) {
                int size = 10 * 1024;
                byte* packet = new byte[size];
                int actualSize = fread(packet, sizeof(byte), size, aacFile);
                if(actualSize > 0) {
                    byte* frameBuffer = NULL;
                    int frameBufferSize = decoder->Decode(packet, size, &frameBuffer);
                    if(frameBufferSize > 0) {
                        fwrite(frameBuffer, sizeof(byte), frameBufferSize, pcmFile);
                    }
                    if(frameBuffer) {
                        delete[] frameBuffer;
                    }
                } else {
                    break;
                }
            }
            decoder->Destroy();
        }
        delete decoder;
        fclose(aacFile);
    }
    if (pcmFile) {
        fclose(pcmFile);
    }
    env->ReleaseStringUTFChars(aacFilePathParam, aacFilePath);
    env->ReleaseStringUTFChars(pcmFilePathParam, pcmFilePath);
    int wasteTimeMills = currentTimeMills() - startTimeMills;
    return wasteTimeMills;
}
