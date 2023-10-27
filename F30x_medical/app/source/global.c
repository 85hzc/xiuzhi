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

void debug_msg_process(uint8_t *msg);

/* reference variable */
uint8_t state_youbei, state_t1, state_t2, state_fuzi, self_diagnose = 0;
uint8_t state_enzyme_count_running, state_qubei_count_running, state_jiazhu_error_count_running, \
    state_position_error_count_running, state_temperature_error_count_running;
uint8_t g_exti_qibei_position_flag, g_exti_luobei_position_flag;
uint8_t g_exti_zhushui_position_flag, g_exti_chubei_position_flag;
uint8_t state_enzyme_ok = 0, state_water_ok = 0;
uint8_t state_qubei_timeout = 0, state_position_error_timeout = 0, \
    state_temperature_error_timeout = 0, state_jiazhu_error_timeout = 0;
uint8_t temperature_set;
uint8_t enzyme_rate;   //稀释比例，如： 1%=1，15%=15（整型数0--100）
uint8_t start_work = 0;     //机器开启运行指令（触摸屏控件启动），初始化为0
uint8_t luobei_retry = 0, luobei_delay = 0;   //二次尝试取杯操作
uint16_t g_water_count = 0, water_count_signals = 0, enzyme_count_times = 0;
uint32_t position_error_count_times = 0, temperature_error_count_times = 0, jiazhu_error_count_times = 0;

/* global structure */
flash_page_type page_type;
Loop_State_e loop_state = LOOP_QIBEI;    //机器运行状态机

/* debug variables */
usart_debug debug_data;

// extern uint8_t read_qibei_position_switch();
// extern uint8_t read_luobei_position_switch();
// extern uint8_t read_zhushui_position_switch();
// extern uint8_t read_chubei_position_switch();
// extern void position_error_timer_clear( void );

void clear_position_flags( void )
{
    g_exti_qibei_position_flag = 0;
    g_exti_luobei_position_flag = 0;
    g_exti_chubei_position_flag = 0;
    g_exti_zhushui_position_flag = 0;
}
            

void work_loop( void )
{

    switch (loop_state) {
        case LOOP_IDLE:
            //弃杯微动开关位置，且弃杯成功状态
            if (!state_youbei && read_qibei_position_switch()) {
                printf(">>>LOOP_IDLE, go to LOOP_LUOBEI\r\n");
                //弃杯成功，启动前进至落杯位置
                position_error_timer_start(5);
                step_motor_move_forward(STEP_MOTOR_STEPS);
                loop_state = LOOP_LUOBEI;
            } else if (read_qibei_position_switch()) {  //显示返回到位，重置超时预警定时器
                position_error_timer_clear();
            }
            break;

        case LOOP_LUOBEI:       //落杯过程
            if (read_luobei_position_switch()) {    //到位检测
                position_error_timer_clear();
                if (start_work && !self_diagnose) { //防止自建过程杯手动干扰
                    luobei_motor_start();
                    printf(">>>LOOP_LUOBEI, go to LOOP_LUOBEI_DETECT\r\n");
                    loop_state = LOOP_LUOBEI_DETECT;
                    start_work = 0;
                    luobei_delay = 0;
                    luobei_retry = 0;
                } else if (self_diagnose){          //自检流程
                    loop_state = LOOP_LUOBEI_DETECT;
                }
            }
            break;

        case LOOP_LUOBEI_DETECT:    //落杯监测
            if (state_youbei) {
                luobei_motor_stop();
                delay_1ms(500);
                position_error_timer_start(5);
                step_motor_move_forward(STEP_MOTOR_STEPS);
                printf(">>>LOOP_LUOBEI_DETECT, go to LOOP_ZHUYE\r\n");
                loop_state = LOOP_ZHUYE;
                luobei_delay = 0;
                luobei_retry = 0;
            }  else if (!self_diagnose && !state_youbei) {         //二次尝试取杯操作

                if (luobei_retry) { // 1：执行一次落杯操作，尝试二次落杯过程
                    if (luobei_retry>=2) {  //二次落杯失败，提示无杯错误
                        luobei_motor_stop();
                        luobei_delay++;
                        //延时判断落杯状态，因为落杯动作需要时间（落杯状态key判断需要SWITCH_LUOBEI_TIME=2（200ms））
                        if (luobei_delay>=SWITCH_LUOBEI_TIME+8) { //延时1000ms
                            set_error(WUBEI_ERROR);
                            luobei_delay = 0;
                            luobei_retry = 0;
                        }
                    }
                }
            } else if (self_diagnose){          //自检流程
                printf(">>>[self diagnos]LOOP_LUOBEI_DETECT, go to LOOP_ZHUYE\r\n");
                loop_state = LOOP_ZHUYE;
                position_error_timer_start(5);
            }
            break;

        case LOOP_ZHUYE: //注液过程
            if (self_diagnose && read_zhushui_position_switch()) {
                position_error_timer_clear();
                position_error_timer_start(5);
                printf(">>>LOOP_ZHUYE, go to LOOP_CHUBEI\r\n");
                loop_state = LOOP_CHUBEI;
            } else if (read_zhushui_position_switch()) {    //到位检测
                position_error_timer_clear();
                state_enzyme_count_running = 1;
                enzyme_motor_start();
                g_water_count = 0;
                water_motor_start();
                loop_state = LOOP_ZHUYE_DETECT;
                printf(">>>LOOP_ZHUYE, go to LOOP_ZHUYE_DETECT\r\n");
                jiazhu_error_timer_start(60);   //计时1min
            }
            break;

        case LOOP_ZHUYE_DETECT:
            if (state_water_ok && state_enzyme_ok) {
                jiazhu_error_timer_clear();
                state_water_ok = 0;
                state_enzyme_ok = 0;
                loop_state = LOOP_CHUBEI;
                printf(">>>LOOP_ZHUYE_DETECT, go to LOOP_CHUBEI\r\n");
            }
            break;

        case LOOP_CHUBEI://出杯过程
            printf(">>>LOOP_CHUBEI, go to LOOP_CHUBEI_DETECT\r\n");
            position_error_timer_start(5);
            step_motor_move_forward(STEP_MOTOR_STEPS);      //驱动到取杯位置
            loop_state = LOOP_CHUBEI_DETECT;
            if (!self_diagnose) {
                state_qubei_count_running = 1;  //取杯15分钟超时计时，放到“LOOP_CHUBEI”此处简化逻辑，计时会有几秒误差
            }
            break;

        case LOOP_CHUBEI_DETECT://出杯位置检测
            if (read_chubei_position_switch()) {    //到位检测
                position_error_timer_clear();
                if (self_diagnose) {
                    //自检流程回退到落杯位置
                    printf(">>>[self diagnos]LOOP_CHUBEI_DETECT, go to LOOP_LUOBEI\r\n");
                    position_error_timer_start(5);
                    step_motor_move_reverse(STEP_MOTOR_STEPS);
                    loop_state = LOOP_LUOBEI;
                    self_diagnose = 0;          //自己流程结束
                } else if (start_work || state_qubei_timeout) { //按下启动/复位开关 or 取杯超时
                    printf(">>>LOOP_CHUBEI_DETECT, go to LOOP_QIBEI\r\n");
                    loop_state = LOOP_QIBEI;
                    qubei_timer_init();
                }
            }
            break;

        case LOOP_QIBEI: //弃杯过程
            if (!read_qibei_position_switch()) {//不在弃杯位置，需要返回初始位置
                printf(">>>1 LOOP_QIBEI, go to LOOP_IDLE\r\n");
                position_error_timer_start(10);
                step_motor_move_reverse(STEP_MOTOR_STEPS);
                loop_state = LOOP_IDLE;
            } else {    //在弃杯位置，可以正常启动工作
                printf(">>>2 LOOP_QIBEI, go to LOOP_IDLE\r\n");
                loop_state = LOOP_IDLE;
            }
            break;

        case LOOP_ERROR:
            break;

        default:
            printf("invalid state.\r\n");
    }
}