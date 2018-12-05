//
// Created by XiaokaiZhan on 2018/5/31.
//
#include <fdk-aac/aacdecoder_lib.h>
#include "AACDecoder.h"
#include "common.h"

#define LOG_TAG "AACDecoder"

#define FDK_MAX_AUDIO_FRAME_SIZE    192000      //1 second of 48khz 32bit audio
AACDecoder::AACDecoder(){
    m_pcm_pkt_size = -1;
    m_SpecInfo = NULL;
    m_SpecInfoSize = 64;
    m_decoderHandle = NULL;
}

AACDecoder::~AACDecoder() {

}

bool AACDecoder::InitWithRAWFormat(byte* m_SpecInfo, UINT m_SpecInfoSize)
{
    LOGI("Enter AACDecoder InitWithRAWFormat");
    int nrOfLayers = 1;
    m_decoderHandle = aacDecoder_Open(TT_MP4_RAW, nrOfLayers);
    int ret = aacDecoder_ConfigRaw(m_decoderHandle, &m_SpecInfo, &m_SpecInfoSize);
    if (ret != AAC_DEC_OK) {
        LOGI("Unable to set configRaw\n");
        return false;
    }
    initFrameSize();
    this->PrintAACInfo();
    return true;
}

bool AACDecoder::InitWithADTSFormat() {
    LOGI("Enter AACDecoder InitWithADTSFormat");
    int nrOfLayers = 1;
    m_decoderHandle = aacDecoder_Open(TT_MP4_ADTS, nrOfLayers);
    this->PrintAACInfo();
    return true;
}

int AACDecoder::Decode(byte *packetBuffer, int pkt_size, byte** frameBuffer) {
    LOGI("Enter AACDecoder Decode");
    int threshold = (FDK_MAX_AUDIO_FRAME_SIZE * 3) / 2;
    uint8_t pcm_buf[threshold];
    int pcm_buf_index = 0;
    int pcm_buf_size = 0;
    while (pkt_size > 0) {
        int data_size = threshold;
        int len1 = this->fdkDecodeAudio((INT_PCM *) (pcm_buf + pcm_buf_index), &data_size,
                                          packetBuffer, pkt_size);
        if (len1 < 0) {
            /* if error, skip frame */
            pkt_size = 0;
            break;
        }
        packetBuffer += len1;
        pkt_size -= len1;
        if (data_size <= 0) {
            /* No data yet, get more frames */
            break;
        }
        pcm_buf_index += data_size;
        pcm_buf_size += data_size;
    }
    if (pcm_buf_size > 0) {
        *frameBuffer = new byte[pcm_buf_size];
        memcpy(*frameBuffer, pcm_buf, pcm_buf_size);
    }
    return pcm_buf_size;
}

/*
 * decoding AAC format audio data by libfdk_aac
 */
int AACDecoder::fdkDecodeAudio(INT_PCM *output_buf, int *output_size, byte *pktBuffer, int pktSize)
{
    this->PrintAACInfo();
    int ret = 0;
    UINT pkt_size = pktSize;
    UINT valid_size = pktSize;
    UCHAR *input_buf[1] = {pktBuffer};

    /* step 1 -> fill aac_data_buf to decoder's internal buf */
    ret = aacDecoder_Fill(m_decoderHandle, input_buf, &pkt_size, &valid_size);
    if (ret != AAC_DEC_OK) {
        fprintf(stderr, "Fill failed: %x\n", ret);
        *output_size  = 0;
        //AVERROR_INVALIDDATA
        return 0;
    }
    int buf_size = m_pcm_pkt_size;
    if(m_pcm_pkt_size <= 0) {
        buf_size = 50*1024;
    }
    /* step 2 -> call decoder function */
    int fdk_flags = 0;
    ret = aacDecoder_DecodeFrame(m_decoderHandle, output_buf, buf_size, fdk_flags);
    if (ret == AAC_DEC_NOT_ENOUGH_BITS) {
        fprintf(stderr, "not enough\n");
        *output_size  = 0;
        return (pktSize - valid_size);
    }
    if (ret != AAC_DEC_OK) {
        fprintf(stderr, "aacDecoder_DecodeFrame : 0x%x\n", ret);
        *output_size = 0;
        //AVERROR_INVALIDDATA
        return 0;
    }
    if(m_pcm_pkt_size <= 0) {
        initFrameSize();
    }
    *output_size = m_pcm_pkt_size;
    /* return aac decode size */
    return (pktSize - valid_size);
}

void AACDecoder::initFrameSize() {
    CStreamInfo *aac_stream_info = aacDecoder_GetStreamInfo(m_decoderHandle);
    if (aac_stream_info) {
        aac_stream_info->aacSamplesPerFrame = aac_stream_info->aacSamplesPerFrame * 2;
        aac_stream_info->aacSampleRate = aac_stream_info->aacSampleRate * 2;
//        m_pcm_pkt_size = aac_stream_info->numChannels * 2 * aac_stream_info->frameSize;
        m_pcm_pkt_size = aac_stream_info->channelConfig * 2 * aac_stream_info->aacSamplesPerFrame;
    }
}

void AACDecoder::PrintAACInfo() {
    CStreamInfo* aac_stream_info = aacDecoder_GetStreamInfo(m_decoderHandle);
    if (aac_stream_info == NULL) {
        LOGI("aacDecoder_GetStreamInfo failed!\n");
        return;
    }
    LOGI("> stream info: channel = %d\tsample_rate = %d\tframe_size = %d\taot = %d\tbitrate = %d\n",
         aac_stream_info->channelConfig, aac_stream_info->aacSampleRate,
         aac_stream_info->aacSamplesPerFrame, aac_stream_info->aot, aac_stream_info->bitRate);
}

void AACDecoder::Destroy() {
    LOGI("Enter AACDecoder Destroy");
    if(m_decoderHandle) {
        aacDecoder_Close(m_decoderHandle);
    }
}