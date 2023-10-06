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

#include "global.h"

/* reference variable */
//uint16_t adc_reference = 800;
uint16_t adc_reference_filtered = 0;
//uint16_t speed_reference = 0;
int16_t torque_reference = 0;
uint8_t state_youbei, state_t1, state_t2, state_fuzi;
uint8_t g_exti_qibei_position_flag, g_exti_luobei_position_flag;
uint8_t g_exti_zhushui_position_flag, g_exti_chubei_position_flag;


/* global structure */
uint16_t g_generator_power = 0, g_error_code = 0;
flash_page_type page_type;
Loop_State_e loop_state = LOOP_IDLE;    //机器运行状态机
uint8_t start_wort = 0;     //机器开启运行指令

/* debug variables */
usart_debug debug_data;


void work_loop( void )
{
    switch (loop_state) {
        case LOOP_IDLE:
            //弃杯微动开关位置，且弃杯成功状态
            if (start_wort) {
                if (!state_youbei && read_qibei_position_switch())
                loop_state = LOOP_LUOBEI;
                start_wort = 0;
            }
            break;

        case LOOP_LUOBEI://落杯过程
            //到位检测
            if (read_luobei_position_switch()) {
                luobei_motor_start();
                loop_state = LOOP_LUOBEI_DETECT;
            }
            break;

        case LOOP_LUOBEI_DETECT://落杯监测
            if (state_youbei) {
                luobei_motor_stop();
                step_motor_move_forward();
                loop_state = LOOP_ZHUYE;
            }
            break;

        case LOOP_ZHUYE: //注液过程
            break;

        case LOOP_CHUBEI://出杯过程
            break;

        case LOOP_QIBEI: //弃杯过程
            break;

        case LOOP_ERROR:
            break;

        default:
            printf("invalid state.\r\n");
    }
}