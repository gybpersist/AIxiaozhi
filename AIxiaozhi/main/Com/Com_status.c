#include "Com_status.h"

Com_Status_t Com_Status = Com_Status_IDLE;

char *Status_arr[5] = {
    "Com_Status_IDLE",       // ¿ÕÏĞ×´Ì¬
    "Com_Status_STRAT",      // ¿ªÊ¼×´Ì¬
    "Com_Status_CONNECTION", // Á¬½Ó×´Ì¬
    "Com_Status_LISTENING",  // ñöÌı×´Ì¬
    "Com_Status_SPEAKERING", // Ëµ»°×´Ì¬
};

/**
 * @brief ÇĞ»» ALĞ¡ÖÇµÄµ±Ç°×´Ì¬
 *
 * @param Status ÒªÇĞ»»µ½µÄ×´Ì¬
 */
void Com_Status_Switch(Com_Status_t Status)
{
    if (Com_Status != Status)
    {
        MY_LOGE("xiaozhi switch status %s ---> %s\r\n", Status_arr[Com_Status], Status_arr[Status]);
        Com_Status = Status;
    }
}
