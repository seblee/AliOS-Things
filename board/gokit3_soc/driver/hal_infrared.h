#ifndef _HAL_INFRARED_H
#define _HAL_INFRARED_H
#include <stdio.h>
#include "aos/kernel.h"
#include "aos/hal/gpio.h"

/* Define your drive pin */
#define Infrared_GPIO_PIN 16

/* Set GPIO Direction */
#define GET_INF GPIO_INPUT_GET(GPIO_ID_PIN(Infrared_GPIO_PIN))

/* Function declaration */
void irInit(void);
int irUpdateStatus(void);
void irSensorTest(void);
#endif /*_HAL_INFRARED_H*/
