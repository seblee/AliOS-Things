#ifndef _HAL_RGB_LED_H
#define _HAL_RGB_LED_H

#include "aos/kernel.h"
#include "aos/hal/gpio.h"

/* Define your drive pin */

#define GPIO_RGB_SCL 15
#define GPIO_RGB_SDA 4

//#define GPIO_RGB_POW 15

/* Set GPIO Direction */

#define SCL_LOW                    \
    {                              \
        gpio_dev_t dev;            \
        dev.port = GPIO_RGB_SCL;   \
        hal_gpio_output_low(&dev); \
    }
#define SCL_HIGH                    \
    {                               \
        gpio_dev_t dev;             \
        dev.port = GPIO_RGB_SCL;    \
        hal_gpio_output_high(&dev); \
    }

#define SDA_LOW                    \
    {                              \
        gpio_dev_t dev;            \
        dev.port = GPIO_RGB_SDA;   \
        hal_gpio_output_low(&dev); \
    }
#define SDA_HIGH                    \
    {                               \
        gpio_dev_t dev;             \
        dev.port = GPIO_RGB_SDA;    \
        hal_gpio_output_high(&dev); \
    }

//#define POW_HIGH GPIO_OUTPUT_SET(GPIO_ID_PIN(GPIO_RGB_POW), 1)

#define R_MAX 255
#define G_MAX 255
#define B_MAX 255

/* Function declaration */
void rgbControl(uint8_t R, uint8_t B, uint8_t G);
void rgbLedInit(void);
void rgbGpioInit(void);
void rgbSensorTest(uint8_t rgbcou);

#endif /*_HAL_RGB_LED_H*/
