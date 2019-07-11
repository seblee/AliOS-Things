
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
int irUpdateStatus(void)
{
    if (gpio16InputGet())
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
static void gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    uint32_t value = 0;
    gpio_dev_t gpio_dev = {0};

    gpio_dev.port = Infrared_GPIO_PIN;
    hal_gpio_input_get(&gpio_dev, &value);
    LOG("GPIO[%u] intr, val: %u\n", gpio_num, value);
}

void infrared_init(void)
{
    gpio_dev_t gpio_dev = {0};
    gpio_dev.config = IRQ_MODE;
    gpio_dev.port = Infrared_GPIO_PIN;
    hal_gpio_init(&gpio_dev);

    /* gpio interrupt config */
    hal_gpio_enable_irq(&gpio_dev, IRQ_TRIGGER_BOTH_EDGES, gpio_isr_handler, (void *)Infrared_GPIO_PIN);
}

void irInit(void)
{
    /* Migrate your driver code */

    gpio16InputConf();

    os_printf("irInit \r\n");
}

void irSensorTest(void)
{
    /* Test LOG model */

    os_printf("InfIO : %d", irUpdateStatus());
}
