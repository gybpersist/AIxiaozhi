#include "Int_decoder.h"

// opus解码器 将 OPUS格式 --> PCM格式
struct audio_decoder
{
    esp_audio_dec_handle_t decoder; // 解码器句柄

    RingbufHandle_t outputbufferhandle;
    RingbufHandle_t inputbufferhandle;
};

// 任务函数声明: opus解码器 将 OPUS格式 --> PCM格式
static void audio_decoder_task(void *args);

/**
 * @brief 创建 opus解码器对象
 *
 * @return audio_decoder_t* opus解码器对象
 */
audio_decoder_t *Int_decoder_Creat(void)
{
    audio_decoder_t *audio_decoder = (audio_decoder_t *)heap_caps_malloc(sizeof(audio_decoder_t), MALLOC_CAP_SPIRAM);
    if (audio_decoder)
    {
        audio_decoder->decoder = NULL; // 显式初始化
        audio_decoder->outputbufferhandle = NULL;
        audio_decoder->inputbufferhandle = NULL;
    }
    // 注册 opus解码器
    esp_opus_dec_register();

    // 配置opus解码器
    esp_opus_dec_cfg_t opus_cfg = {
        .sample_rate = ESP_AUDIO_SAMPLE_RATE_16K,
        .channel = ESP_AUDIO_MONO,
        .frame_duration = ESP_OPUS_DEC_FRAME_DURATION_60_MS,
        .self_delimited = false,
    };
    esp_audio_dec_cfg_t dec_cfg = {
        .type = ESP_AUDIO_TYPE_OPUS,
        .cfg = &opus_cfg,
        .cfg_sz = sizeof(opus_cfg),
    };

    // 创建opus解码器对象
    esp_audio_dec_open(&dec_cfg, &audio_decoder->decoder);

    // 返回 opus解码器对象
    return audio_decoder;
}

/**
 * @brief opus解码器开始工作
 *
 * @param audio_decoder opus解码器对象
 * @note 启动任务函数
 */
void Int_decoder_Start(audio_decoder_t *audio_decoder)
{
    assert(audio_decoder);

    // 创建任务函数: opus解码器 将 OPUS格式 --> PCM格式
    xTaskCreateWithCaps(audio_decoder_task, "audio_decoder_task", 32 * 1024, audio_decoder, 5, NULL, MALLOC_CAP_SPIRAM);
}

// 任务函数实现: opus解码器 将 OPUS格式 --> PCM格式
static void audio_decoder_task(void *args)
{
    MY_LOGE("audio decoder task is start...\r\n");
    audio_decoder_t *audio_decoder = (audio_decoder_t *)args;

    // // 调用 `decode_one_frame` 解码一帧编码的 OPUS 数据
    // esp_audio_dec_in_raw_t raw = {
    //     .buffer = NULL,
    //     .len = 0,
    // };
    // uint8_t *out_buffer = heap_caps_malloc(1920, MALLOC_CAP_SPIRAM);
    // esp_audio_dec_out_frame_t out_frame = {
    //     .buffer = out_buffer,
    //     .len = 1920,
    // };

    // while (1)
    // {
    //     // 从输入缓冲区读取数据
    //     raw.buffer = xRingbufferReceive(audio_decoder->inputbufferhandle, &raw.len, portMAX_DELAY);
    //     // 解码音频数据
    //     esp_audio_dec_process(audio_decoder->decoder, &raw, &out_frame);
    //     // 把解码的音频数据放到输出缓冲区
    //     xRingbufferSend(audio_decoder->outputbufferhandle, out_frame.buffer, out_frame.decoded_size, portMAX_DELAY);

    //     // 释放资源
    //     vRingbufferReturnItem(audio_decoder->inputbufferhandle, raw.buffer);
    //     vTaskDelay(10);
    // }

    // 调用 `decode_one_frame` 解码一帧编码的 OPUS 数据
    esp_audio_dec_in_raw_t raw = {0};
    uint8_t *out_buffer = heap_caps_malloc(1920, MALLOC_CAP_SPIRAM);
    esp_audio_dec_out_frame_t out_frame = {
        .buffer = out_buffer,
        .len = 1920,
    };

    size_t read_len = 0;
    void *read_data = NULL;
    while (1)
    {
        // 从输入缓冲区读取数据
        read_data = xRingbufferReceive(audio_decoder->inputbufferhandle, &read_len, portMAX_DELAY);
        raw.buffer = read_data;
        raw.len = read_len;
        // 解码音频数据
        esp_audio_dec_process(audio_decoder->decoder, &raw, &out_frame);
        // 把解码的音频数据放到输出缓冲区
        xRingbufferSend(audio_decoder->outputbufferhandle, out_frame.buffer, out_frame.decoded_size, portMAX_DELAY);

        // 释放资源
        vRingbufferReturnItem(audio_decoder->inputbufferhandle, read_data);
        vTaskDelay(10);
    }
}

/**
 * @brief 为 opus解码器添加输出缓冲区
 *
 * @param audio_decoder opus解码器对象
 * @param outputbufferhandle 输出缓冲区句柄
 */
void Int_decoder_Set_OutputBuffer(audio_decoder_t *audio_decoder, RingbufHandle_t outputbufferhandle)
{
    assert(audio_decoder);
    assert(outputbufferhandle);

    // 添加到结构体对象中
    audio_decoder->outputbufferhandle = outputbufferhandle;
}

/**
 * @brief 为 opus解码器添加输入缓冲区
 *
 * @param audio_decoder opus解码器对象
 * @param inputbufferhandle 输入缓冲区句柄
 */
void Int_decoder_Set_InputBuffer(audio_decoder_t *audio_decoder, RingbufHandle_t inputbufferhandle)
{
    assert(audio_decoder);
    assert(inputbufferhandle);

    // 添加到结构体对象中
    audio_decoder->inputbufferhandle = inputbufferhandle;
}
