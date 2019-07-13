﻿
/*********************************************************
*
* @file      hal_motor.c
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

#include "hal_motor.h"
#include "pwm.h"
#include "esp8266/eagle_soc.h"
#include "esp8266/pin_mux_register.h"

struct pwm_param motorParam = {0};

/******************************************************************************
 * FunctionName : user_light_set_duty
 * Description  : set each channel's duty params
 * Parameters   : uint8 duty    : duty 的范围随 PWM 周期改变， 
 *                              最大值为： Period * 1000 /45 。
 *                              例如， 1KHz PWM， duty 范围是： 0 ~ 22222
 *                uint8 channel : MOTOR_A/MOTOR_B
 * Returns      : NONE
*******************************************************************************/
void motorPwmControl(uint8_t m0, uint8_t m1)
{
    uint32 temp_m0 = 0;
    uint32 temp_m1 = 0;

    temp_m0 = (uint32)((m0 * 1.0 / (MOTOR_SFCT_STA)) * (MOTOR_MAX_DUTY));
    temp_m1 = (uint32)((m1 * 1.0 / (MOTOR_SFCT_STA)) * (MOTOR_MAX_DUTY));

    if (temp_m0 != motorParam.duty[CHANNEL_0])
    {
        pwm_set_duty(temp_m0, CHANNEL_0);
        motorParam.duty[CHANNEL_0] = pwm_get_duty(CHANNEL_0);
    }

    if (temp_m1 != motorParam.duty[CHANNEL_1])
    {
        pwm_set_duty(temp_m1, CHANNEL_1);
        motorParam.duty[CHANNEL_1] = pwm_get_duty(CHANNEL_1);
    }

    pwm_start();
}

void motorControl(_MOTOR_T status)
{
    if ((-5 > status) || (5 < status))
    {
        printf("Motor_status Error : [%d] \r\n", status);
    }

    if (status == 0)
    {
        motorPwmControl(0, 0);
    }
    else if (status > 0)
    {
        motorPwmControl(MOTOR_MIN_STA, (uint8_t)status);
    }
    else if (status < 0)
    {
        motorPwmControl(MOTOR_SFCT_STA, (uint8_t)(MOTOR_SFCT_STA - abs(status)));
    }
}

void motorInit(void)
{
    /* Migrate your driver code */
    LOG("rgbLedInit start");
    motorParam.period = MOTOR_PERIOD;

    uint32 io_info[][3] =
        {
            {PWM_0_OUT_IO_MUX, PWM_0_OUT_IO_FUNC, PWM_0_OUT_IO_NUM},
            {PWM_1_OUT_IO_MUX, PWM_1_OUT_IO_FUNC, PWM_1_OUT_IO_NUM},
        };

    uint32 pwm_duty_init[PWM_CHANNEL] = {0};

    /*PIN FUNCTION INIT FOR PWM OUTPUT*/
    pwm_init(motorParam.period, pwm_duty_init, PWM_CHANNEL, io_info);

    return;
}

void motorSensorTest(uint8_t Mocou)
{
    /* Test LOG model */

    motorControl(Mocou);
}
