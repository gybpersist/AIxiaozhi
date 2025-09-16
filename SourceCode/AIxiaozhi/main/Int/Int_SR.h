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

// 语音识别对象
typedef struct audio_sr audio_sr_t;

/**
 * @brief 创建 语音识别系统
 * 
 * @return audio_sr_t* 语音识别对象
 */
audio_sr_t *Int_SR_Creat(void);

/**
 * @brief 开启 语音识别系统
 *
 * @param audio_sr 语音识别对象
 * @note 创建两个任务 (把来自mic的数据写入到AFE声学前端) (处理AFE声学前端的输出结果)
 */
void Int_SR_Start(audio_sr_t *audio_sr);

/**
 * @brief 重置 语音识别系统的唤醒状态
 * 
 * @param audio_sr 语音识别对象
 */
void Int_SR_Reset_Wakenet(audio_sr_t *audio_sr);

/**
 * @brief 为语音识别系统添加输出缓冲区
 * 
 * @param audio_sr 语音识别对象
 * @param ringbuffer 缓冲区对象
 */
void Int_SR_Set_OutputBuffer(audio_sr_t *audio_sr, RingbufHandle_t ringbufferhandle);

/**
 * @brief 创建唤醒后的回调函数
 * 
 * @param audio_sr 语音识别对象
 * @param wakenet_cb 唤醒后的回调函数
 * @param args 函数参数
 */
void Int_SR_Set_WakeNet_cb(audio_sr_t *audio_sr, void (*wakenet_cb)(void *), void *args);

/**
 * @brief 创建人声识别的回调函数
 * 
 * @param audio_sr 语音识别对象
 * @param vad_cb 人声识别的回调函数
 * @param args 函数参数
 */
void Int_SR_Set_VAD_cb(audio_sr_t *audio_sr, void (*vad_cb)(vad_state_t vad_state, void *), void *args);

#endif /* __INT_SR_H__ */
