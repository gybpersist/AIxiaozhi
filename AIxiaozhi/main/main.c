#include <stdio.h>
#include "esp_task.h"
#include "Int_ES8311.h"
#include "Dri_WIFI.h"
#include "Int_SR.h"
#include "App_audio.h"
#include "Dri_http.h"
#include "Dri_websocket.h"
#include "Com_status.h"

// 音频处理结构体
audio_process_t *audio_process;

// 唤醒后的回调函数
static void app_main_wakenet_cb(void *args)
{
    MY_LOGE("AI xiao zi is wakeup...\r\n");
    // 小智如果在空闲状态就可以切换到连接状态
    if (Com_Status == Com_Status_IDLE)
    {
        // 小智切换到 连接状态
        Com_Status_Switch(Com_Status_CONNECTION);

        // 启动 websocket客户端
        Dri_websocket_Start();
        // 发送 hello 消息
        Dri_websocket_SendHello();
        // 发送需要AI控制的外设
        Dri_websocket_Send_iot_dev();
        // 发送需要AI控制的外设当前的状态
        Dri_websocket_Send_iot_dev_status();
    } // 小智在Com_Status_SPEAKERING状态就进行打断
    else if (Com_Status == Com_Status_SPEAKERING)
    {
        // 发送 打断请求
        Dri_websocket_SendAbort();
    }
    // 发送唤醒信息
    Dri_websocket_SendWakeNet();
    // 小智切换到 空闲状态
    Com_Status_Switch(Com_Status_IDLE);
}

// 语音检测的回调函数
static void app_main_vad_cb(vad_state_t vad_state, void *args)
{
    // 没有在说话
    if (vad_state == VAD_SILENCE)
    {
        MY_LOGI("vad state: VAD_SLENCE...\r\n");

        if (Com_Status == Com_Status_LISTENING)
        {
            // 小智切换到 空闲状态
            Com_Status_Switch(Com_Status_IDLE);

            // 停止监听信息
            Dri_websocket_SendStopListen();
        }
    }
    // 正在说话
    else
    {
        MY_LOGI("vad state: VAD_SPEECH...\r\n");

        if (Com_Status == Com_Status_IDLE)
        {
            // 切换到 聆听状态
            Com_Status_Switch(Com_Status_LISTENING);

            // 开始 监听信息
            Dri_websocket_SendStartListen();
        }
    }
}

// 接收到文本数据的回调函数
static void app_main_ws_text_cb(const char *data, int len)
{
    // 此处应使用 cJSON 解析接收到的信息
    // 将cJSON字符串转换为cJSON对象
    cJSON *root = cJSON_Parse(data);

    // 获取当前消息的类型
    cJSON *type = cJSON_GetObjectItem(root, "type");
    char *type_str = cJSON_GetStringValue(type);

    // 根据 type_str 的不同执行对应的处理
    if (strcmp(type_str, "hello") == 0)
    {
        MY_LOGE("receive Hello data : %.*s", len, data);

        // 提取session_id
        cJSON *s_id = cJSON_GetObjectItem(root, "session_id");

        char *s_id_str = cJSON_GetStringValue(s_id);
        memcpy(session_id, s_id_str, strlen(s_id_str));
        MY_LOGE("session_id:%s", session_id);

        // 接收到HELLO消息的事件标志
        xEventGroupSetBits(ws_eventgroup, WS_HELLO_EVENT);
    } // 语音识别结果（Speech-to-Text）
    else if (strcmp(type_str, "stt") == 0)
    {
        MY_LOGE("receive Speech-to-Text data : %.*s", len, data);
    } // 接收到 iot 信息
    else if (strcmp(type_str, "iot") == 0)
    {
        MY_LOGE("receive iot data : %.*s", len, data);

        // 提取 commands
        cJSON *commands = cJSON_GetObjectItem(root, "commands");
        int commands_size = cJSON_GetArraySize(commands);
        for (int i = 0; i < commands_size; i++)
        {
            cJSON *method_json = cJSON_GetArrayItem(commands, i);

            // 提取 method
            cJSON *method = cJSON_GetObjectItem(method_json, "method");
            char *method_str = cJSON_GetStringValue(method);

            if (strcmp(method_str, "SetVolume") == 0)
            {
                // 提取 parameters 的 volume参数
                cJSON *parameters = cJSON_GetObjectItem(method_json, "parameters");
                cJSON *volume = cJSON_GetObjectItem(parameters, "volume");
                double volume_num = cJSON_GetNumberValue(volume);

                // 设置音量大小
                Int_ES8311_SetVolume((int)volume_num);
            }
            else if (strcmp(method_str, "SetMute") == 0)
            {
                // 提取 parameters 的 Mute参数
                cJSON *parameters = cJSON_GetObjectItem(method_json, "parameters");
                cJSON *Mute = cJSON_GetObjectItem(parameters, "Mute");

                if (cJSON_IsTrue(Mute))
                {
                    Int_ES8311_SetMute(true);
                }
                else
                {
                    Int_ES8311_SetMute(false);
                }
            }
        }

    } // 表情回复
    else if (strcmp(type_str, "llm") == 0)
    {
        MY_LOGE("receive Speech-to-Text data : %.*s", len, data);
    } // 文字回复（Text-to-Speech）
    else if (strcmp(type_str, "tts") == 0)
    {
        MY_LOGE("receive Text-to-Speech data : %.*s", len, data);

        // 提取state
        cJSON *state = cJSON_GetObjectItem(root, "state");
        char *state_str = cJSON_GetStringValue(state);
        if (strcmp(state_str, "start") == 0)
        {
            // 小智切换到 讲话状态
            Com_Status_Switch(Com_Status_SPEAKERING);
        }
        else if (strcmp(state_str, "stop") == 0)
        {
            // 小智切换到 空闲状态
            Com_Status_Switch(Com_Status_IDLE);
        }
    }

    // 释放资源
    cJSON_Delete(root);
}

// 接收到二进制数据的回调函数
static void app_main_ws_bin_cb(const char *data, int len)
{
    // 将二进制数据写入到解码器的数据缓冲区
    App_process_WriteToDecoderData(audio_process, (void *)data, len);
}

// 结束的回调函数
static void app_main_ws_finish_cb(void)
{
    // 重置 语音识别系统的唤醒状态
    App_process_Reset_Wakenet(audio_process);

    // 小智切换到空闲状态
    Com_Status_Switch(Com_Status_IDLE);
}

void app_main(void)
{
    printf("AI小智\r\n");

    // 小智切换到 开始状态
    Com_Status_Switch(Com_Status_STRAT);

    Int_ES8311_Init();
    Dri_nvs_Init();
    Dri_WIFI_Init();
    Dri_http_Init();
    Dri_websocket_Init(app_main_ws_text_cb, app_main_ws_bin_cb, app_main_ws_finish_cb);

    // 创建 音频处理结构体
    audio_process = App_process_Creat();
    // 设置 SR模块的唤醒回调函数
    App_process_SR_Set_WakeNet_cb(audio_process, app_main_wakenet_cb, NULL);
    // 设置 SR模块的语音检测回调函数
    App_process_SR_Set_VAD_cb(audio_process, app_main_vad_cb, NULL);
    // 启动 音频处理流程
    App_process_Start(audio_process);

    // 小智切换到 空闲状态
    Com_Status_Switch(Com_Status_IDLE);

    size_t len = 0;

    while (1)
    {

        // 从编码器读取编码后的数据
        void *data = App_process_ReadFromEncoderData(audio_process, &len);

        // 把数据通过websocket客户端发送给服务器
        Dri_websocket_SendOpusTOService(data, (int)len);

        // 释放资源
        free(data);

        // vTaskDelay(10);
    }

    // void *data = NULL;
    // while (1)
    // {

    //     // 从opus解码器的缓冲区读取数据
    //     data = App_process_ReadFromEncoderData(audio_process, &len);

    //     // Int_ES8311_ReadFromMic(data, 512);
    //     App_process_WriteToDecoderData(audio_process,data, len);

    //     vTaskDelay(10);
    // }
}

// #include <stdio.h>
// #include "esp_task.h"
// #include "Int_ES8311.h"
// #include "Dri_WIFI.h"
// #include "Int_SR.h"
// #include "Int_encoder.h"
// #include "Int_decoder.h"

// // uint8_t data[512] = {0};

// // 唤醒后的回调函数
// static void app_main_wakenet_cb(void *args)
// {
//     MY_LOGE("AI xiao zi is wakeup...\r\n");
// }

// // 语音检测的回调函数
// static void app_main_vad_cb(vad_state_t vad_state, void *args)
// {
//     if (vad_state == VAD_SILENCE)
//     {
//         MY_LOGE("vad state: VAD_SLENCE...\r\n");
//     }
//     else
//     {
//         MY_LOGE("vad state: VAD_SPEECH...\r\n");
//     }
// }

// void app_main(void)
// {
//     printf("hello\r\n");

//     Int_ES8311_Init();
//     // Dri_WIFI_Init();

//     // 创建 语音识别系统对象
//     audio_sr_t *audio_sr = Int_SR_Creat();
//     // 创建 opus编码器对象
//     audio_encoder_t *audio_encoder = Int_encoder_Creat();
//     // 创建 opus解码器对象
//     audio_decoder_t *audio_decoder = Int_decoder_Creat();

//     // 为语音识别系统创建一个输入环形缓冲区
//     RingbufHandle_t sr_ringbuffer = xRingbufferCreateWithCaps(64 * 1024, RINGBUF_TYPE_BYTEBUF,MALLOC_CAP_SPIRAM);
//     // 为语音识别系统设置输出缓冲区
//     Int_SR_Set_OutputBuffer(audio_sr, sr_ringbuffer);
//     // 为opus编码器设置输入缓冲区 (即语音识别系统的输出缓冲区)
//     Int_encoder_Set_InputBuffer(audio_encoder,sr_ringbuffer);

//     // 为opus编码器创建一个输出环形缓冲区
//     RingbufHandle_t encoder_ringbuffer = xRingbufferCreateWithCaps(64 * 1024,RINGBUF_TYPE_NOSPLIT,MALLOC_CAP_SPIRAM);
//     // 为opus编码器设置输出缓冲区
//     Int_encoder_Set_OutputBuffer(audio_encoder,encoder_ringbuffer);
//     // 为opus解码器设置输入缓冲区 (即opus编码器的输出缓冲区)
//     Int_decoder_Set_InputBuffer(audio_decoder,encoder_ringbuffer);

//     // 为opus解码器创建一个输出环形缓冲区
//     RingbufHandle_t decoder_ringbuffer = xRingbufferCreateWithCaps(64 * 1024,RINGBUF_TYPE_NOSPLIT,MALLOC_CAP_SPIRAM);
//     // 为opus解码器设置输出缓冲区
//     Int_decoder_Set_OutputBuffer(audio_decoder,decoder_ringbuffer);

//     // 设置唤醒后的回调函数
//     Int_SR_Set_WakeNet_cb(audio_sr, app_main_wakenet_cb, NULL);
//     // 设置语音识别的回调函数
//     Int_SR_Set_VAD_cb(audio_sr, app_main_vad_cb, NULL);

//     // 启动 opus解码器
//     Int_decoder_Start(audio_decoder);
//     // 启动 opus编码器
//     Int_encoder_Start(audio_encoder);
//     // 启动 语音识别系统
//     Int_SR_Start(audio_sr);

//     void *data = NULL;
//     size_t len = 0;

//     while (1)
//     {

//         // 从opus解码器的缓冲区读取数据
//         data = xRingbufferReceive(decoder_ringbuffer, &len, portMAX_DELAY);

//         // Int_ES8311_ReadFromMic(data, 512);
//         Int_ES8311_WriteToSpeak(data, len);

//         // 释放缓冲区资源
//         vRingbufferReturnItem(decoder_ringbuffer, data);
//         vTaskDelay(10);
//     }
// }