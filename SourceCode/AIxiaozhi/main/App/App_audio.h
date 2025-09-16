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

// 音频处理器
typedef struct audio_process audio_process_t;

/**
 * @brief 创建 音频处理器对象
 * 
 * @return audio_process_t* 音频处理器对象
 */
audio_process_t *App_process_Creat(void);

/**
 * @brief 开启 音频处理器对象的任务
 * 
 * @param audio_process 音频处理器对象
 */
void App_process_Start(audio_process_t *audio_process);

/**
 * @brief 读取来自编码器输出缓冲区的数据
 * 
 * @param audio_process 音频处理器对象
 * @param len 读取数据的长度
 * @return void* 读取到的数据
 */
void *App_process_ReadFromEncoderData(audio_process_t *audio_process, size_t *len);

/**
 * @brief 写入数据到解码器的输入缓冲区
 * 
 * @param audio_process 音频处理器对象
 * @param data 写入的数据
 * @param len 写入的数据长度
 */
void App_process_WriteToDecoderData(audio_process_t *audio_process, void *data, int len);

/**
 * @brief 重置 语音识别系统的唤醒状态
 * 
 * @param audio_process 语音识别对象
 */
void App_process_Reset_Wakenet(audio_process_t *audio_process);

/**
 * @brief 创建 SR模块的唤醒回调函数
 * 
 * @param audio_process 音频处理器对象
 * @param wakenet_cb 创建的回调函数
 * @param args 回调函数的参数
 */
void App_process_SR_Set_WakeNet_cb(audio_process_t *audio_process, void (*wakenet_cb)(void *), void *args);

/**
 * @brief 创建 SR模块的人声识别回调函数
 * 
 * @param audio_process 音频处理器对象
 * @param vad_cb 创建的回调函数
 * @param args 回调函数的参数
 */
void App_process_SR_Set_VAD_cb(audio_process_t *audio_process, void (*vad_cb)(vad_state_t vad_state, void *), void *args);

#endif /* __APP_AUDIO_H__ */
