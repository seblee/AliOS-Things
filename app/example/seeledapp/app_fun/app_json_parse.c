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

/********
 * fun  :property_set_json_parse
 * para :json_src 
 * 
 */
int property_set_json_parse(const char *json_src)
{
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
        LightSwitch = js_switch->valueint;

        if (LightSwitch)
        {
            rgbControl(10, 0, 0);
        }
        else
        {
            rgbControl(0, 0, 0);
        }
        EXAMPLE_TRACE("LightSwitCtrl:%d", LightSwitch);
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
}
