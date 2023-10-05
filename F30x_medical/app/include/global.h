/*!
    \file    global.c
    \brief   FOC demo

    \version 2019-07-15, V1.0.0, FOC demo
    \version 2020-12-15, V1.1.0, FOC demo
    \version 2021-02-01, V1.2.0, FOC demo
    \version 2021-08-16, V1.3.0, FOC demo
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#ifndef GLOBAL_H
#define GLOBAL_H

#include "gd32f30x.h"
#include "math.h"
#include "arm_math.h"

#include "hard_ware.h"
#include "debug.h"
#include "systick.h"
#include "key.h"
#include "led.h"
#include "oled.h"
#include "flash.h"
#include "app_exti.h"
#include "usart2.h"

#include "type.h"
#include "timeflag.h"
#include "utils.h"

/* system parameters */
#define SYSTEM_FREQUENCE                    (120000000U)                        /* system frequence */
#define PWM_FREQUENCE                       (16000U)
#define PWM_COUNT                           (SYSTEM_FREQUENCE/PWM_FREQUENCE/2)  /* PWM frequence */
#define TS_NS                               (1000000000/PWM_FREQUENCE)          /* sampling time in nanoseconds */
#define TS_S                                (1.0f/PWM_FREQUENCE)                /* sampling time in seconds */


/* motor parameter check */
#define INPUT_VOLTAGE_MIN                   (16U)                               /* the minimum value of bus voltage in volt */
#define INPUT_VOLTAGE_MAX                   (32U)                               /* the maximum value of bus voltage in volt */
//#define TEMPERATURE_MIN                     (-5)                                /* the minimum value of mosfet temperature in centigrade */
//#define TEMPERATURE_MAX                     (80U)                               /* the maximum value of mosfet temperature in centigrade */


/* ADC sample time */
#define ADC_SAMPLE_TIME                     (ADC_SAMPLETIME_41POINT5)            /* ADC sampling time configuration */
/* conditioning circuit parameter */
#define CURRENT_AMP_GAIN                    (1.0f)                              /* the gain of amplifier in the current conditioning circuit */
#define CURRENT_AMP_SHIFT                   (1.65f)                             /* the bias voltage in the current sampling circuit */
#define CURRENT_SHUNT_RES                   (0.1f)                              /* the sampling resistance in the current sampling circuit */


/* acquire the modulation degree, according to the sampling time */
#if (defined (CSVPWM) || defined(CBSVPWM))
    #define MODULATION_DEGREE           (0.860f - ((float)TMIN_NS*2)/TS_NS)
#elif defined DSVPWMMIN
    #define MODULATION_DEGREE           (0.860f - ((float)TMIN_NS)/TS_NS)
#endif /* CSVPWM & DSVPWMMIN */


/* pid parameter configuration */
/* HALL SPEED MODE pid parameter */
#define SPEED_LOOP_KP                       (400U)                              /* the proportional paramter of speed regulator in hall mode */
#define SPEED_LOOP_KI                       (20U)                               /* the integral paramter of speed regulator in hall mode */


/* debug function */
/* debug buffer size configuration, 12 bytes per frame */
#define DEBUG_BUFFER_SIZE                   (600U)
/* usart configuration */
#define USART_DEBUG_ENABLE
#define DEBUG_USART                         USART1
#define DEBUG_USART_CLOCK                   RCU_USART1
#define DEBUG_USART_RX_PORT                 GPIOA
#define DEBUG_USART_RX_PIN                  GPIO_PIN_3
#define DEBUG_USART_TX_PORT                 GPIOA
#define DEBUG_USART_TX_PIN                  GPIO_PIN_2


#define COM_USART                           USART2
#define COM_USART_CLOCK                     RCU_USART2
#define COM_USART_RX_PORT                   GPIOC
#define COM_USART_RX_PIN                    GPIO_PIN_11
#define COM_USART_TX_PORT                   GPIOC
#define COM_USART_TX_PIN                    GPIO_PIN_10


/* constants */
#define ONE_BY_SQRT3                        (0.577350f)                         /* the value of 1/sqrt(3) */
#define TWO_BY_SQRT3                        (2.0f * 0.577350f)                  /* the value of 2/sqrt(3) */
#define SQRT3_BY_2                          (0.866025f)                         /* the value of sqrt(3)/2 */
#define M_PI                                (3.141592f)                         /* the value of pi */


/* 发电开关 */
#define GENERATE_SWITCH_PIN                 GPIO_PIN_7
#define GENERATE_SWITCH_GPIO_CLK            RCU_GPIOC
#define GENERATE_SWITCH_GPIO_PORT           GPIOC

/* CAN pin configuration */
#define CAN_RX_PORT                         GPIOA
#define CAN_RX_PIN                          GPIO_PIN_11
#define CAN_TX_PORT                         GPIOA
#define CAN_TX_PIN                          GPIO_PIN_12


/* adc channel configuration */
#define TORQUE_SENSOR_PIN                   GPIO_PIN_0
#define TORQUE_SENSOR_PORT                  GPIOA
#define TORQUE_SENSOR_CHANNEL               ADC_CHANNEL_0

#define VBUS_PIN                            GPIO_PIN_1
#define VBUS_PORT                           GPIOA
#define VBUS_CHANNEL                        ADC_CHANNEL_1

#define POSITION_PIN                        GPIO_PIN_0
#define POSITION_PORT                       GPIOC
#define POSITION_CHANNEL                    ADC_CHANNEL_10

#define I_BUS_PIN                           GPIO_PIN_1
#define I_BUS_PORT                          GPIOB
#define I_BUS_CHANNEL                       ADC_CHANNEL_9

/*
#define I_V_PIN                             GPIO_PIN_1
#define I_V_PORT                            GPIOC
#define I_V_CHANNEL                         ADC_CHANNEL_11

#define TEMPERATURE_PIN                     GPIO_PIN_5
#define TEMPERATURE_PORT                    GPIOA
#define TEMPERATURE_CHANNEL                 ADC_CHANNEL_5
*/

/* conflicting configuration */

#if ( (defined(HALL_FIX)) && (defined(HALL_DETECT)) )
#error "Invalid configuration: HALL_FIX and HALL_DETECT"
#endif /* HALL_FIX & HALL_DETECT */

#if ( (defined(CSVPWM)) && (defined(DSVPWMMIN)))
#error "Invalid configuration: CSVPWM and DSVPWMMIN"
#endif /* CSVPWM & DSVPWMMIN */

#if ( (defined(CSVPWM)) && (defined(CBSVPWM)))
#error "Invalid configuration: CSVPWM and CBSVPWM"
#endif /* CSVPWM & CBSVPWM */

#if ( (defined(DSVPWMMIN)) && (defined(CBSVPWM)))
#error "Invalid configuration: DSVPWMMIN and CBSVPWM"
#endif /* DSVPWMMIN & CBSVPWM */

typedef enum {
    GENERATOR_0 = 0,
    GENERATOR_1,
    GENERATOR_2,
    GENERATOR_3,
    GENERATOR_4,
    GENERATOR_5
} Generator_e;

typedef enum {
    GEAR_0 = 0,
    GEAR_1,
    GEAR_2,
    GEAR_3,
    GEAR_4,
    GEAR_5
} Gear_e;


/* global variable declaration */
/* reference variable */
//extern uint16_t adc_reference;
extern uint16_t adc_reference_filtered;
//extern uint16_t speed_reference;
extern int16_t torque_reference;

/* global structure */
extern motor_state motor;
extern mc_flags motor_flag;
extern pid_parameter speed, torque, flux, flux_weaken;
extern flash_page_type page_type;

/* hall variables */
extern hall_structure hall;

/* dma buffer */
extern uint16_t timer_update_buffer[6];
extern uint16_t adc_buffer[2];

/* phase current */
extern int16_t current_a, current_b, current_c;
/* the reload value of timer */
extern uint16_t pwm_top;
/* the frequency of PWM */
extern uint16_t frequency_now;
/* the angle of open-loop control */
extern float phase_sin,phase_cos;
extern int16_t uphase_sin,uphase_cos;

/* debug varieble */
extern usart_debug debug_data;

extern Generator_e g_generator_gears,g_generator_gears_pre;
extern Gear_e g_drive_mode;
extern uint16_t g_generator_power, g_error_code;

#endif /* GLOBAL_H */
