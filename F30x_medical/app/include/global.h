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
#include "lcd.h"
#include "oled.h"
#include "flash.h"
#include "app_exti.h"
#include "usart2.h"
#include "lcd_ctrl.h"

#include "type.h"
#include "timeflag.h"
#include "utils.h"

/* system parameters */
#define SYSTEM_FREQUENCE                    (120000000U)                        /* system frequence */
#define PWM_FREQUENCE                       (16000U)
#define PWM_COUNT                           (SYSTEM_FREQUENCE/PWM_FREQUENCE/2)  /* PWM frequence */


/* motor parameter check */
#define TEMPERATURE_PWM_MIN                 (0)                                 /* the minimum value of mosfet temperature in centigrade */
#define TEMPERATURE_PWM_MAX                 (100U)                              /* the maximum value of mosfet temperature in centigrade */

#define STEP_MOTOR_STEPS                    (0xFFFF)
#define QIBEI_TIMEROUT_TIMER                (5*60*1000)                        /* 弃杯超时计时时间 1分钟/15分钟*/

/* ADC sample time */
#define ADC_SAMPLE_TIME                     (ADC_SAMPLETIME_41POINT5)           /* ADC sampling time configuration */


/* pid parameter configuration */
/* HALL SPEED MODE pid parameter */
#define SPEED_LOOP_KP                       (400U)                              /* the proportional paramter of speed regulator in hall mode */
#define SPEED_LOOP_KI                       (20U)                               /* the integral paramter of speed regulator in hall mode */


#define Rp                                  (10)                                 //Rp上拉电阻值
#define T2                                  (273.15+25)                          //T2为25摄氏度，折算为开尔文温度
#define Ka                                  (273.15)

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

/* 蜂鸣器驱动信号 */
#define BEEP_PIN                            GPIO_PIN_6
#define BEEP_GPIO_CLK                       RCU_GPIOB
#define BEEP_GPIO_PORT                      GPIOB


//////落杯模块//////
/* 纸杯杯托位置检测开关信号 */
#define SWITCH_YOUBEI_PIN                   GPIO_PIN_7
#define SWITCH_YOUBEI_GPIO_CLK              RCU_GPIOB
#define SWITCH_YOUBEI_GPIO_PORT             GPIOB
/* 落杯器电机驱动信号 */
#define MOTOR_LUOBEI_PIN                    GPIO_PIN_12
#define MOTOR_LUOBEI_GPIO_CLK               RCU_GPIOA
#define MOTOR_LUOBEI_GPIO_PORT              GPIOA



//////储水模块//////
/* 液位开关信号 */
#define SWITCH_WATER_PIN                    GPIO_PIN_5
#define SWITCH_WATER_GPIO_CLK               RCU_GPIOB
#define SWITCH_WATER_GPIO_PORT              GPIOB
/* 温度传感器 */
#define TEMPERATURE_PIN                     GPIO_PIN_0
#define TEMPERATURE_PORT                    GPIOA
#define TEMPERATURE_CHANNEL                 ADC_CHANNEL_0


//////加注模块//////
/* 纯水泵电机驱动信号 */
#define MOTOR_WATER_PIN                     GPIO_PIN_10
#define MOTOR_WATER_GPIO_CLK                RCU_GPIOA
#define MOTOR_WATER_GPIO_PORT               GPIOA
/* 酶液泵电机驱动信号 */
#define MOTOR_ENZYME_PIN                    GPIO_PIN_11
#define MOTOR_ENZYME_GPIO_CLK               RCU_GPIOA
#define MOTOR_ENZYME_GPIO_PORT              GPIOA


//////运杯模块//////
/* 步进电机信号脚 */
#if 0
#define MOTOR_STEP_PIN_A_l                  GPIO_PIN_13
#define MOTOR_STEP_PIN_B_l                  GPIO_PIN_14
#define MOTOR_STEP_l_GPIO_CLK               RCU_GPIOB
#define MOTOR_STEP_l_GPIO_PORT              GPIOB

#define MOTOR_STEP_PIN_A_h                  GPIO_PIN_8
#define MOTOR_STEP_PIN_B_h                  GPIO_PIN_9
#define MOTOR_STEP_h_GPIO_CLK               RCU_GPIOA
#define MOTOR_STEP_h_GPIO_PORT              GPIOA
#else
#define STEP_DIR_GPIO_PORT                  GPIOA
#define STEP_DIR_GPIO_PIN                   GPIO_PIN_8

#endif

#define TIME_23_59_59                       (0x00015180)
//#define DEBUG_PRINT
#define NTC_10K_3950                        0
#define PT_100                              1


#define MOTOR_STEP                          50
#define MOTOR_PWM                           100

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
    LOOP_IDLE = 0,
    LOOP_LUOBEI,//落杯过程
    LOOP_LUOBEI_DETECT,//落杯过程监测
    LOOP_ZHUYE, //注液过程
    LOOP_ZHUYE_DETECT, //注液过程监测
    LOOP_CHUBEI,//出杯过程
    LOOP_CHUBEI_DETECT,//出杯过程监测
    LOOP_QIBEI, //弃杯过程
    LOOP_ERROR = 0xff
} Loop_State_e;

typedef enum {
    READY_ = 0,
    RESET_,
    CHUBEI_,
    HUISHOU_,

    POSITION_ERROR = 4, //用于告警状态显示，值不可随意调整
    WUBEI_ERROR,
    QIBEI_ERROR,
    ZHUSHUI_ERROR,

    WENDU_ERROR,
    SHUIWEI_ERROR,

    STATUS_NULL,
    ERROR_max
} error_type_e;

typedef enum {
    INIT_WATER_LEVEL = 0,
    EMPTY_WATER_LEVEL,
    FULL_WATER_LEVEL,

} water_level_e;


/* global variable declaration */
/* reference variable */
extern uint16_t adc_reference_filtered;
extern int16_t torque_reference;

/* global structure */
extern pid_parameter speed, torque, flux, flux_weaken;
extern flash_page_type page_type;


/* debug varieble */
extern usart_debug debug_data;
extern PID_Parm pidParm;
extern ctrl_opt_type_e ctrlOptType;

extern uint8_t state_youbei, state_t1, state_t2, self_diagnose, self_diag_first_time_flag;
extern uint8_t state_enzyme_count_running, state_qubei_count_running, state_jiazhu_error_count_running, \
    state_position_error_count_running, state_temperature_error_count_running;
extern uint8_t g_exti_chubei_protet_position_flag, g_exti_qibei_position_flag, g_exti_luobei_position_flag;
extern uint8_t g_exti_zhushui_position_flag, g_exti_chubei_position_flag;
extern uint8_t state_enzyme_ok, state_water_ok;
extern uint8_t state_qubei_timeout, state_position_error_timeout, \
    state_temperature_error_timeout, state_jiazhu_error_timeout;
extern uint8_t temperature_set, enzyme_rate;
extern uint16_t cup_count, cup_flag, water_set;
extern uint8_t luobei_retry, luobei_delay;
extern uint16_t g_water_count, water_count_signals, enzyme_count_times;
extern uint32_t position_error_count_times, temperature_error_count_times, jiazhu_error_count_times;
extern float temperature, temperature_f, temperature_cb;
extern uint16_t error_bits_flag;
extern uint8_t start_work;
extern uint16_t lcd_water_set;
extern uint8_t lcd_enzyme_rate, lcd_temperature_set, heat_disable;
extern uint32_t lcd_timestamp_set;
extern Loop_State_e loop_state;
extern uint8_t lcd_update_flag, lcd_ok_flag;
extern water_level_e state_fuzi;
extern uint8_t warnning_loop;

extern void set_error(error_type_e err_bit);
extern void clear_error(error_type_e err_bit);
extern void step_motor_move_forward(uint16_t steps);
extern void step_motor_move_reverse(uint16_t steps);
extern void usart2_data_transfer(uint8_t *usart_data, uint8_t len);
extern void report_operation_handle(uint8_t opt);
extern uint8_t lcd_check_conn_status();
#endif /* GLOBAL_H */
