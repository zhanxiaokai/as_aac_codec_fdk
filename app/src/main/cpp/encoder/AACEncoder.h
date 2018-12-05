//
// Created by XiaokaiZhan on 2018/5/31.
//

#ifndef AAC_CODEC_FDK_AACENCODER_H
#define AAC_CODEC_FDK_AACENCODER_H

#include "fdk-aac/aacenc_lib.h"
#include "common.h"
#include <string.h>


typedef enum {
    LC_AAC = 2,
    HE_AAC = 5,
    HE_V2_AAC = 29
} AACProfile;

/**
 * https://github.com/jgfntu/libav/blob/master/libavcodec/libfdk-aacenc.c
 */
class AACEncoder {
private:
    HANDLE_AACENCODER   m_aacEncHandle;
    AACENC_InfoStruct m_EncInfo = { 0 };
    uint8_t* m_inBuffer;
    int m_inBufferCursor;
    int m_inputSizeFixed;
    uint8_t m_aacOutbuf[20480];
    FILE* test_aacFile;
    bool isFlagGlobalHeader;

public:
    AACEncoder();
    ~AACEncoder();
    bool Init(AACProfile profile, int sampleRate, int channels, int bitRate);
    int Encode(byte* pData, int dataByteSize, byte** packetBuffer);
    void Destroy();

private:
    int fdkEncodeAudio();

    void addADTStoPacket(uint8_t* packet, int packetLen);
    void writeAACPakcetToFile(uint8_t* data, int datalen);
};


#endif //AAC_CODEC_FDK_AACENCODER_H
