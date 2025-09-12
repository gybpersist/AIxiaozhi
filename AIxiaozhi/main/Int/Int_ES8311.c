#include "Int_ES8311.h"

// I2S�������ͨ������
i2s_chan_handle_t speaker_tx_handle;
i2s_chan_handle_t mic_rx_handle;

// ES8311�豸��������
esp_codec_dev_handle_t codec_dev;

/**
 * @brief ��ʼ�� I2C
 *
 */
static void Int_ES8311_I2C_Init(void)
{
    // ���� I2C ����
    i2c_config_t i2c_cfg = {
        .mode = I2C_MODE_MASTER,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
    };
    i2c_cfg.sda_io_num = ES8311_I2C_SDA_PIN;
    i2c_cfg.scl_io_num = ES8311_I2C_SCL_PIN;
    // ʹ������Ч
    i2c_param_config(I2C_NUM_0, &i2c_cfg);
    // ��װ I2C ����
    i2c_driver_install(I2C_NUM_0, i2c_cfg.mode, 0, 0, 0);
}

/**
 * @brief ��ʼ�� I2S
 *
 */
static void Int_ES8311_I2S_Init(void)
{
    // I2SĬ�ϲ�������
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    // ��Ƶ��׼��ʽ/ԭʼ��ʽ(PCM)������Ϣ
    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(16000),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(16, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = ES8311_I2S_MCK_PIN,
            .bclk = ES8311_I2S_BCK_PIN,
            .ws = ES8311_I2S_WS_PIN,
            .dout = ES8311_I2S_DO_PIN,
            .din = ES8311_I2S_DI_PIN,
        },
    };

    // ���������뷢��ͨ��
    i2s_new_channel(&chan_cfg, &speaker_tx_handle, &mic_rx_handle);

    // ��ʼ�������뷢��ͨ��
    i2s_channel_init_std_mode(speaker_tx_handle, &std_cfg);
    i2s_channel_init_std_mode(mic_rx_handle, &std_cfg);

    // ʹ�ܽ����뷢��ͨ��
    i2s_channel_enable(speaker_tx_handle);
    i2s_channel_enable(mic_rx_handle);
}

/**
 * @brief ��ʼ�� �豸
 *
 */
static void Int_ES8311_DEV_Init(void)
{
    // Create the control and data interfaces for the codec using the interface provided by default
    // I2Sͨ������
    audio_codec_i2s_cfg_t i2s_cfg = {
        .rx_handle = mic_rx_handle,
        .tx_handle = speaker_tx_handle,
    };
    // ʹ��I2S���ô������ݽӿ�
    const audio_codec_data_if_t *data_if = audio_codec_new_i2s_data(&i2s_cfg);

    // I2C������Ϣ
    audio_codec_i2c_cfg_t i2c_cfg = {
        .addr = ES8311_CODEC_DEFAULT_ADDR,
        // .bus_handle,  // ��I2C��Ҫ���õ�
        .port = I2C_NUM_0,
    };
    // ʹ��I2C���ô���������ƽӿ�
    const audio_codec_ctrl_if_t *out_ctrl_if = audio_codec_new_i2c_ctrl(&i2c_cfg);

    // ����GPIO�ӿ�
    const audio_codec_gpio_if_t *gpio_if = audio_codec_new_gpio();

    // Create the codec interface based on control interface and codec-specified configuration
    // ����ES8311�����������
    es8311_codec_cfg_t es8311_cfg = {
        .codec_mode = ESP_CODEC_DEV_WORK_MODE_BOTH,
        .ctrl_if = out_ctrl_if,
        .gpio_if = gpio_if,
        .pa_pin = ES8311_PA_EN_PIN,
        .use_mclk = true,
    };
    // ������������ӿ�
    const audio_codec_if_t *out_codec_if = es8311_codec_new(&es8311_cfg);

    // Get esp_codec_dev_handle_t through esp_codec_dev_new
    // Now you can use the handle for further playback and recording as follows:
    // �豸������Ϣ
    esp_codec_dev_cfg_t dev_cfg = {
        .codec_if = out_codec_if,              // codec interface from es8311_codec_new
        .data_if = data_if,                    // data interface from audio_codec_new_i2s_data
        .dev_type = ESP_CODEC_DEV_TYPE_IN_OUT, // codec support both playback and record
    };
    // ����ES8311�豸��������
    codec_dev = esp_codec_dev_new(&dev_cfg);
    // �����������
    esp_codec_dev_set_out_vol(codec_dev, 60.0);
    // ������������
    esp_codec_dev_set_in_gain(codec_dev, 20.0);
    // ��������
    esp_codec_dev_sample_info_t fs = {
        .sample_rate = 16000,                               // �����ʣ�16000Hz���� I2S_STD_CLK_DEFAULT_CONFIG һ�£�
        .channel = 1,                                       // ͨ������1������������ I2S_SLOT_MODE_MONO һ�£�
        .channel_mask = ESP_CODEC_DEV_MAKE_CHANNEL_MASK(0), // ͨ�����룺ѡ��� 0 ͨ����������Ĭ�ϣ�
        .bits_per_sample = 16,                              // ����λ��16bit���� I2S_STD_PHILIPS_SLOT һ�£�
        .mclk_multiple = 256,                               // MCLK ��Ƶϵ����256�������ʣ�ES8311 �Ƽ����ã�ȷ��ʱ���ȶ���
    };
    // �����豸
    esp_codec_dev_open(codec_dev, &fs);
}

/**
 * @brief es8311 ���������ʼ��
 *
 */
void Int_ES8311_Init(void)
{
    // ��ʼ�� I2C
    Int_ES8311_I2C_Init();

    // ��ʼ�� I2S
    Int_ES8311_I2S_Init();

    // ��ʼ�� �豸
    Int_ES8311_DEV_Init();
}

/**
 * @brief ����˷��ȡ����
 *
 * @param data ��ȡ��������
 * @param len Ҫ��ȡ�����ݳ���
 */
void Int_ES8311_ReadFromMic(void *data, int len)
{
    if (codec_dev != NULL && len > 0)
    {
        esp_codec_dev_read(codec_dev, data, len);
    }
}

/**
 * @brief д�뵽����
 *
 * @param data д�������
 */
void Int_ES8311_WriteToSpeak(void *data, int len)
{
    if (codec_dev != NULL && len > 0)
    {
        esp_codec_dev_write(codec_dev, data, len);
    }
}
