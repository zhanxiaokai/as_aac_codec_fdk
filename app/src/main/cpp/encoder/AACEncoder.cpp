//
// Created by XiaokaiZhan on 2018/5/31.
//

#include <fdk-aac/aacenc_lib.h>
#include "AACEncoder.h"
#include "put_bits.h"

#define LOG_TAG "AACEncoder"
AACEncoder::AACEncoder(){
    m_aacEncHandle = NULL;
    m_inBufferCursor = 0;
    m_inBuffer = NULL;
    m_inputSizeFixed = 0;
    isFlagGlobalHeader = false;
}

AACEncoder::~AACEncoder() {
}

bool AACEncoder::Init(AACProfile profile, int sampleRate, int channels, int bitRate) {
    LOGI("Enter AACEncoder Init");
    if (aacEncOpen(&m_aacEncHandle, 0, channels) != AACENC_OK) {
        LOGI("Unable to open fdkaac encoder\n");
        return false;
    }
    /**编码规格
     *  - 2: LC. MPEG-4 AAC Low Complexity
     *  - 5: HE-AAC. MPEG-4 AAC Low Complexity with Spectral Band Replication
     *  - 29: HE-AAC v2. AAC Low Complexity with Spectral Band Replication and Parametric Stereo(声道数必须是2)
     */
    if (aacEncoder_SetParam(m_aacEncHandle, AACENC_AOT, profile) != AACENC_OK) {
        LOGI("Unable to set the AOT\n");
        return false;
    }
    /**
     * 设置采样率
     */
    if (aacEncoder_SetParam(m_aacEncHandle, AACENC_SAMPLERATE, sampleRate) != AACENC_OK) {
        LOGI("Unable to set the AOT\n");
        return false;
    }
    /**
     * 设置声道数
     */
    CHANNEL_MODE modeChannel = channels == 2 ? MODE_2 : MODE_1;
    if (aacEncoder_SetParam(m_aacEncHandle, AACENC_CHANNELMODE, modeChannel) != AACENC_OK) {
        LOGI("Unable to set the channel mode\n");
        return false;
    }
    /**
     * 设置比特率
     */
    if (aacEncoder_SetParam(m_aacEncHandle, AACENC_BITRATE, bitRate) != AACENC_OK) {
        LOGI("Unable to set the bitrate\n");
        return false;
    }

    /**
     *  设置输出格式，是AAC的裸流还是ADTS的流
     *  0-raw 2-adts
     */
    int encode_mode = 2;
    if(isFlagGlobalHeader) {
        encode_mode = 0;
    }
    if (aacEncoder_SetParam(m_aacEncHandle, AACENC_TRANSMUX, encode_mode) != AACENC_OK) {
        LOGI("Unable to set the ADTS transmux\n");
        return false;
    }

    /**
     * 初始化编码器
     */
    if (aacEncEncode(m_aacEncHandle, NULL, NULL, NULL, NULL) != AACENC_OK) {
        LOGI("Unable to initialize the encoder\n");
        return false;
    }

    if (aacEncInfo(m_aacEncHandle, &m_EncInfo) != AACENC_OK) {
        LOGI("Unable to get the encoder info\n");
        return false;
    }
    m_inputSizeFixed = channels * 2 * m_EncInfo.frameLength; //frameLength是每帧每个channel的采样点数
    m_inBufferCursor = 0;
    m_inBuffer = new uint8_t[m_inputSizeFixed];
    memset(m_inBuffer, 0, m_inputSizeFixed);
    //如果是编码裸流，可以取出编码器的SpecInfo来
    byte * audioSpecInfo = m_EncInfo.confBuf;
    FILE* audioSpecInfoFile = fopen("/mnt/sdcard/a_songstudio/audio_spec_info", "wb+");
    fwrite(audioSpecInfo, 1, m_EncInfo.confSize, audioSpecInfoFile);
    fclose(audioSpecInfoFile);
    test_aacFile = fopen("/mnt/sdcard/a_songstudio/test.aac", "wb+");
    return true;
}

int AACEncoder::Encode(byte* pData, int dataByteSize, byte** packetBuffer) {
    LOGI("Enter AACEncoder Encode");
    int pDataCursor = 0;
    *packetBuffer = new byte[dataByteSize];
    int packetSize = 0;
    while (dataByteSize > 0) {
        int cpySize = 0;
        if(m_inBufferCursor + dataByteSize >= m_inputSizeFixed) {
            cpySize = m_inputSizeFixed - m_inBufferCursor;
            memcpy(m_inBuffer + m_inBufferCursor, pData + pDataCursor, cpySize);
            int aacPktSize = this->fdkEncodeAudio();
            if(aacPktSize > 0) {
                this->writeAACPakcetToFile(m_aacOutbuf, aacPktSize);
                memcpy(*packetBuffer + packetSize, m_aacOutbuf, aacPktSize);
                packetSize += aacPktSize;
            }
            m_inBufferCursor = 0;
            memset(m_inBuffer, 0, m_inputSizeFixed);
        } else {
            cpySize = dataByteSize;
            memcpy(m_inBuffer + m_inBufferCursor, pData + pDataCursor, cpySize);
            m_inBufferCursor += cpySize;
        }
        dataByteSize -= cpySize;
        pDataCursor += cpySize;
    }
    return packetSize;
}
void AACEncoder::addADTStoPacket(uint8_t* packet, int packetLen) {
    int profile = 1;//5;//0 : LC; 5 : HE-AAC; 29: HEV2
    int freqIdx = 3; // 48KHz
    int chanCfg = 2; // Mono

    PutBitContext pb;
    init_put_bits(&pb, packet, ADTS_HEADER_SIZE);
    /* adts_fixed_header */
    put_bits(&pb, 12, 0xfff);   /* syncword */
    put_bits(&pb, 1, 0);        /* ID */
    put_bits(&pb, 2, 0);        /* layer */
    put_bits(&pb, 1, 1);        /* protection_absent */
    put_bits(&pb, 2, profile); /* profile_objecttype */
    put_bits(&pb, 4, freqIdx);
    put_bits(&pb, 1, 0);        /* private_bit */
    put_bits(&pb, 3, chanCfg); /* channel_configuration */
    put_bits(&pb, 1, 0);        /* original_copy */
    put_bits(&pb, 1, 0);        /* home */

    /* adts_variable_header */
    put_bits(&pb, 1, 0);        /* copyright_identification_bit */
    put_bits(&pb, 1, 0);        /* copyright_identification_start */
    put_bits(&pb, 13, packetLen); /* aac_frame_length */
    put_bits(&pb, 11, 0x7ff);   /* adts_buffer_fullness */
    put_bits(&pb, 2, 0);        /* number_of_raw_data_blocks_in_frame */

    flush_put_bits(&pb);
}

void AACEncoder::writeAACPakcetToFile(uint8_t* data, int datalen) {
    LOGI("After One Encode Size is : %d", datalen);
    uint8_t* buffer = data;
    if(isFlagGlobalHeader) {
        datalen += 7;
        uint8_t* buffer = new uint8_t[datalen];
        memset(buffer, 0, datalen);
        memcpy(buffer + 7, data, datalen - 7);
        addADTStoPacket(buffer, datalen);
    }
    fwrite(buffer, sizeof(uint8_t), datalen, test_aacFile);
    if(isFlagGlobalHeader) {
        delete[] buffer;
    }
}

int AACEncoder::fdkEncodeAudio() {
    AACENC_BufDesc in_buf = {0 }, out_buf = {0 };
    AACENC_InArgs in_args = { 0 };
    AACENC_OutArgs out_args = { 0 };
    int in_identifier = IN_AUDIO_DATA;
    int in_elem_size = 2;

    in_args.numInSamples = m_inputSizeFixed / 2;  //size为pcm字节数
    in_buf.numBufs = 1;
    in_buf.bufs = (void**)(&m_inBuffer);  //pData为pcm数据指针
    in_buf.bufferIdentifiers = &in_identifier;
    in_buf.bufSizes = &m_inputSizeFixed;
    in_buf.bufElSizes = &in_elem_size;

    int out_identifier = OUT_BITSTREAM_DATA;
    void *out_ptr = m_aacOutbuf;
    int out_size = sizeof(m_aacOutbuf);
    int out_elem_size = 1;
    out_buf.numBufs = 1;
    out_buf.bufs = &out_ptr;
    out_buf.bufferIdentifiers = &out_identifier;
    out_buf.bufSizes = &out_size;
    out_buf.bufElSizes = &out_elem_size;

    if ((aacEncEncode(m_aacEncHandle, &in_buf, &out_buf, &in_args, &out_args)) != AACENC_OK) {
        LOGI("Encoding aac failed\n");
        return 0;
    }
    if (out_args.numOutBytes == 0)
        return 0;
    //编码后的aac数据存在outbuf中，大小为out_args.numOutBytes
    return out_args.numOutBytes;
}

void AACEncoder::Destroy() {
    LOGI("Enter AACEncoder Destroy");
    if(m_aacEncHandle) {
        aacEncClose(&m_aacEncHandle);
    }
    fclose(test_aacFile);
}