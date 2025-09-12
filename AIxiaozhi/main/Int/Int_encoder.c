#include "Int_encoder.h"

// opus������ �� PCM��ʽ --> OPUS��ʽ
struct audio_encoder
{
    esp_audio_enc_handle_t encoder; // ���������

    RingbufHandle_t outputbufferhandle; // ������������
    RingbufHandle_t inputbufferhandle;  // ���뻺�������
};

// ��������: opus�������������� (PCM��ʽ --> OPUS��ʽ)������
static void audio_encoder_task(void *args);

// opus�����������뻺������ȡ���ݵ�pcm_data,ÿ�ζ���Ҫ��֤1920���ֽ� ��ʱ��������pcm_data��������ζ�ȡ����Ƭ������ƴ�ӳ�����֡���ٽ�������������
static void Int_encoder_ReadFromBuffer(RingbufHandle_t inputbufferhandle, uint8_t *pcm_data, int pcm_size)
{
    void *read_data = NULL;
    size_t read_size = 0;
    while (pcm_size > 0)
    {
        read_data = xRingbufferReceiveUpTo(inputbufferhandle, &read_size, portMAX_DELAY, pcm_size);
        memcpy(pcm_data,read_data,read_size);
        pcm_size -= read_size;
        pcm_data += read_size; 

        // �ͷ���Դ
        vRingbufferReturnItem(inputbufferhandle,read_data);
    }
}

/**
 * @brief ���� opus���������� ��ע�� opus������
 *
 * @return audio_encoder_t* opus����������
 */
audio_encoder_t *Int_encoder_Creat(void)
{
    // ���� opus������(PCM��ʽ --> OPUS��ʽ)������
    audio_encoder_t *audio_encoder = (audio_encoder_t *)heap_caps_malloc(sizeof(audio_encoder_t), MALLOC_CAP_SPIRAM);
    if (audio_encoder)
    {
        audio_encoder->encoder = NULL; // ��ʽ��ʼ��
        audio_encoder->outputbufferhandle = NULL;
        audio_encoder->inputbufferhandle = NULL;
    }
    // ע�� opus������
    esp_opus_enc_register();
    // opus������������
    esp_opus_enc_config_t opus_cfg = {
        .sample_rate = ESP_AUDIO_SAMPLE_RATE_16K,
        .channel = ESP_AUDIO_MONO,
        .bits_per_sample = ESP_AUDIO_BIT16,
        .bitrate = 90000,
        .frame_duration = ESP_OPUS_ENC_FRAME_DURATION_60_MS,
        .application_mode = ESP_OPUS_ENC_APPLICATION_VOIP,
        .complexity = 5,
        .enable_fec = false,
        .enable_dtx = false,
        .enable_vbr = false,
    };
    esp_audio_enc_config_t enc_cfg = {
        .type = ESP_AUDIO_TYPE_OPUS,
        .cfg = &opus_cfg,
        .cfg_sz = sizeof(opus_cfg)};
    // ���� opus����������
    esp_audio_enc_open(&enc_cfg, &audio_encoder->encoder);

    // ���ر���������
    return audio_encoder;
}

/**
 * @brief opus��������ʼ����
 *
 * @param audio_encoder opus����������
 * @note ����������
 */
void Int_encoder_Start(audio_encoder_t *audio_encoder)
{
    assert(audio_encoder);

    // �������� opus�������������� (PCM��ʽ --> OPUS��ʽ)������
    xTaskCreateWithCaps(audio_encoder_task, "audio_encoder_task", 32 * 1024, audio_encoder, 5, NULL, MALLOC_CAP_SPIRAM);
}

// ����ʵ��: opus�������������� (PCM��ʽ --> OPUS��ʽ)������
static void audio_encoder_task(void *args)
{
    MY_LOGE("audio encoder task is start...\r\n");
    audio_encoder_t *audio_encoder = (audio_encoder_t *)args;
    // ��ȡ����Ļ�������С��׼���ڴ� (pcm_data:ԭ��δ��������,raw_data:ԭ��ѹ������)
    int pcm_size = 0, raw_size = 0;
    esp_audio_enc_get_frame_size(audio_encoder->encoder, &pcm_size, &raw_size);
    // 16000Hz 16λ(2�ֽ�)  60ms  ��ͨ��
    MY_LOGE("PCM_SIZE = %d", pcm_size); // 1920 = 16000 * 2 * 60 / 1000

    uint8_t *pcm_data = heap_caps_malloc(pcm_size, MALLOC_CAP_SPIRAM);
    uint8_t *raw_data = heap_caps_malloc(raw_size, MALLOC_CAP_SPIRAM);

    // �������ݵ�׼�� (frame:���,�ṹ)
    esp_audio_enc_in_frame_t in_frame = {
        .buffer = pcm_data,
        .len = pcm_size,
    };
    esp_audio_enc_out_frame_t out_frame = {
        .buffer = raw_data,
        .len = raw_size,
    };

    while (1)
    {
        // opus�����������뻺������ȡ���ݵ�pcm_data,ÿ�ζ���Ҫ��֤1920���ֽ� ��ʱ��������pcm_data��������ζ�ȡ����Ƭ������ƴ�ӳ�����֡���ٽ�������������
        Int_encoder_ReadFromBuffer(audio_encoder->inputbufferhandle, pcm_data, pcm_size);
        // �����Ѿ���ȡ����pcm_data����
        esp_audio_enc_process(audio_encoder->encoder, &in_frame, &out_frame);
        // �Ѵ�����ɵ����ݷŵ����������
        xRingbufferSend(audio_encoder->outputbufferhandle, out_frame.buffer, out_frame.encoded_bytes, portMAX_DELAY);

        vTaskDelay(10);
    }
}

/**
 * @brief Ϊ opus������������������
 *
 * @param audio_encoder opus����������
 * @param outputbufferhandle ������������
 */
void Int_encoder_Set_OutputBuffer(audio_encoder_t *audio_encoder, RingbufHandle_t outputbufferhandle)
{
    assert(audio_encoder);
    assert(outputbufferhandle);

    // ��ӵ��ṹ�������
    audio_encoder->outputbufferhandle = outputbufferhandle;
}

/**
 * @brief Ϊ opus������������뻺����
 *
 * @param audio_encoder opus����������
 * @param inputbufferhandle ���뻺�������
 */
void Int_encoder_Set_InputBuffer(audio_encoder_t *audio_encoder, RingbufHandle_t inputbufferhandle)
{
    assert(audio_encoder);
    assert(inputbufferhandle);

    // ��ӵ��ṹ�������
    audio_encoder->inputbufferhandle = inputbufferhandle;
}
