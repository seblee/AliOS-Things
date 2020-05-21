/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ulog/ulog.h"

#include "network/hal/base.h"
#include <hal/wifi.h>
#include "at/at.h"

#define TAG "wifi_port"

#define AT_RSP_SUCCESS "OK"
#define AT_RSP_FAIL "ERROR"

extern int at_dev_fd;
static int get_mac_helper(char *mac);
static int get_ip_stat_helper(hal_wifi_ip_stat_t *result);

static void fetch_ip_stat(void *arg)
{
    hal_wifi_ip_stat_t ip_stat = {0};
    hal_wifi_module_t *m;

    if (!arg) {
        LOGE(TAG, "%s failed, invalid argument\r\n", __func__);
        return;
    }

    m = (hal_wifi_module_t *)arg;

    get_ip_stat_helper(&ip_stat);

    if (m->ev_cb->ip_got != NULL) {
        m->ev_cb->ip_got(m, &ip_stat, NULL);
    }
}

void at_wevent_handler(void *arg, char *buf, int buflen)
{
    hal_wifi_module_t *m;

    if (NULL == arg){
        m = hal_wifi_get_default_module();
    } else {
        m = (hal_wifi_module_t *)arg;
    }

    if (NULL == m) {
        return;
    }

    if (NULL == m->ev_cb) {
        return;
    }

    if (m->ev_cb->stat_chg != NULL) {
        m->ev_cb->stat_chg(m, NOTIFY_STATION_UP, NULL);
    }

    fetch_ip_stat(m);
}

static int wifi_init(hal_wifi_module_t *m)
{
    LOGI(TAG, "wifi init success!!\n");
    return 0;
};

#define MAC_STR_LEN 12
static void mac_str_to_hex(char *str, uint8_t *mac)
{
    int i, k;
    char c;
    unsigned int j;

    if (!str || !mac) return;

    memset(mac, 0, MAC_STR_LEN >> 1);
    for (k = 0, i = 0; k < strlen(str); k++) {
        c = str[k];
        if (c == ':')
           continue;

        if (c >= '0' && c <= '9') j = c - '0';
        else if (c >= 'A' && c <= 'F') j = c - 'A' + 0xa;
        else if (c >= 'a' && c <= 'f') j = c - 'a' + 0xa;
        else j = c % 0xf;
        j <<= i & 1 ? 0 : 4;
        mac[i>>1] |= j;

        i++;
    }
}

static void wifi_get_mac_addr(hal_wifi_module_t *m, uint8_t *mac)
{
    static char mac_str[MAC_STR_LEN+1] = {0};

    if (!mac) return;

    (void)m;
    LOGD(TAG, "wifi_get_mac_addr!!\n");

    if (strlen(mac_str) == 0)
        get_mac_helper(mac_str);

    mac_str_to_hex(mac_str, mac);
    LOGI(TAG, "mac in hex: %02x%02x%02x%02x%02x%02x",
      mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
};

#define AT_CMD_CONNECT_AP "AT+WJAP"
static int wifi_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
{
    char in[128] = {0};
    char out[128] = {0};

    (void)init_para;

    if (strcmp(init_para->wifi_key, "open") == 0) {
        snprintf(in, sizeof(in), AT_CMD_CONNECT_AP"=%s",
          init_para->wifi_ssid);
    } else {
        snprintf(in, sizeof(in), AT_CMD_CONNECT_AP"=%s,%s",
          init_para->wifi_ssid, init_para->wifi_key);
    }

    LOGI(TAG, "Will connect via at cmd: %s\r\n", in);

    if (at_send_wait_reply(at_dev_fd, in, strlen(in), true, NULL, 0, out, sizeof(out), NULL) == 0)
        LOGI(TAG, "AT command %s succeed, rsp: %s\r\n", in, out);
    else
        LOGE(TAG, "AT command %s failed\r\n", in);

    if (strstr(out, AT_RSP_FAIL)) {
        LOGE(TAG, "Connect wifi failed\r\n");
        return -1;
    }

    return 0;
}

static int wifi_start_adv(hal_wifi_module_t *m, hal_wifi_init_type_adv_t *init_para_adv)
{
    (void)init_para_adv;

    return 0;
}

#define AT_CMD_OBTAIN_MAC "AT+WMAC?"
#define AT_RSP_MAC_PREFIX "+WMAC"
/* mac string, e.g. "BF01ADE2F5CE" */
static int get_mac_helper(char *mac)
{
    char out[128] = {0};
    char *at_mac_str = AT_CMD_OBTAIN_MAC;

    if (!mac) return -1;

    if (at_send_wait_reply(at_dev_fd, at_mac_str, strlen(at_mac_str), true,
                           NULL, 0, out, sizeof(out), NULL) == 0) {
        LOGI(TAG, "AT command %s succeed, rsp: %s", AT_CMD_OBTAIN_MAC, out);
    } else {
        LOGE(TAG, "AT command %s failed\r\n", AT_CMD_OBTAIN_MAC);
        return -1;
    }

    if (strstr(out, AT_RSP_FAIL) || !strstr(out, AT_RSP_MAC_PREFIX)) {
        LOGE(TAG, "Command %s executed with ERROR.", AT_CMD_OBTAIN_MAC);
        return -1;
    }

    sscanf(out, "%*[^:]:%[^\r]", mac);
    LOGI(TAG, "mac result: %s\r\n", mac);

    return 0;
}

#define AT_CMD_OBTAIN_IP "AT+WJAPIP?"
#define AT_RSP_IP_PREFIX "+WJAPIP"
static int get_ip_stat_helper(hal_wifi_ip_stat_t *result)
{
    char out[128] = {0};
    int ret = 0;
    char *at_ip_str = AT_CMD_OBTAIN_IP;

    if (!result) return -1;

    if (at_send_wait_reply(at_dev_fd, at_ip_str, strlen(at_ip_str), true,
                           NULL, 0, out, sizeof(out), NULL) == 0) {
        LOGI(TAG, "AT command %s succeed, rsp: %s", AT_CMD_OBTAIN_IP, out);
    } else {
        LOGE(TAG, "AT command %s failed\r\n", AT_CMD_OBTAIN_IP);
        return -1;
    }

    if (strstr(out, AT_RSP_FAIL) || !strstr(out, AT_RSP_IP_PREFIX)) {
        LOGE(TAG, "Command  %s executed with ERROR", AT_CMD_OBTAIN_IP);
        return -1;
    }

    sscanf(out, "%*[^:]:%[^,],%[^,],%[^,],%[^\r]",
      result->ip, result->mask, result->gate, result->dns);

    LOGD(TAG, "result: %s %s %s %s\r\n",
      result->ip, result->mask, result->gate, result->dns);

    ret = get_mac_helper(result->mac);

    return ret;
}

static int get_ip_stat(hal_wifi_module_t *m, hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type)
{
    (void)wifi_type;
    (void)m;

    get_ip_stat_helper(out_net_para);

    return 0;
}

hal_wifi_module_t aos_wifi_module_mk3060 = {
    .base.name           = "aos_wifi_module_mk3060",
    .init                =  wifi_init,
    .get_mac_addr        =  wifi_get_mac_addr,
    .start               =  wifi_start,
    .start_adv           =  wifi_start_adv,
    .get_ip_stat         =  get_ip_stat,
};
