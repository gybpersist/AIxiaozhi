#include "App_audio.h"

struct audio_process
{
    audio_sr_t *audio_sr;           // ����ʶ�����
    audio_encoder_t *audio_encoder; // ����������
    audio_decoder_t *audio_decoder; // ����������

    RingbufHandle_t encoder_inputbuffer;  // ���������뻺����
    RingbufHandle_t encoder_outputbuffer; // ���������������
    RingbufHandle_t decoder_inputbuffer;  // ���������뻺����
    RingbufHandle_t decoder_outputbuffer; // ���������������
};

// ����������: ��Ƶ��������
static void audio_process_task(void *args);

/**
 * @brief ���� ��Ƶ����������
 *
 * @return audio_process_t* ��Ƶ����������
 */
audio_process_t *App_process_Creat(void)
{
    // ���� ��Ƶ����������
    audio_process_t *audio_process = (audio_process_t *)heap_caps_malloc(sizeof(audio_process_t), MALLOC_CAP_SPIRAM);
    audio_process->audio_sr = Int_SR_Creat();
    audio_process->audio_encoder = Int_encoder_Creat();
    audio_process->audio_decoder = Int_decoder_Creat();
    // ���� ������
    audio_process->encoder_inputbuffer = xRingbufferCreateWithCaps(320 * 1024, RINGBUF_TYPE_BYTEBUF, MALLOC_CAP_SPIRAM);
    audio_process->encoder_outputbuffer = xRingbufferCreateWithCaps(320 * 1024, RINGBUF_TYPE_NOSPLIT, MALLOC_CAP_SPIRAM);
    audio_process->decoder_inputbuffer = xRingbufferCreateWithCaps(320 * 1024, RINGBUF_TYPE_NOSPLIT, MALLOC_CAP_SPIRAM);
    audio_process->decoder_outputbuffer = xRingbufferCreateWithCaps(320 * 1024, RINGBUF_TYPE_NOSPLIT, MALLOC_CAP_SPIRAM);
    Int_SR_Set_OutputBuffer(audio_process->audio_sr,audio_process->encoder_inputbuffer);
    Int_encoder_Set_InputBuffer(audio_process->audio_encoder, audio_process->encoder_inputbuffer);
    Int_encoder_Set_OutputBuffer(audio_process->audio_encoder, audio_process->encoder_outputbuffer);
    Int_decoder_Set_InputBuffer(audio_process->audio_decoder, audio_process->decoder_inputbuffer);
    Int_decoder_Set_OutputBuffer(audio_process->audio_decoder, audio_process->decoder_outputbuffer);

    // ���� ��Ƶ����������
    return audio_process;
}

/**
 * @brief ���� ��Ƶ���������������
 *
 * @param audio_process ��Ƶ����������
 */
void App_process_Start(audio_process_t *audio_process)
{
    assert(audio_process);
    // ���� ��Ƶ��������
    xTaskCreateWithCaps(audio_process_task, "audio_process_task", 32 * 1024, audio_process, 5, NULL, MALLOC_CAP_SPIRAM);
    // ������������
    Int_decoder_Start(audio_process->audio_decoder);
    Int_encoder_Start(audio_process->audio_encoder);
    Int_SR_Start(audio_process->audio_sr);
}

// ������ʵ��: ��Ƶ��������
static void audio_process_task(void *args)
{
    MY_LOGE("audio process task is start...\r\n");
    audio_process_t *audio_process = (audio_process_t *)args;

    void *read_data = NULL;
    size_t len = 0;
    while (1)
    {
        // ���ս��������������������
        read_data = xRingbufferReceive(audio_process->decoder_outputbuffer,&len,portMAX_DELAY);

        // �ѽ��յ�������д�뵽������
        Int_ES8311_WriteToSpeak(read_data,len);

        // �ͷŻ�������Դ
        vRingbufferReturnItem(audio_process->decoder_outputbuffer,read_data);
    }
}

/**
 * @brief ��ȡ���Ա��������������������
 *
 * @param audio_process ��Ƶ����������
 * @param len ��ȡ���ݵĳ���
 * @return void* ��ȡ��������
 */
void *App_process_ReadFromEncoderData(audio_process_t *audio_process, size_t *len)
{
    assert(audio_process);
    // ��ȡ���Ա��������������������
    void *read_data = xRingbufferReceive(audio_process->encoder_outputbuffer, len, portMAX_DELAY);
    void *data = heap_caps_malloc(*len, MALLOC_CAP_SPIRAM);
    memcpy(data, read_data, *len);
    vRingbufferReturnItem(audio_process->encoder_outputbuffer, read_data);
    return data;
}

/**
 * @brief д�����ݵ������������뻺����
 *
 * @param audio_process ��Ƶ����������
 * @param data д�������
 * @param len д������ݳ���
 */
void App_process_WriteToDecoderData(audio_process_t *audio_process, void *data, int len)
{
    assert(audio_process);
    // д�����ݵ������������뻺����
    xRingbufferSend(audio_process->decoder_inputbuffer,data,len,portMAX_DELAY);
}

/**
 * @brief ���� ����ʶ��ϵͳ�Ļ���״̬
 * 
 * @param audio_process ����ʶ�����
 */
void App_process_Reset_Wakenet(audio_process_t *audio_process)
{
    assert(audio_process);

    // ���� ����ʶ��ϵͳ�Ļ���״̬
    Int_SR_Reset_Wakenet(audio_process->audio_sr);
}

/**
 * @brief ���� SRģ��Ļ��ѻص�����
 *
 * @param audio_process ��Ƶ����������
 * @param wakenet_cb �����Ļص�����
 * @param args �ص������Ĳ���
 */
void App_process_SR_Set_WakeNet_cb(audio_process_t *audio_process, void (*wakenet_cb)(void *), void *args)
{
    assert(audio_process);

    // �������Ѻ�Ļص�����
    Int_SR_Set_WakeNet_cb(audio_process->audio_sr, wakenet_cb, args);
}

/**
 * @brief ���� SRģ�������ʶ��ص�����
 *
 * @param audio_process ��Ƶ����������
 * @param vad_cb �����Ļص�����
 * @param args �ص������Ĳ���
 */
void App_process_SR_Set_VAD_cb(audio_process_t *audio_process, void (*vad_cb)(vad_state_t vad_state, void *), void *args)
{
    assert(audio_process);

    // ��������ʶ��Ļص�����
    Int_SR_Set_VAD_cb(audio_process->audio_sr, vad_cb, args);
}
