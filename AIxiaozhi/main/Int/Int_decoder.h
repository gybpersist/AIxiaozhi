#ifndef __INT_DECODER_H__
#define __INT_DECODER_H__

#include "freertos/ringbuf.h"
#include "esp_audio_dec_default.h"
#include "esp_audio_dec_reg.h"
#include "esp_audio_dec.h"

#include "assert.h"
#include "esp_heap_caps.h"
#include "Com_Debug.h"
#include "Int_ES8311.h"

// opus解码器 将 OPUS格式 --> PCM格式
typedef struct audio_decoder audio_decoder_t;

/**
 * @brief 创建 opus解码器对象
 *
 * @return audio_decoder_t* opus解码器对象
 */
audio_decoder_t *Int_decoder_Creat(void);

/**
 * @brief opus解码器开始工作
 *
 * @param audio_decoder opus解码器对象
 * @note 启动任务函数
 */
void Int_decoder_Start(audio_decoder_t *audio_decoder);

/**
 * @brief 为 opus解码器添加输出缓冲区
 *
 * @param audio_decoder opus解码器对象
 * @param outputbufferhandle 输出缓冲区句柄
 */
void Int_decoder_Set_OutputBuffer(audio_decoder_t *audio_decoder, RingbufHandle_t outputbufferhandle);

/**
 * @brief 为 opus解码器添加输入缓冲区
 *
 * @param audio_decoder opus解码器对象
 * @param inputbufferhandle 输入缓冲区句柄
 */
void Int_decoder_Set_InputBuffer(audio_decoder_t *audio_decoder, RingbufHandle_t inputbufferhandle);


#endif /* __INT_DECODER_H__ */
