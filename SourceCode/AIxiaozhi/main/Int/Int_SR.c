#include "Int_SR.h"

// ����ʶ�����
struct audio_sr
{
    esp_afe_sr_iface_t *afe_handle; // AFEʵ�����
    esp_afe_sr_data_t *afe_data;    // AFEʵ��

    bool is_wakeup; // �Ƿ��ѱ�־

    vad_state_t last_vad_state; // �ϴ�����״̬

    RingbufHandle_t ringbufferhandle; // ���λ��������

    void *wakenet_args;                            // ���ѻص������Ĳ���
    void (*wakenet_cb)(void *);                    // ���ѻص�����
    void *vad_args;                                // ����ʶ��ص������Ĳ���
    void (*vad_cb)(vad_state_t vad_state, void *); // ����ʶ��Ļص�����
};

// ����������: ����AFE��ѧǰ�˵�������
static void audio_sr_fetch_task(void *args);
// ����������: ������mic������д�뵽AFE��ѧǰ��
static void audio_sr_feed_task(void *args);

/**
 * @brief ���� ����ʶ��ϵͳ
 *
 * @return audio_sr_t* ����ʶ�����
 */
audio_sr_t *Int_SR_Creat(void)
{
    // Ϊ����ʶ�����������Դ
    audio_sr_t *audio_sr = (audio_sr_t *)heap_caps_malloc(sizeof(audio_sr_t), MALLOC_CAP_SPIRAM);

    // ��ʼ��AFE����
    srmodel_list_t *models = esp_srmodel_init("model");
    afe_config_t *afe_config = afe_config_init("M", models, AFE_TYPE_SR, AFE_MODE_HIGH_PERF);
    afe_config->aec_init = false;
    afe_config->se_init = false;
    afe_config->ns_init = false;
    afe_config->agc_init = false;

    afe_config->wakenet_init = true;
    afe_config->wakenet_mode = DET_MODE_95; // Aggressive
    afe_config->vad_init = true;
    afe_config->vad_mode = VAD_MODE_2; // Very Aggressive
    afe_config->memory_alloc_mode = AFE_MEMORY_ALLOC_MORE_PSRAM;

    // ����AFEʵ��
    // ��ȡ���
    audio_sr->afe_handle = esp_afe_handle_from_config(afe_config);
    // ����ʵ��
    audio_sr->afe_data = audio_sr->afe_handle->create_from_config(afe_config);

    // ������Ƶ����
    return audio_sr;
}

/**
 * @brief ���� ����ʶ��ϵͳ
 *
 * @param audio_sr ����ʶ�����
 * @note ������������ (������mic������д�뵽AFE��ѧǰ��) (����AFE��ѧǰ�˵�������)
 */
void Int_SR_Start(audio_sr_t *audio_sr)
{
    // �ж� audio_sr�Ƿ�Ϊ��
    assert(audio_sr);
    // Ϊ����״̬����ֵ
    audio_sr->is_wakeup = false;
    // Ϊ�ϴ�����״̬����ֵ
    audio_sr->last_vad_state = VAD_SILENCE;

    // �������� ����AFE��ѧǰ�˵������� (fetch:����,ȡ��)
    xTaskCreateWithCaps(audio_sr_fetch_task, "audio_sr_fetch_task", 32 * 1024, audio_sr, 5, NULL, MALLOC_CAP_SPIRAM);
    // �������� ������mic������д�뵽AFE��ѧǰ�� (feed:ι��,�ṩ)
    xTaskCreateWithCaps(audio_sr_feed_task, "audio_sr_feed_task", 32 * 1024, audio_sr, 5, NULL, MALLOC_CAP_SPIRAM);
}

/**
 * @brief ���� ����ʶ��ϵͳ�Ļ���״̬
 * 
 * @param audio_sr ����ʶ�����
 */
void Int_SR_Reset_Wakenet(audio_sr_t *audio_sr)
{
    // �ж� audio_sr�Ƿ�Ϊ��
    assert(audio_sr);
    // ���û���״̬
    audio_sr->is_wakeup = false;
    // �����ϴ�����״̬
    audio_sr->last_vad_state = VAD_SILENCE;
}

// ʵ��������: ������mic������д�뵽AFE��ѧǰ��
static void audio_sr_feed_task(void *args)
{
    MY_LOGE("feed task is start...\r\n");
    audio_sr_t *audio_sr = args;
    esp_afe_sr_iface_t *afe_handle; // AFEʵ�����
    esp_afe_sr_data_t *afe_data;    // AFEʵ��
    afe_handle = audio_sr->afe_handle;
    afe_data = audio_sr->afe_data;

    // ����Ƶ��������AFE���д��� (feed_chunksize��ÿ֡����������� ,eed_nch���������ݵ�ͨ���� ,feed_buff��ͨ���������Ƶ����)
    int feed_chunksize = afe_handle->get_feed_chunksize(afe_data);
    int feed_nch = afe_handle->get_feed_channel_num(afe_data);
    int feed_size = feed_chunksize * feed_nch * sizeof(int16_t);
    int16_t *feed_buff = (int16_t *)malloc(feed_size);

    while (1)
    {
        // ��mic��ȡ����
        Int_ES8311_ReadFromMic(feed_buff, feed_size);
        afe_handle->feed(afe_data, feed_buff);
        vTaskDelay(10);
    }
}

// ʵ��������: ����AFE��ѧǰ�˵�������
static void audio_sr_fetch_task(void *args)
{
    MY_LOGE("fetch task is start...\r\n");
    audio_sr_t *audio_sr = args;
    esp_afe_sr_iface_t *afe_handle; // AFEʵ�����
    esp_afe_sr_data_t *afe_data;    // AFEʵ��
    afe_handle = audio_sr->afe_handle;
    afe_data = audio_sr->afe_data;

    // ����Ƶ��������AFE���д��� (feed_chunksize��ÿ֡����������� ,eed_nch���������ݵ�ͨ���� ,feed_buff��ͨ���������Ƶ����)
    int feed_chunksize = afe_handle->get_feed_chunksize(afe_data);
    int feed_nch = afe_handle->get_feed_channel_num(afe_data);
    int feed_size = feed_chunksize * feed_nch * sizeof(int16_t);

    while (1)
    {
        // ��ȡ�����ĵ�ͨ����Ƶ����ͼ��״̬
        afe_fetch_result_t *result = afe_handle->fetch(afe_data);
        if (result == NULL)
        { // ������ NULL
            MY_LOGE("AFE fetch failed: result is NULL");
            vTaskDelay(10);
            continue;
        }
        wakenet_state_t wakeup_state = result->wakeup_state;
        vad_state_t vad_state = result->vad_state;

        // get the processed audio with specified delay, default delay is 2000 ms
        // afe_fetch_result_t *delayed_result  = afe_handle->fetch_with_delay(afe_data, 100 / portTICK_PERIOD_MS);

        // ��⵽���Ѵ�
        if (wakeup_state == WAKENET_DETECTED)
        {
            audio_sr->is_wakeup = true;
            if (audio_sr->wakenet_cb != NULL)
            {
                // ���û��ѻص�����
                audio_sr->wakenet_cb(audio_sr->wakenet_args);
            }
        }

        // ����ڻ���״̬������״̬�����ı�
        if (audio_sr->is_wakeup == true)
        {
            if (audio_sr->last_vad_state != vad_state)
            {
                // �����ϴ�����״̬
                audio_sr->last_vad_state = vad_state;
                if (audio_sr->vad_cb != NULL)
                {
                    // ����ʶ�������ص�����
                    audio_sr->vad_cb(vad_state, audio_sr->vad_args);
                }
            }
        }

        // �����ǰΪ����״̬���Ҽ�⵽����,��ô��Ҫ����Ƶ���ݱ�����������
        if (vad_state == VAD_SPEECH && audio_sr->is_wakeup == true && Com_Status == Com_Status_LISTENING)
        {
            // ȡ����Ƶ����
            int16_t *data = result->raw_data;

            // ������д�뻷�λ�����
            xRingbufferSend(audio_sr->ringbufferhandle, data, feed_size, portMAX_DELAY);
        }

        // vTaskDelay(10);
    }
}

/**
 * @brief Ϊ����ʶ��ϵͳ������������
 *
 * @param audio_sr ����ʶ�����
 * @param ringbuffer ����������
 */
void Int_SR_Set_OutputBuffer(audio_sr_t *audio_sr, RingbufHandle_t ringbufferhandle)
{
    // �ж� audio ringbufferhandle�Ƿ�Ϊ��
    assert(audio_sr);
    assert(ringbufferhandle);

    // ��ӵ��ṹ�������
    audio_sr->ringbufferhandle = ringbufferhandle;
}

/**
 * @brief �������Ѻ�Ļص�����
 *
 * @param audio_sr ����ʶ�����
 * @param wakenet_cb ���Ѻ�Ļص�����
 * @param args ��������
 */
void Int_SR_Set_WakeNet_cb(audio_sr_t *audio_sr, void (*wakenet_cb)(void *), void *args)
{
    // �ж� audio_sr�Ƿ�Ϊ��
    assert(audio_sr);

    // ��ӵ��ṹ�������
    audio_sr->wakenet_cb = wakenet_cb;
    audio_sr->wakenet_args = args;
}

/**
 * @brief ��������ʶ��Ļص�����
 *
 * @param audio_sr ����ʶ�����
 * @param vad_cb ����ʶ��Ļص�����
 * @param args ��������
 */
void Int_SR_Set_VAD_cb(audio_sr_t *audio_sr, void (*vad_cb)(vad_state_t vad_state, void *), void *args)
{
    // �ж� audio_sr�Ƿ�Ϊ��
    assert(audio_sr);

    // ��ӵ��ṹ�������
    audio_sr->vad_cb = vad_cb;
    audio_sr->vad_args = args;
}
