#ifndef __APP_AUDIO_H__
#define __APP_AUDIO_H__

#include "freertos/ringbuf.h"

#include "assert.h"
#include "esp_heap_caps.h"
#include "Com_Debug.h"
#include "Int_ES8311.h"
#include "Int_encoder.h"
#include "Int_decoder.h"
#include "Int_SR.h"

// ��Ƶ������
typedef struct audio_process audio_process_t;

/**
 * @brief ���� ��Ƶ����������
 * 
 * @return audio_process_t* ��Ƶ����������
 */
audio_process_t *App_process_Creat(void);

/**
 * @brief ���� ��Ƶ���������������
 * 
 * @param audio_process ��Ƶ����������
 */
void App_process_Start(audio_process_t *audio_process);

/**
 * @brief ��ȡ���Ա��������������������
 * 
 * @param audio_process ��Ƶ����������
 * @param len ��ȡ���ݵĳ���
 * @return void* ��ȡ��������
 */
void *App_process_ReadFromEncoderData(audio_process_t *audio_process, size_t *len);

/**
 * @brief д�����ݵ������������뻺����
 * 
 * @param audio_process ��Ƶ����������
 * @param data д�������
 * @param len д������ݳ���
 */
void App_process_WriteToDecoderData(audio_process_t *audio_process, void *data, int len);

/**
 * @brief ���� ����ʶ��ϵͳ�Ļ���״̬
 * 
 * @param audio_process ����ʶ�����
 */
void App_process_Reset_Wakenet(audio_process_t *audio_process);

/**
 * @brief ���� SRģ��Ļ��ѻص�����
 * 
 * @param audio_process ��Ƶ����������
 * @param wakenet_cb �����Ļص�����
 * @param args �ص������Ĳ���
 */
void App_process_SR_Set_WakeNet_cb(audio_process_t *audio_process, void (*wakenet_cb)(void *), void *args);

/**
 * @brief ���� SRģ�������ʶ��ص�����
 * 
 * @param audio_process ��Ƶ����������
 * @param vad_cb �����Ļص�����
 * @param args �ص������Ĳ���
 */
void App_process_SR_Set_VAD_cb(audio_process_t *audio_process, void (*vad_cb)(vad_state_t vad_state, void *), void *args);

#endif /* __APP_AUDIO_H__ */
