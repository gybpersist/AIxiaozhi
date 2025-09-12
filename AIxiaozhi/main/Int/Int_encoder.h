#ifndef __INT_ENCODER_H__
#define __INT_ENCODER_H__

#include "freertos/ringbuf.h"
#include "esp_audio_enc_default.h"
#include "esp_audio_enc_reg.h"
#include "esp_audio_enc.h"

#include "assert.h"
#include "esp_heap_caps.h"
#include "Com_Debug.h"
#include "Int_ES8311.h"


// opus编码器 将 PCM格式 --> OPUS格式
typedef struct audio_encoder audio_encoder_t;

/**
 * @brief 创建 opus编码器对象
 *
 * @return audio_encoder_t* opus编码器对象
 */
audio_encoder_t *Int_encoder_Creat(void);

/**
 * @brief opus编码器开始工作
 *
 * @param audio_encoder opus编码器对象
 * @note 启动任务函数
 */
void Int_encoder_Start(audio_encoder_t *audio_encoder);

/**
 * @brief 为 opus编码器添加输出缓冲区
 *
 * @param audio_encoder opus编码器对象
 * @param outputbufferhandle 输出缓冲区句柄
 */
void Int_encoder_Set_OutputBuffer(audio_encoder_t *audio_encoder, RingbufHandle_t outputbufferhandle);

/**
 * @brief 为 opus编码器添加输入缓冲区
 *
 * @param audio_encoder opus编码器对象
 * @param inputbufferhandle 输入缓冲区句柄
 */
void Int_encoder_Set_InputBuffer(audio_encoder_t *audio_encoder, RingbufHandle_t inputbufferhandle);

#endif /* __INT_ENCODER_H__ */
