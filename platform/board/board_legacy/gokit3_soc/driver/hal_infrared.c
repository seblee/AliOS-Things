
/*********************************************************
*
* @file      hal_infrared.c
* @author    Gizwtis
* @version   V3.0
* @date      2016-03-09
*
* @brief     机智云 只为智能硬件而生
*            Gizwits Smart Cloud  for Smart Products
*            链接|增值|开放|中立|安全|自有|自由|生态
*            www.gizwits.com
*
*********************************************************/

#include "hal_infrared.h"
#include "ulog/ulog.h"
gpio_dev_t Infrared_gpio = {Infrared_GPIO_PIN, INPUT_PULL_UP, NULL};

int irUpdateStatus(void)
{
    int32_t value;
    hal_gpio_input_get(&Infrared_gpio, &value);
    if (value)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
void irInit(void)
{
    int32_t result;
    /* Migrate your driver code */
    printf("irInit \r\n");

    result = hal_gpio_init(&Infrared_gpio);
    printf("infrared_init result:%d \r\n", result);

    // gpio16InputConf();
}

void irSensorTest(void)
{
    /* Test LOG model */

    printf("InfIO : %d", irUpdateStatus());
}
