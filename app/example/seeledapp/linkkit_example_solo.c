/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "aos/kernel.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "linkkit/infra/infra_types.h"
#include "linkkit/infra/infra_defs.h"
#include "linkkit/infra/infra_compat.h"
#include "linkkit/dev_model_api.h"
#include "linkkit/infra/infra_config.h"
#include "linkkit/wrappers/wrappers.h"

#ifdef INFRA_MEM_STATS
#include "linkkit/infra/infra_mem_stats.h"
#endif

#include "cJSON.h"
#ifdef ATM_ENABLED
#include "at_api.h"
#endif
#include "app_entry.h"
#include "app_json_parse.h"

// for demo only
#define PRODUCT_KEY "a1Njg3Y1xSy"
#define PRODUCT_SECRET "NyeaHJBWhKrRN9oG"
#define DEVICE_NAME "4D2n5LklSyDM7a8GRuUc"
#define DEVICE_SECRET "W6oNPjM6HCg2n9kvAJwOBtmbNvlqm3gX"

#define EXAMPLE_TRACE(...)                                      \
    do                                                          \
    {                                                           \
        HAL_Printf("\033[1;32;40m%s.%d: ", __func__, __LINE__); \
        HAL_Printf(__VA_ARGS__);                                \
        HAL_Printf("\033[0m\r\n");                              \
    } while (0)

#define EXAMPLE_MASTER_DEVID (0)
#define EXAMPLE_YIELD_TIMEOUT_MS (200)

typedef struct
{
    int master_devid;
    int cloud_connected;
    int master_initialized;
} user_example_ctx_t;

/**
 * These PRODUCT_KEY|PRODUCT_SECRET|DEVICE_NAME|DEVICE_SECRET are listed for demo only
 *
 * When you created your own devices on iot.console.com, you SHOULD replace them with what you got from console
 *
 */

static user_example_ctx_t g_user_example_ctx;

/** cloud connected event callback */
static int user_connected_event_handler(void)
{
    EXAMPLE_TRACE("Cloud Connected");
    g_user_example_ctx.cloud_connected = 1;

    return 0;
}

/** cloud disconnected event callback */
static int user_disconnected_event_handler(void)
{
    EXAMPLE_TRACE("Cloud Disconnected");
    g_user_example_ctx.cloud_connected = 0;

    return 0;
}

/* device initialized event callback */
static int user_initialized(const int devid)
{
    EXAMPLE_TRACE("Device Initialized");
    g_user_example_ctx.master_initialized = 1;

    return 0;
}

/** recv property post response message from cloud **/
static int user_report_reply_event_handler(const int devid, const int msgid, const int code, const char *reply,
                                           const int reply_len)
{
    EXAMPLE_TRACE("Message Post Reply Received, Message ID: %d, Code: %d, Reply: %.*s", msgid, code,
                  reply_len,
                  (reply == NULL) ? ("NULL") : (reply));
    return 0;
}

/** recv event post response message from cloud **/
static int user_trigger_event_reply_event_handler(const int devid, const int msgid, const int code, const char *eventid,
                                                  const int eventid_len, const char *message, const int message_len)
{
    EXAMPLE_TRACE("Trigger Event Reply Received, Message ID: %d, Code: %d, EventID: %.*s, Message: %.*s",
                  msgid, code,
                  eventid_len,
                  eventid, message_len, message);

    return 0;
}

/** recv event post response message from cloud **/
static int user_property_set_event_handler(const int devid, const char *request, const int request_len)
{
    int res = 0;
    EXAMPLE_TRACE("Property Set Received, Request: %s", request);
    property_set_json_parse(request);
    res = IOT_Linkkit_Report(EXAMPLE_MASTER_DEVID, ITM_MSG_POST_PROPERTY,
                             (unsigned char *)request, request_len);
    EXAMPLE_TRACE("Post Property Message ID: %d", res);

    return 0;
}

static int user_property_get_event_handler(const int devid, const char *request, const int request_len, char **response,
                                           int *response_len)
{
    cJSON *request_root = NULL, *item_propertyid = NULL;
    cJSON *response_root = NULL;
    int index = 0;
    EXAMPLE_TRACE("Property Get Received, Devid: %d, Request: %s", devid, request);

    /* Parse Request */
    request_root = cJSON_Parse(request);
    if (request_root == NULL || !cJSON_IsArray(request_root))
    {
        EXAMPLE_TRACE("JSON Parse Error");
        return -1;
    }

    /* Prepare Response */
    response_root = cJSON_CreateObject();
    if (response_root == NULL)
    {
        EXAMPLE_TRACE("No Enough Memory");
        cJSON_Delete(request_root);
        return -1;
    }

    for (index = 0; index < cJSON_GetArraySize(request_root); index++)
    {
        item_propertyid = cJSON_GetArrayItem(request_root, index);
        if (item_propertyid == NULL || !cJSON_IsString(item_propertyid))
        {
            EXAMPLE_TRACE("JSON Parse Error");
            cJSON_Delete(request_root);
            cJSON_Delete(response_root);
            return -1;
        }

        EXAMPLE_TRACE("Property ID, index: %d, Value: %s", index, item_propertyid->valuestring);

        if (strcmp("WIFI_Tx_Rate", item_propertyid->valuestring) == 0)
        {
            cJSON_AddNumberToObject(response_root, "WIFI_Tx_Rate", 1111);
        }
        else if (strcmp("WIFI_Rx_Rate", item_propertyid->valuestring) == 0)
        {
            cJSON_AddNumberToObject(response_root, "WIFI_Rx_Rate", 2222);
        }
        else if (strcmp("RGBColor", item_propertyid->valuestring) == 0)
        {
            cJSON *item_rgbcolor = cJSON_CreateObject();
            if (item_rgbcolor == NULL)
            {
                cJSON_Delete(request_root);
                cJSON_Delete(response_root);
                return -1;
            }
            cJSON_AddNumberToObject(item_rgbcolor, "Red", 100);
            cJSON_AddNumberToObject(item_rgbcolor, "Green", 100);
            cJSON_AddNumberToObject(item_rgbcolor, "Blue", 100);
            cJSON_AddItemToObject(response_root, "RGBColor", item_rgbcolor);
        }
        else if (strcmp("HSVColor", item_propertyid->valuestring) == 0)
        {
            cJSON *item_hsvcolor = cJSON_CreateObject();
            if (item_hsvcolor == NULL)
            {
                cJSON_Delete(request_root);
                cJSON_Delete(response_root);
                return -1;
            }
            cJSON_AddNumberToObject(item_hsvcolor, "Hue", 50);
            cJSON_AddNumberToObject(item_hsvcolor, "Saturation", 50);
            cJSON_AddNumberToObject(item_hsvcolor, "Value", 50);
            cJSON_AddItemToObject(response_root, "HSVColor", item_hsvcolor);
        }
        else if (strcmp("HSLColor", item_propertyid->valuestring) == 0)
        {
            cJSON *item_hslcolor = cJSON_CreateObject();
            if (item_hslcolor == NULL)
            {
                cJSON_Delete(request_root);
                cJSON_Delete(response_root);
                return -1;
            }
            cJSON_AddNumberToObject(item_hslcolor, "Hue", 70);
            cJSON_AddNumberToObject(item_hslcolor, "Saturation", 70);
            cJSON_AddNumberToObject(item_hslcolor, "Lightness", 70);
            cJSON_AddItemToObject(response_root, "HSLColor", item_hslcolor);
        }
        else if (strcmp("WorkMode", item_propertyid->valuestring) == 0)
        {
            cJSON_AddNumberToObject(response_root, "WorkMode", 4);
        }
        else if (strcmp("NightLightSwitch", item_propertyid->valuestring) == 0)
        {
            cJSON_AddNumberToObject(response_root, "NightLightSwitch", 1);
        }
        else if (strcmp("Brightness", item_propertyid->valuestring) == 0)
        {
            cJSON_AddNumberToObject(response_root, "Brightness", 30);
        }
        else if (strcmp("LightSwitch", item_propertyid->valuestring) == 0)
        {
            cJSON_AddNumberToObject(response_root, "LightSwitch", 1);
        }
        else if (strcmp("ColorTemperature", item_propertyid->valuestring) == 0)
        {
            cJSON_AddNumberToObject(response_root, "ColorTemperature", 2800);
        }
        else if (strcmp("PropertyCharacter", item_propertyid->valuestring) == 0)
        {
            cJSON_AddStringToObject(response_root, "PropertyCharacter", "testprop");
        }
        else if (strcmp("Propertypoint", item_propertyid->valuestring) == 0)
        {
            cJSON_AddNumberToObject(response_root, "Propertypoint", 50);
        }
        else if (strcmp("LocalTimer", item_propertyid->valuestring) == 0)
        {
            cJSON *array_localtimer = cJSON_CreateArray();
            if (array_localtimer == NULL)
            {
                cJSON_Delete(request_root);
                cJSON_Delete(response_root);
                return -1;
            }

            cJSON *item_localtimer = cJSON_CreateObject();
            if (item_localtimer == NULL)
            {
                cJSON_Delete(request_root);
                cJSON_Delete(response_root);
                cJSON_Delete(array_localtimer);
                return -1;
            }
            cJSON_AddStringToObject(item_localtimer, "Timer", "10 11 * * * 1 2 3 4 5");
            cJSON_AddNumberToObject(item_localtimer, "Enable", 1);
            cJSON_AddNumberToObject(item_localtimer, "IsValid", 1);
            cJSON_AddItemToArray(array_localtimer, item_localtimer);
            cJSON_AddItemToObject(response_root, "LocalTimer", array_localtimer);
        }
    }
    cJSON_Delete(request_root);

    *response = cJSON_PrintUnformatted(response_root);
    if (*response == NULL)
    {
        EXAMPLE_TRACE("No Enough Memory");
        cJSON_Delete(response_root);
        return -1;
    }
    cJSON_Delete(response_root);
    *response_len = strlen(*response);

    EXAMPLE_TRACE("Property Get Response: %s", *response);

    return SUCCESS_RETURN;
}

static int user_service_request_event_handler(const int devid, const char *serviceid, const int serviceid_len,
                                              const char *request, const int request_len,
                                              char **response, int *response_len)
{
    int add_result = 0;
    int ret = -1;
    cJSON *root = NULL, *item = NULL;
    EXAMPLE_TRACE("Service Request Received, Service ID: %.*s, Payload: %s", serviceid_len, serviceid, request);

    /* Parse Root */
    root = cJSON_Parse(request);
    if (root == NULL || !cJSON_IsObject(root))
    {
        EXAMPLE_TRACE("JSON Parse Error");
        return -1;
    }
    do
    {
        if (strlen("Custom") == serviceid_len && memcmp("Custom", serviceid, serviceid_len) == 0)
        {
            /* Parse Item */
            const char *response_fmt = "{\"Contrastratio\":%d}";
            item = cJSON_GetObjectItem(root, "transparency");
            if (item == NULL || !cJSON_IsNumber(item))
            {
                cJSON_Delete(root);
                return -1;
            }
            EXAMPLE_TRACE("transparency: %d", item->valueint);
            add_result = item->valueint + 1;

            /* Send Service Response To Cloud */
            *response_len = strlen(response_fmt) + 10 + 1;
            *response = (char *)HAL_Malloc(*response_len);
            if (*response == NULL)
            {
                EXAMPLE_TRACE("Memory Not Enough");
                return -1;
            }
            memset(*response, 0, *response_len);
            HAL_Snprintf(*response, *response_len, response_fmt, add_result);
            *response_len = strlen(*response);
        }
        else if (strlen("SyncService") == serviceid_len && memcmp("SyncService", serviceid, serviceid_len) == 0)
        {
            /* Parse Item */
            const char *response_fmt = "{\"ToCloud\":%d}";
            item = cJSON_GetObjectItem(root, "FromCloud");
            if (item == NULL || !cJSON_IsNumber(item))
            {
                cJSON_Delete(root);
                return -1;
            }
            EXAMPLE_TRACE("FromCloud: %d", item->valueint);
            add_result = item->valueint + 1;

            /* Send Service Response To Cloud */
            *response_len = strlen(response_fmt) + 10 + 1;
            *response = (char *)HAL_Malloc(*response_len);
            if (*response != NULL)
            {
                EXAMPLE_TRACE("Memory Not Enough");
                return -1;
            }
            memset(*response, 0, *response_len);
            HAL_Snprintf(*response, *response_len, response_fmt, add_result);
            *response_len = strlen(*response);
        }
    } while (0);

    cJSON_Delete(root);
    return 0;
}

static int user_timestamp_reply_event_handler(const char *timestamp)
{
    EXAMPLE_TRACE("Current Timestamp: %s", timestamp);

    return 0;
}

/** fota event handler **/
static int user_fota_event_handler(int type, const char *version)
{
    char buffer[128] = {0};
    int buffer_length = 128;

    /* 0 - new firmware exist, query the new firmware */
    if (type == 0)
    {
        EXAMPLE_TRACE("New Firmware Version: %s", version);

        IOT_Linkkit_Query(EXAMPLE_MASTER_DEVID, ITM_MSG_QUERY_FOTA_DATA, (unsigned char *)buffer, buffer_length);
    }

    return 0;
}

/* cota event handler */
static int user_cota_event_handler(int type, const char *config_id, int config_size, const char *get_type,
                                   const char *sign, const char *sign_method, const char *url)
{
    char buffer[128] = {0};
    int buffer_length = 128;

    /* type = 0, new config exist, query the new config */
    if (type == 0)
    {
        EXAMPLE_TRACE("New Config ID: %s", config_id);
        EXAMPLE_TRACE("New Config Size: %d", config_size);
        EXAMPLE_TRACE("New Config Type: %s", get_type);
        EXAMPLE_TRACE("New Config Sign: %s", sign);
        EXAMPLE_TRACE("New Config Sign Method: %s", sign_method);
        EXAMPLE_TRACE("New Config URL: %s", url);

        IOT_Linkkit_Query(EXAMPLE_MASTER_DEVID, ITM_MSG_QUERY_COTA_DATA, (unsigned char *)buffer, buffer_length);
    }

    return 0;
}

void user_post_property(void)
{
    int res = 0;
    char *property_payload = "NULL";
    uint8_t need_free = 1;

    res = property_json_build(&property_payload);
    if (res != 0)
    {
        property_payload = "hello world";
        need_free = 0;
    }

    res = IOT_Linkkit_Report(EXAMPLE_MASTER_DEVID, ITM_MSG_POST_PROPERTY,
                             (unsigned char *)property_payload, strlen(property_payload));
    if (need_free)
        aos_free(property_payload);
    EXAMPLE_TRACE("Post Property Message ID: %d", res);
}

void user_post_event(void)
{
    int res = 0;
    char *event_id = "HardwareError";
    char *event_payload = "NULL";
    uint8_t need_free = 0;

    int result = event_json_build(&event_payload);
    need_free = 1;
    if (result != 0)
    {
        event_payload = "{\"ErrorCode\":0}";
        need_free = 0;
    }

    res = IOT_Linkkit_TriggerEvent(EXAMPLE_MASTER_DEVID, event_id, strlen(event_id),
                                   event_payload, strlen(event_payload));
    if (need_free)
        aos_free(event_payload);
    EXAMPLE_TRACE("Post Event Message ID: %d", res);
}

void user_deviceinfo_update(void)
{
    int res = 0;
    char *device_info_update = "[{\"attrKey\":\"abc\",\"attrValue\":\"hello,world\"}]";

    res = IOT_Linkkit_Report(EXAMPLE_MASTER_DEVID, ITM_MSG_DEVICEINFO_UPDATE,
                             (unsigned char *)device_info_update, strlen(device_info_update));
    EXAMPLE_TRACE("Device Info Update Message ID: %d", res);
}

void user_deviceinfo_delete(void)
{
    int res = 0;
    char *device_info_delete = "[{\"attrKey\":\"abc\"}]";

    res = IOT_Linkkit_Report(EXAMPLE_MASTER_DEVID, ITM_MSG_DEVICEINFO_DELETE,
                             (unsigned char *)device_info_delete, strlen(device_info_delete));
    EXAMPLE_TRACE("Device Info Delete Message ID: %d", res);
}

static int user_cloud_error_handler(const int code, const char *data, const char *detail)
{
    EXAMPLE_TRACE("code =%d ,data=%s, detail=%s", code, data, detail);
    return 0;
}

void set_iotx_info()
{
    char _device_name[IOTX_DEVICE_NAME_LEN + 1] = {0};
    HAL_GetDeviceName(_device_name);
    if (strlen(_device_name) == 0)
    {
        HAL_SetProductKey(PRODUCT_KEY);
        HAL_SetProductSecret(PRODUCT_SECRET);
        HAL_SetDeviceName(DEVICE_NAME);
        HAL_SetDeviceSecret(DEVICE_SECRET);
    }
}

int linkkit_main(void *paras)
{
    int res = 0;
    int cnt = 0;
    int auto_quit = 0;
    iotx_linkkit_dev_meta_info_t master_meta_info;
    int domain_type = 0, dynamic_register = 0, post_reply_need = 0, fota_timeout = 30;
    int argc = 0;
    char **argv = NULL;

    if (paras != NULL)
    {
        argc = ((app_main_paras_t *)paras)->argc;
        argv = ((app_main_paras_t *)paras)->argv;
    }
#ifdef ATM_ENABLED
    if (IOT_ATM_Init() < 0)
    {
        EXAMPLE_TRACE("IOT ATM init failed!\n");
        return -1;
    }
#endif

    if (argc >= 2 && !strcmp("auto_quit", argv[1]))
    {
        auto_quit = 1;
        cnt = 0;
    }
    memset(&g_user_example_ctx, 0, sizeof(user_example_ctx_t));

    memset(&master_meta_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));
    HAL_GetProductKey(master_meta_info.product_key);
    HAL_GetDeviceName(master_meta_info.device_name);
    HAL_GetProductSecret(master_meta_info.product_secret);
    HAL_GetDeviceSecret(master_meta_info.device_secret);

    IOT_SetLogLevel(IOT_LOG_DEBUG);

    /* Register Callback */
    IOT_RegisterCallback(ITE_CONNECT_SUCC, user_connected_event_handler);
    IOT_RegisterCallback(ITE_DISCONNECTED, user_disconnected_event_handler);
    IOT_RegisterCallback(ITE_SERVICE_REQUEST, user_service_request_event_handler);
    IOT_RegisterCallback(ITE_PROPERTY_SET, user_property_set_event_handler);
    IOT_RegisterCallback(ITE_PROPERTY_GET, user_property_get_event_handler);
    IOT_RegisterCallback(ITE_REPORT_REPLY, user_report_reply_event_handler);
    IOT_RegisterCallback(ITE_TRIGGER_EVENT_REPLY, user_trigger_event_reply_event_handler);
    IOT_RegisterCallback(ITE_TIMESTAMP_REPLY, user_timestamp_reply_event_handler);
    IOT_RegisterCallback(ITE_INITIALIZE_COMPLETED, user_initialized);
    IOT_RegisterCallback(ITE_FOTA, user_fota_event_handler);
    IOT_RegisterCallback(ITE_COTA, user_cota_event_handler);
    IOT_RegisterCallback(ITE_CLOUD_ERROR, user_cloud_error_handler);

    domain_type = IOTX_CLOUD_REGION_SHANGHAI;
    IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);

    /* Choose Login Method */
    dynamic_register = 0;
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);

    /* post reply doesn't need */
    post_reply_need = 1;
    IOT_Ioctl(IOTX_IOCTL_RECV_EVENT_REPLY, (void *)&post_reply_need);

    IOT_Ioctl(IOTX_IOCTL_FOTA_TIMEOUT_MS, (void *)&fota_timeout);
#if defined(USE_ITLS)
    {
        char url[128] = {0};
        int port = 1883;
        snprintf(url, 128, "%s.itls.cn-shanghai.aliyuncs.com", master_meta_info.product_key);
        IOT_Ioctl(IOTX_IOCTL_SET_MQTT_DOMAIN, (void *)url);
        IOT_Ioctl(IOTX_IOCTL_SET_CUSTOMIZE_INFO, (void *)"authtype=id2");
        IOT_Ioctl(IOTX_IOCTL_SET_MQTT_PORT, &port);
    }
#endif
    /* Create Master Device Resources */
    do
    {
        g_user_example_ctx.master_devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &master_meta_info);
        if (g_user_example_ctx.master_devid >= 0)
        {
            break;
        }
        EXAMPLE_TRACE("IOT_Linkkit_Open failed! retry after %d ms\n", 2000);
        HAL_SleepMs(2000);
    } while (1);
    /* Start Connect Aliyun Server */
    do
    {
        res = IOT_Linkkit_Connect(g_user_example_ctx.master_devid);
        if (res >= 0)
        {
            break;
        }
        EXAMPLE_TRACE("IOT_Linkkit_Connect failed! retry after %d ms\n", 5000);
        HAL_SleepMs(5000);
    } while (1);

    while (1)
    {
        IOT_Linkkit_Yield(EXAMPLE_YIELD_TIMEOUT_MS);

        /* Post Proprety Example */

        if ((cnt % 20) == 0)
        {
            user_post_property();
        }

        /* Post Event Example */
        if ((cnt % 50) == 0)
        {
            user_post_event();
        }

        cnt++;

        if (auto_quit == 1 && cnt > 3600)
        {
            break;
        }
    }

    IOT_Linkkit_Close(g_user_example_ctx.master_devid);

    IOT_DumpMemoryStats(IOT_LOG_DEBUG);

    return 0;
}
