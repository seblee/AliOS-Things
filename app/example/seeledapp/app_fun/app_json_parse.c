#include "json_parser.h"
#include "cJSON.h"
#include "iot_export.h"
#include "iot_import.h"

#define EXAMPLE_TRACE(...)                                      \
    do                                                          \
    {                                                           \
        HAL_Printf("\033[1;32;40m%s.%d: ", __func__, __LINE__); \
        HAL_Printf(__VA_ARGS__);                                \
        HAL_Printf("\033[0m\r\n");                              \
    } while (0)

static int LightSwitch = 0;
static int Brightness = 0;
static int RGB_RED = 0;
static int RGB_GREEN = 0;
static int RGB_BLUE = 0;
static uint8_t EnvTemperature = 0;
static uint8_t EnvHumidity = 0;
static int16_t motor = 0;
static uint8_t EventCode = 0;
const char *EventStr[] = {
    "device normal",
    "TempHumError",
    "RGBError",
    "MotorError",
    "InfraredError",
};

/********
 * fun  :property_set_json_parse
 * para :json_src 
 * 
 */
int property_set_json_parse(const char *json_src)
{
    uint8_t Led_Ctr = 0;
    cJSON *root = NULL;
    cJSON *js_switch = NULL;
    root = cJSON_Parse(json_src);
    if (root == NULL || !cJSON_IsObject(root))
    {
        EXAMPLE_TRACE("JSON Parse Error");
        return -1;
    }
    js_switch = cJSON_GetObjectItem(root, "LightSwitch");
    if (js_switch)
    {
        Led_Ctr = 1;
        LightSwitch = js_switch->valueint;
        EXAMPLE_TRACE("LightSwitCtrl:%d", LightSwitch);
    }
    cJSON *js_RGB_RED = cJSON_GetObjectItem(root, "RGB_RED");
    if (js_RGB_RED)
    {
        Led_Ctr = 1;
        RGB_RED = js_RGB_RED->valueint;
    }
    cJSON *js_RGB_GREEN = cJSON_GetObjectItem(root, "RGB_GREEN");
    if (js_RGB_GREEN)
    {
        Led_Ctr = 1;
        RGB_GREEN = js_RGB_GREEN->valueint;
    }
    cJSON *js_RGB_BLUE = cJSON_GetObjectItem(root, "RGB_BLUE");
    if (js_RGB_BLUE)
    {
        Led_Ctr = 1;
        RGB_BLUE = js_RGB_BLUE->valueint;
    }
    if (Led_Ctr)
    {
        if (LightSwitch)
        {
            rgbControl(RGB_RED, RGB_GREEN, RGB_BLUE);
        }
        else
        {
            rgbControl(0, 0, 0);
        }
    }
    cJSON *js_motor = cJSON_GetObjectItem(root, "motor");
    if (js_motor)
    {
        motor = js_motor->valueint - 5;
        motorControl(motor);
    }
}

int property_json_build(char **json)
{
    cJSON *response_root = NULL;
    response_root = cJSON_CreateObject();
    if (response_root == NULL || !cJSON_IsObject(response_root))
    {
        EXAMPLE_TRACE("JSON Create Error");
        return -1;
    }
    cJSON_AddNumberToObject(response_root, "LightSwitch", LightSwitch);
    cJSON_AddNumberToObject(response_root, "Brightness", Brightness);
    cJSON_AddNumberToObject(response_root, "RGB_RED", RGB_RED);
    cJSON_AddNumberToObject(response_root, "RGB_GREEN", RGB_GREEN);
    cJSON_AddNumberToObject(response_root, "RGB_BLUE", RGB_BLUE);
    cJSON_AddNumberToObject(response_root, "Infrared", irUpdateStatus());
    dht11ReadData(&EnvTemperature, &EnvHumidity);
    cJSON_AddNumberToObject(response_root, "EnvTemperature", EnvTemperature);
    cJSON_AddNumberToObject(response_root, "EnvHumidity", EnvHumidity);
    *json = cJSON_PrintUnformatted(response_root);
    cJSON_Delete(response_root);
    if (*json != NULL)
        EXAMPLE_TRACE("JSON len:%d,JSON:%s", strlen(*json), *json);
    return 0;
}

int event_json_build(char **json)
{
    cJSON *response_root = NULL;
    response_root = cJSON_CreateObject();
    if (response_root == NULL || !cJSON_IsObject(response_root))
    {
        EXAMPLE_TRACE("JSON Create Error");
        return -1;
    }
    cJSON_AddNumberToObject(response_root, "ErrorCode", EventCode);
    cJSON_AddStringToObject(response_root, "ErrorName", EventStr[EventCode]);

    *json = cJSON_PrintUnformatted(response_root);
    cJSON_Delete(response_root);
    if (*json != NULL)
        EXAMPLE_TRACE("JSON len:%d,JSON:%s", strlen(*json), *json);
    EventCode++;
    if (EventCode == 5)
        EventCode = 0;
    return 0;
}
