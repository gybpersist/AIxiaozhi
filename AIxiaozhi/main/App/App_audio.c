#include "App_audio.h"

struct audio_process
{
    audio_sr_t *audio_sr;           // 语言识别对象
    audio_encoder_t *audio_encoder; // 编码器对象
    audio_decoder_t *audio_decoder; // 解码器对象

    RingbufHandle_t encoder_inputbuffer;  // 编码器输入缓冲区
    RingbufHandle_t encoder_outputbuffer; // 编码器输出缓冲区
    RingbufHandle_t decoder_inputbuffer;  // 解码器输入缓冲区
    RingbufHandle_t decoder_outputbuffer; // 解码器输出缓冲区
};

// 任务函数声明: 音频处理任务
static void audio_process_task(void *args);

/**
 * @brief 创建 音频处理器对象
 *
 * @return audio_process_t* 音频处理器对象
 */
audio_process_t *App_process_Creat(void)
{
    // 创建 音频处理器对象
    audio_process_t *audio_process = (audio_process_t *)heap_caps_malloc(sizeof(audio_process_t), MALLOC_CAP_SPIRAM);
    audio_process->audio_sr = Int_SR_Creat();
    audio_process->audio_encoder = Int_encoder_Creat();
    audio_process->audio_decoder = Int_decoder_Creat();
    // 创建 缓冲区
    audio_process->encoder_inputbuffer = xRingbufferCreateWithCaps(320 * 1024, RINGBUF_TYPE_BYTEBUF, MALLOC_CAP_SPIRAM);
    audio_process->encoder_outputbuffer = xRingbufferCreateWithCaps(320 * 1024, RINGBUF_TYPE_NOSPLIT, MALLOC_CAP_SPIRAM);
    audio_process->decoder_inputbuffer = xRingbufferCreateWithCaps(320 * 1024, RINGBUF_TYPE_NOSPLIT, MALLOC_CAP_SPIRAM);
    audio_process->decoder_outputbuffer = xRingbufferCreateWithCaps(320 * 1024, RINGBUF_TYPE_NOSPLIT, MALLOC_CAP_SPIRAM);
    Int_SR_Set_OutputBuffer(audio_process->audio_sr,audio_process->encoder_inputbuffer);
    Int_encoder_Set_InputBuffer(audio_process->audio_encoder, audio_process->encoder_inputbuffer);
    Int_encoder_Set_OutputBuffer(audio_process->audio_encoder, audio_process->encoder_outputbuffer);
    Int_decoder_Set_InputBuffer(audio_process->audio_decoder, audio_process->decoder_inputbuffer);
    Int_decoder_Set_OutputBuffer(audio_process->audio_decoder, audio_process->decoder_outputbuffer);

    // 返回 音频处理器对象
    return audio_process;
}

/**
 * @brief 开启 音频处理器对象的任务
 *
 * @param audio_process 音频处理器对象
 */
void App_process_Start(audio_process_t *audio_process)
{
    assert(audio_process);
    // 创建 音频处理任务
    xTaskCreateWithCaps(audio_process_task, "audio_process_task", 32 * 1024, audio_process, 5, NULL, MALLOC_CAP_SPIRAM);
    // 开启其他任务
    Int_decoder_Start(audio_process->audio_decoder);
    Int_encoder_Start(audio_process->audio_encoder);
    Int_SR_Start(audio_process->audio_sr);
}

// 任务函数实现: 音频处理任务
static void audio_process_task(void *args)
{
    MY_LOGE("audio process task is start...\r\n");
    audio_process_t *audio_process = (audio_process_t *)args;

    void *read_data = NULL;
    size_t len = 0;
    while (1)
    {
        // 接收解码器输出缓冲区的数据
        read_data = xRingbufferReceive(audio_process->decoder_outputbuffer,&len,portMAX_DELAY);

        // 把接收到的数据写入到扬声器
        Int_ES8311_WriteToSpeak(read_data,len);

        // 释放缓冲区资源
        vRingbufferReturnItem(audio_process->decoder_outputbuffer,read_data);
    }
}

/**
 * @brief 读取来自编码器输出缓冲区的数据
 *
 * @param audio_process 音频处理器对象
 * @param len 读取数据的长度
 * @return void* 读取到的数据
 */
void *App_process_ReadFromEncoderData(audio_process_t *audio_process, size_t *len)
{
    assert(audio_process);
    // 读取来自编码器输出缓冲区的数据
    void *read_data = xRingbufferReceive(audio_process->encoder_outputbuffer, len, portMAX_DELAY);
    void *data = heap_caps_malloc(*len, MALLOC_CAP_SPIRAM);
    memcpy(data, read_data, *len);
    vRingbufferReturnItem(audio_process->encoder_outputbuffer, read_data);
    return data;
}

/**
 * @brief 写入数据到解码器的输入缓冲区
 *
 * @param audio_process 音频处理器对象
 * @param data 写入的数据
 * @param len 写入的数据长度
 */
void App_process_WriteToDecoderData(audio_process_t *audio_process, void *data, int len)
{
    assert(audio_process);
    // 写入数据到解码器的输入缓冲区
    xRingbufferSend(audio_process->decoder_inputbuffer,data,len,portMAX_DELAY);
}

/**
 * @brief 重置 语音识别系统的唤醒状态
 * 
 * @param audio_process 语音识别对象
 */
void App_process_Reset_Wakenet(audio_process_t *audio_process)
{
    assert(audio_process);

    // 重置 语音识别系统的唤醒状态
    Int_SR_Reset_Wakenet(audio_process->audio_sr);
}

/**
 * @brief 创建 SR模块的唤醒回调函数
 *
 * @param audio_process 音频处理器对象
 * @param wakenet_cb 创建的回调函数
 * @param args 回调函数的参数
 */
void App_process_SR_Set_WakeNet_cb(audio_process_t *audio_process, void (*wakenet_cb)(void *), void *args)
{
    assert(audio_process);

    // 创建唤醒后的回调函数
    Int_SR_Set_WakeNet_cb(audio_process->audio_sr, wakenet_cb, args);
}

/**
 * @brief 创建 SR模块的人声识别回调函数
 *
 * @param audio_process 音频处理器对象
 * @param vad_cb 创建的回调函数
 * @param args 回调函数的参数
 */
void App_process_SR_Set_VAD_cb(audio_process_t *audio_process, void (*vad_cb)(vad_state_t vad_state, void *), void *args)
{
    assert(audio_process);

    // 创建人声识别的回调函数
    Int_SR_Set_VAD_cb(audio_process->audio_sr, vad_cb, args);
}
