#include "Int_decoder.h"

// opus������ �� OPUS��ʽ --> PCM��ʽ
struct audio_decoder
{
    esp_audio_dec_handle_t decoder; // ���������

    RingbufHandle_t outputbufferhandle;
    RingbufHandle_t inputbufferhandle;
};

// ����������: opus������ �� OPUS��ʽ --> PCM��ʽ
static void audio_decoder_task(void *args);

/**
 * @brief ���� opus����������
 *
 * @return audio_decoder_t* opus����������
 */
audio_decoder_t *Int_decoder_Creat(void)
{
    audio_decoder_t *audio_decoder = (audio_decoder_t *)heap_caps_malloc(sizeof(audio_decoder_t), MALLOC_CAP_SPIRAM);
    if (audio_decoder)
    {
        audio_decoder->decoder = NULL; // ��ʽ��ʼ��
        audio_decoder->outputbufferhandle = NULL;
        audio_decoder->inputbufferhandle = NULL;
    }
    // ע�� opus������
    esp_opus_dec_register();

    // ����opus������
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

    // ����opus����������
    esp_audio_dec_open(&dec_cfg, &audio_decoder->decoder);

    // ���� opus����������
    return audio_decoder;
}

/**
 * @brief opus��������ʼ����
 *
 * @param audio_decoder opus����������
 * @note ����������
 */
void Int_decoder_Start(audio_decoder_t *audio_decoder)
{
    assert(audio_decoder);

    // ����������: opus������ �� OPUS��ʽ --> PCM��ʽ
    xTaskCreateWithCaps(audio_decoder_task, "audio_decoder_task", 32 * 1024, audio_decoder, 5, NULL, MALLOC_CAP_SPIRAM);
}

// ������ʵ��: opus������ �� OPUS��ʽ --> PCM��ʽ
static void audio_decoder_task(void *args)
{
    MY_LOGE("audio decoder task is start...\r\n");
    audio_decoder_t *audio_decoder = (audio_decoder_t *)args;

    // // ���� `decode_one_frame` ����һ֡����� OPUS ����
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
    //     // �����뻺������ȡ����
    //     raw.buffer = xRingbufferReceive(audio_decoder->inputbufferhandle, &raw.len, portMAX_DELAY);
    //     // ������Ƶ����
    //     esp_audio_dec_process(audio_decoder->decoder, &raw, &out_frame);
    //     // �ѽ������Ƶ���ݷŵ����������
    //     xRingbufferSend(audio_decoder->outputbufferhandle, out_frame.buffer, out_frame.decoded_size, portMAX_DELAY);

    //     // �ͷ���Դ
    //     vRingbufferReturnItem(audio_decoder->inputbufferhandle, raw.buffer);
    //     vTaskDelay(10);
    // }

    // ���� `decode_one_frame` ����һ֡����� OPUS ����
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
        // �����뻺������ȡ����
        read_data = xRingbufferReceive(audio_decoder->inputbufferhandle, &read_len, portMAX_DELAY);
        raw.buffer = read_data;
        raw.len = read_len;
        // ������Ƶ����
        esp_audio_dec_process(audio_decoder->decoder, &raw, &out_frame);
        // �ѽ������Ƶ���ݷŵ����������
        xRingbufferSend(audio_decoder->outputbufferhandle, out_frame.buffer, out_frame.decoded_size, portMAX_DELAY);

        // �ͷ���Դ
        vRingbufferReturnItem(audio_decoder->inputbufferhandle, read_data);
        vTaskDelay(10);
    }
}

/**
 * @brief Ϊ opus������������������
 *
 * @param audio_decoder opus����������
 * @param outputbufferhandle ������������
 */
void Int_decoder_Set_OutputBuffer(audio_decoder_t *audio_decoder, RingbufHandle_t outputbufferhandle)
{
    assert(audio_decoder);
    assert(outputbufferhandle);

    // ��ӵ��ṹ�������
    audio_decoder->outputbufferhandle = outputbufferhandle;
}

/**
 * @brief Ϊ opus������������뻺����
 *
 * @param audio_decoder opus����������
 * @param inputbufferhandle ���뻺�������
 */
void Int_decoder_Set_InputBuffer(audio_decoder_t *audio_decoder, RingbufHandle_t inputbufferhandle)
{
    assert(audio_decoder);
    assert(inputbufferhandle);

    // ��ӵ��ṹ�������
    audio_decoder->inputbufferhandle = inputbufferhandle;
}
