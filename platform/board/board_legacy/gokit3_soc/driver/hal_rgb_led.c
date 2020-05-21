
/*********************************************************
*
* @file      hal_rgb_led.c
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

#include "hal_rgb_led.h"
#ifdef GOKIT3_BSP_RGBLED
static void rgbDelay(unsigned int us)
{
    /* Define your delay function */
    volatile unsigned int i = 0;
    // long long now = aos_now();

    // while (aos_now() < now + i * 1000)
    // {
    //     /* code */
    // }
    for (i = 0; i < us; i++)
        ;
}

/************ generation clock *********************/
static void clkProduce(void)
{
    SCL_LOW; // SCL=0
    rgbDelay(40);
    SCL_HIGH; // SCL=1
    rgbDelay(40);
}

/**********  send 32 zero ********************/
static void send_32zero(void)
{
    uint8_t i;

    SDA_LOW; // SDA=0
    for (i = 0; i < 32; i++)
    {
        clkProduce();
    }
}

/********* invert the grey value of the first two bits ***************/
static uint8_t takeAntiCode(uint8_t dat)
{
    uint8_t tmp = 0;

    tmp = ((~dat) & 0xC0) >> 6;
    return tmp;
}

/****** send gray data *********/
static void dataSend(uint32_t dx)
{
    uint8_t i;

    for (i = 0; i < 32; i++)
    {
        if ((dx & 0x80000000) != 0)
        {

            SDA_HIGH; //  SDA=1;
        }
        else
        {
            SDA_LOW; //  SDA=0;
        }

        dx <<= 1;
        clkProduce();
    }
}

/******* data processing  ********************/
static void dataDealwithSend(uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t dx = 0;

    dx |= (uint32_t)0x03 << 30; // The front of the two bits 1 is flag bits
    dx |= (uint32_t)takeAntiCode(b) << 28;
    dx |= (uint32_t)takeAntiCode(g) << 26;
    dx |= (uint32_t)takeAntiCode(r) << 24;

    dx |= (uint32_t)b << 16;
    dx |= (uint32_t)g << 8;
    dx |= r;

    dataSend(dx);
}

void rgbControl(uint8_t R, uint8_t G, uint8_t B)
{
    //contron power

    send_32zero();
    dataDealwithSend(R, G, B); // display red
    dataDealwithSend(R, G, B); // display red
}

void rgbLedInit(void)
{
    //contron power

    send_32zero();
    dataDealwithSend(0, 0, 0); // display red
    dataDealwithSend(0, 0, 0);
}
#include "ulog/ulog.h"
void rgbGpioInit(void)
{
    /* Migrate your driver code */
    gpio_dev_t gpio_dev1 = {GPIO_RGB_SCL, OUTPUT_PUSH_PULL, NULL};
    gpio_dev_t gpio_dev2 = {GPIO_RGB_SDA, OUTPUT_PUSH_PULL, NULL};

    hal_gpio_init(&gpio_dev1);

    hal_gpio_init(&gpio_dev2);

    printf("rgbGpioInit \r\n");
}

void rgbSensorTest(uint8_t rgbcou)
{
    /* Test LOG model */

    if (0 == rgbcou)
    {
        rgbControl(0, 0, 250);
        //      GAgent_Printf(GAGENT_CRITICAL, "RGB : B");
    }
    else if (1 == rgbcou)
    {
        rgbControl(0, 250, 0);
        //      GAgent_Printf(GAGENT_CRITICAL, "RGB : G");
    }
    else if (2 == rgbcou)
    {
        rgbControl(250, 0, 0);
        //      GAgent_Printf(GAGENT_CRITICAL, "RGB : R");
    }
}
#endif
