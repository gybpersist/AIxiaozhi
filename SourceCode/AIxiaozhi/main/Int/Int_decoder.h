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

// opus������ �� OPUS��ʽ --> PCM��ʽ
typedef struct audio_decoder audio_decoder_t;

/**
 * @brief ���� opus����������
 *
 * @return audio_decoder_t* opus����������
 */
audio_decoder_t *Int_decoder_Creat(void);

/**
 * @brief opus��������ʼ����
 *
 * @param audio_decoder opus����������
 * @note ����������
 */
void Int_decoder_Start(audio_decoder_t *audio_decoder);

/**
 * @brief Ϊ opus������������������
 *
 * @param audio_decoder opus����������
 * @param outputbufferhandle ������������
 */
void Int_decoder_Set_OutputBuffer(audio_decoder_t *audio_decoder, RingbufHandle_t outputbufferhandle);

/**
 * @brief Ϊ opus������������뻺����
 *
 * @param audio_decoder opus����������
 * @param inputbufferhandle ���뻺�������
 */
void Int_decoder_Set_InputBuffer(audio_decoder_t *audio_decoder, RingbufHandle_t inputbufferhandle);


#endif /* __INT_DECODER_H__ */
