﻿#ifndef _HAL_HEMP_HUM_H
#define _HAL_HEMP_HUM_H

#include "aos/kernel.h"
#include "aos/hal/gpio.h"

/* Define your drive pin */
#define DHT11_GPIO_PIN 5

/* Set GPIO Direction */
#define DHT11_IO_IN GPIO_DIS_OUTPUT(GPIO_ID_PIN(DHT11_GPIO_PIN)) // gpio_output_set(0, 0, 0, GPIO_ID_PIN(DHT11_GPIO_PIN))不可用

#define DHT11_IO_OUT gpio_output_set(0, 0, GPIO_ID_PIN(DHT11_GPIO_PIN), 0)

#define DHT11_OUT_HIGH GPIO_OUTPUT_SET(GPIO_ID_PIN(DHT11_GPIO_PIN), 1)
#define DHT11_OUT_LOW GPIO_OUTPUT_SET(GPIO_ID_PIN(DHT11_GPIO_PIN), 0)

#define DHT11_IN GPIO_INPUT_GET(GPIO_ID_PIN(DHT11_GPIO_PIN))

/* Set GPIO Direction */

#define DHT11_IO_IN                 \
    {                               \
        gpio_dev_t dev;             \
        dev.port = DHT11_GPIO_PIN;  \
        dev.config = INPUT_PULL_UP; \
        hal_gpio_init(&dev);        \
    }

#define DHT11_IO_OUT                \
    {                               \
        gpio_dev_t dev;             \
        dev.port = DHT11_GPIO_PIN;  \
        dev.config = INPUT_PULL_UP; \
        hal_gpio_init(&dev);        \
    }

#define DHT11_OUT_HIGH              \
    {                               \
        gpio_dev_t dev;             \
        dev.port = DHT11_GPIO_PIN;  \
        hal_gpio_output_high(&dev); \
    }
#define DHT11_OUT_LOW              \
    {                              \
        gpio_dev_t dev;            \
        dev.port = DHT11_GPIO_PIN; \
        hal_gpio_output_low(&dev); \
    }
#define DHT11_IN DHT11_Read(DHT11_GPIO_PIN)

#define MEAN_NUM 10

typedef struct
{
    uint8_t th_num;
    uint8_t th_amount;
    uint8_t th_bufs[10][2];
} th_typedef_t;

/* Function declaration */
uint8_t dh11Read(uint8_t *temperature, uint8_t *humidity);
uint8_t dh11Init(void); //Init DHT11
void dh11SensorTest(void);

#endif /*_HAL_HEMP_HUM_H*/
