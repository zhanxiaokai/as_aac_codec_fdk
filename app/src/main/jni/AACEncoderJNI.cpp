//
// Created by XiaokaiZhan on 2018/5/31.
//

#include "media_ushow_aac_codec_fdk_AACEncoder.h"
#include "AACEncoder.h"
#include "AACDecoder.h"
#include "common.h"

#define LOG_TAG "JNIEncoder"
extern "C"
JNIEXPORT jint JNICALL Java_media_ushow_aac_1codec_1fdk_AACEncoder_encode(JNIEnv * env, jobject obj, jstring pcmFilePathParam, jstring aacFilePathParam){
    const char* pcmFilePath = env->GetStringUTFChars(pcmFilePathParam, NULL);
    const char* aacFilePath = env->GetStringUTFChars(aacFilePathParam, NULL);
    LOGI("pcmFilePath is %s", pcmFilePath);
    LOGI("aacFilePath is %s", aacFilePath);
    long long startTimeMills = currentTimeMills();
    FILE* pcmFile = fopen(pcmFilePath, "rb+");
    FILE* aacFile = fopen(aacFilePath, "wb+");
    int packetCnt = 0;
    if(pcmFile) {
        AACEncoder* encoder = new AACEncoder();
        AACProfile profile = LC_AAC;
        int sampleRate = 48000;
        int channels = 1;
        int bitRate = 32 * 1024;
        if(encoder->Init(profile, sampleRate, channels, bitRate)) {
            while (true) {
                int size = 1024;
                uint16_t * buffer = new uint16_t[size];
                int actualSize = fread(buffer, sizeof(uint16_t), size, pcmFile);
                if(actualSize > 0) {
                    byte* packetBuffer = NULL;
                    int packetBufferSize = encoder->Encode((byte*)buffer, size * sizeof(uint16_t), &packetBuffer);
                    if(packetBufferSize > 0) {
                        packetCnt++;
                        fwrite(packetBuffer, sizeof(byte), packetBufferSize, aacFile);
                    }
                    if(packetBuffer) {
                        delete[] packetBuffer;
                    }
                } else {
                    break;
                }
            }
            encoder->Destroy();
        }
        delete encoder;
        fclose(pcmFile);
    }
    if(aacFile) {
        fclose(aacFile);
    }
    env->ReleaseStringUTFChars(pcmFilePathParam, pcmFilePath);
    env->ReleaseStringUTFChars(aacFilePathParam, aacFilePath);
    int wasteTimeMills = currentTimeMills() - startTimeMills;
    LOGI("packetCnt is %d", packetCnt);
    LOGI("wasteTimeMills is %d", wasteTimeMills);
    return wasteTimeMills;
}

JNIEXPORT jint JNICALL Java_media_ushow_aac_1codec_1fdk_AACEncoder_encodeAndDecodeTest(JNIEnv * env, jobject obj, jstring pcmFilePathParam, jstring aacFilePathParam){
    const char* pcmFilePath = env->GetStringUTFChars(pcmFilePathParam, NULL);
    const char* aacFilePath = env->GetStringUTFChars(aacFilePathParam, NULL);
    LOGI("pcmFilePath is %s", pcmFilePath);
    LOGI("aacFilePath is %s", aacFilePath);
    long long startTimeMills = currentTimeMills();
    FILE* pcmFile = fopen(pcmFilePath, "rb+");
    FILE* aacFile = fopen(aacFilePath, "wb+");
    if(pcmFile) {
        AACEncoder* encoder = new AACEncoder();
        AACDecoder* decoder = new AACDecoder();
        AACProfile profile = LC_AAC;//HE_AAC;
        int sampleRate = 44100;
        int channels = 2;
        int bitRate = 64 * 1024;
        if(encoder->Init(profile, sampleRate, channels, bitRate)) {
            int audioSpecInfoSize = 2;
            byte audioSpecInfo[2];
            memcpy(audioSpecInfo, "\x13\x90", 2);
//            if(decoder->InitWithRAWFormat(audioSpecInfo, audioSpecInfoSize)) {
            if(decoder->InitWithADTSFormat()) {
                while (true) {
                    int size = 1024 * 2;
                    uint16_t * buffer = new uint16_t[size];
                    int actualSize = fread(buffer, sizeof(uint16_t), size, pcmFile);
                    if(actualSize > 0) {
                        byte* packetBuffer = NULL;
                        int packetBufferSize = encoder->Encode((byte*)buffer, size * sizeof(uint16_t), &packetBuffer);
                        if(packetBufferSize > 0) {
                            byte* frameBuffer = NULL;
                            int frameBufferSize = decoder->Decode(packetBuffer, packetBufferSize, &frameBuffer);
                            if(frameBufferSize > 0) {
                                fwrite(frameBuffer, sizeof(byte), frameBufferSize, aacFile);
                            }
                            if(frameBuffer) {
                                delete[] frameBuffer;
                            }
                        }
                        if(packetBuffer) {
                            delete[] packetBuffer;
                        }
                    } else {
                        break;
                    }
                }
                decoder->Destroy();
            }
            encoder->Destroy();
        }
        delete encoder;
        delete decoder;
        fclose(pcmFile);
    }
    if(aacFile) {
        fclose(aacFile);
    }
    env->ReleaseStringUTFChars(pcmFilePathParam, pcmFilePath);
    env->ReleaseStringUTFChars(aacFilePathParam, aacFilePath);
    int wasteTimeMills = currentTimeMills() - startTimeMills;
    return wasteTimeMills;
}