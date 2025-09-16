#ifndef __INT_SR_H__
#define __INT_SR_H__

#include "freertos/ringbuf.h"
#include "esp_afe_sr_iface.h"
#include "esp_afe_sr_models.h"

#include "assert.h"
#include "esp_heap_caps.h"
#include "Com_Debug.h"
#include "Int_ES8311.h"
#include "Com_status.h"

// ����ʶ�����
typedef struct audio_sr audio_sr_t;

/**
 * @brief ���� ����ʶ��ϵͳ
 * 
 * @return audio_sr_t* ����ʶ�����
 */
audio_sr_t *Int_SR_Creat(void);

/**
 * @brief ���� ����ʶ��ϵͳ
 *
 * @param audio_sr ����ʶ�����
 * @note ������������ (������mic������д�뵽AFE��ѧǰ��) (����AFE��ѧǰ�˵�������)
 */
void Int_SR_Start(audio_sr_t *audio_sr);

/**
 * @brief ���� ����ʶ��ϵͳ�Ļ���״̬
 * 
 * @param audio_sr ����ʶ�����
 */
void Int_SR_Reset_Wakenet(audio_sr_t *audio_sr);

/**
 * @brief Ϊ����ʶ��ϵͳ������������
 * 
 * @param audio_sr ����ʶ�����
 * @param ringbuffer ����������
 */
void Int_SR_Set_OutputBuffer(audio_sr_t *audio_sr, RingbufHandle_t ringbufferhandle);

/**
 * @brief �������Ѻ�Ļص�����
 * 
 * @param audio_sr ����ʶ�����
 * @param wakenet_cb ���Ѻ�Ļص�����
 * @param args ��������
 */
void Int_SR_Set_WakeNet_cb(audio_sr_t *audio_sr, void (*wakenet_cb)(void *), void *args);

/**
 * @brief ��������ʶ��Ļص�����
 * 
 * @param audio_sr ����ʶ�����
 * @param vad_cb ����ʶ��Ļص�����
 * @param args ��������
 */
void Int_SR_Set_VAD_cb(audio_sr_t *audio_sr, void (*vad_cb)(vad_state_t vad_state, void *), void *args);

#endif /* __INT_SR_H__ */
