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


// opus������ �� PCM��ʽ --> OPUS��ʽ
typedef struct audio_encoder audio_encoder_t;

/**
 * @brief ���� opus����������
 *
 * @return audio_encoder_t* opus����������
 */
audio_encoder_t *Int_encoder_Creat(void);

/**
 * @brief opus��������ʼ����
 *
 * @param audio_encoder opus����������
 * @note ����������
 */
void Int_encoder_Start(audio_encoder_t *audio_encoder);

/**
 * @brief Ϊ opus������������������
 *
 * @param audio_encoder opus����������
 * @param outputbufferhandle ������������
 */
void Int_encoder_Set_OutputBuffer(audio_encoder_t *audio_encoder, RingbufHandle_t outputbufferhandle);

/**
 * @brief Ϊ opus������������뻺����
 *
 * @param audio_encoder opus����������
 * @param inputbufferhandle ���뻺�������
 */
void Int_encoder_Set_InputBuffer(audio_encoder_t *audio_encoder, RingbufHandle_t inputbufferhandle);

#endif /* __INT_ENCODER_H__ */
