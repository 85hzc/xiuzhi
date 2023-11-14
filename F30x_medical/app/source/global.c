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
uint8_t state_youbei, state_t1, state_t2, self_diagnose = 1, self_diag_first_time_flag = 1;
uint8_t state_enzyme_count_running, state_qubei_count_running, state_jiazhu_error_count_running, \
    state_position_error_count_running, state_temperature_error_count_running;
uint8_t g_exti_chubei_protet_position_flag, g_exti_qibei_position_flag, g_exti_luobei_position_flag;
uint8_t g_exti_zhushui_position_flag, g_exti_chubei_position_flag;
uint8_t state_enzyme_ok = 0, state_water_ok = 0;
uint8_t state_qubei_timeout = 0, state_position_error_timeout = 0, \
    state_temperature_error_timeout = 0, state_jiazhu_error_timeout = 0;
uint8_t temperature_set;
uint8_t enzyme_rate;   //稀释比例，如： 1%=1，15%=15（整型数0--255） 最大25.5%
uint8_t start_work = 0;     //机器开启运行指令（触摸屏控件启动），初始化为0
uint8_t luobei_retry = 0, luobei_delay = 0;   //二次尝试取杯操作
uint16_t g_water_count = 0, water_count_signals = 0, enzyme_count_times = 0;
uint32_t position_error_count_times = 0, temperature_error_count_times = 0, jiazhu_error_count_times = 0;
water_level_e state_fuzi = EMPTY_WATER_LEVEL;

/* global structure */
flash_page_type page_type;
Loop_State_e loop_state = LOOP_QIBEI;    //机器运行状态机

/* debug variables */
usart_debug debug_data;


void clear_position_flags( void )
{
    g_exti_qibei_position_flag = 0;
    g_exti_luobei_position_flag = 0;
    g_exti_chubei_position_flag = 0;
    g_exti_zhushui_position_flag = 0;
}

void printf_exti_flags( void )
{
    printf("cb:%d, zs:%d, lb:%d, qb:%d, err:%d\r\n", 
        g_exti_chubei_position_flag, 
        g_exti_zhushui_position_flag,
        g_exti_luobei_position_flag, 
        g_exti_qibei_position_flag,
        state_position_error_timeout);
}

void self_test_init( void )
{
    self_diag_first_time_flag = 1;
    self_diagnose = 1;
    loop_state = LOOP_QIBEI;
}

void work_loop( void )
{
    static uint8_t lcd_flash_once = 0;

    switch (loop_state) {
        case LOOP_IDLE:
            //弃杯微动开关位置，且弃杯成功状态
            if (!state_youbei && read_qibei_position_switch()) {
                clear_error(QIBEI_ERROR);
                lcd_update_flag = 1;

                //落杯位置信号中断
                exti_interrupt_enable(N1S_LUOBEI_EXTI_LINE);
                delay_1ms(500);
                #ifdef DEBUG_PRINT
                printf(">>>LOOP_IDLE, go to LOOP_LUOBEI\r\n");
                #endif
                //弃杯成功，启动前进至落杯位置
                position_error_timer_start(10);
                self_diag_first_time_flag = 0;  //从出杯口自检返回时，防止经过落杯位置停机
                loop_state = LOOP_LUOBEI;
                lcd_flash_once = 1;
                step_motor_move_forward(STEP_MOTOR_STEPS);
            } else if (read_qibei_position_switch()) {  //显示返回到位，重置超时预警定时器
                
                position_error_timer_clear();
                //弃杯不成功的告警提示
                set_error(QIBEI_ERROR);
                lcd_update_flag = 1;
            }
            break;

        case LOOP_LUOBEI:       //落杯过程
            if (read_luobei_position_switch()) {    //到位检测
                position_error_timer_clear();
                if (start_work && !self_diagnose) { //防止自检过程被手动干扰

                    //如果是按键重启，有可能是错误恢复的情况，需要清空可能存在的错误
                    if (start_work && serious_error()) {
                        serious_error_clear();
                    }

                    loop_state = LOOP_LUOBEI_DETECT;
                    start_work = 0;
                    luobei_delay = 0;
                    luobei_retry = 0;
                    luobei_motor_start();
                    #ifdef DEBUG_PRINT
                    printf(">>>LOOP_LUOBEI, go to LOOP_LUOBEI_DETECT\r\n");
                    #endif
                } else if (self_diagnose){          //自检流程
                    exti_interrupt_enable(N2S_ZHUSHUI_EXTI_LINE);
                    loop_state = LOOP_ZHUYE;
                    delay_1ms(500);
                    position_error_timer_start(10);
                    #ifdef DEBUG_PRINT
                    printf(">>>[self diagnos]LOOP_LUOBEI, go to LOOP_ZHUYE\r\n");
                    #endif
                    step_motor_move_forward(STEP_MOTOR_STEPS);
                } else if (lcd_flash_once){
                    set_error(READY_);
                    clear_error(HUISHOU_);
                    lcd_update_flag = 1;
                    lcd_flash_once = 0;
                }
            }
            break;

        case LOOP_LUOBEI_DETECT:    //落杯监测
            if (state_youbei) {
                luobei_motor_stop();
                delay_1ms(500);
                exti_interrupt_enable(N2S_ZHUSHUI_EXTI_LINE);
                loop_state = LOOP_ZHUYE;
                luobei_delay = 0;
                luobei_retry = 0;
                position_error_timer_start(10);
                step_motor_move_forward(STEP_MOTOR_STEPS);
                #ifdef DEBUG_PRINT
                printf(">>>LOOP_LUOBEI_DETECT, go to LOOP_ZHUYE\r\n");
                #endif
            }  else if (!self_diagnose && !state_youbei) {         //二次尝试取杯操作

                //set_error(STATUS_NULL);
                if (luobei_retry) { // 1：执行一次落杯操作，尝试二次落杯过程
                    if (luobei_retry>=2) {  //二次落杯失败，提示无杯错误
                        #ifdef DEBUG_PRINT
                        printf("stop retry cup\r\n");
                        #endif
                        luobei_motor_stop();
                        luobei_delay++;
                        //延时判断落杯状态，因为落杯动作需要时间（落杯状态key判断需要SWITCH_LUOBEI_TIME=2（200ms））
                        if (luobei_delay>=SWITCH_LUOBEI_TIME+8) { //延时1000ms
                            set_error(WUBEI_ERROR);
                            lcd_update_flag = 1;
                            luobei_delay = 0;
                            luobei_retry = 0;
                        }
                    }
                }
            }
            break;

        case LOOP_ZHUYE: //注液过程
            if (self_diagnose && read_zhushui_position_switch()) {
                position_error_timer_clear();
                position_error_timer_start(10);
                #ifdef DEBUG_PRINT
                printf(">>>[self diagnos]LOOP_ZHUYE, go to LOOP_CHUBEI\r\n");
                #endif
                loop_state = LOOP_CHUBEI;
            } else if (read_zhushui_position_switch()) {    //到位检测
                position_error_timer_clear();
                exti_interrupt_enable(VELOCITY_EXTI_LINE);
                state_enzyme_count_running = 1;
                state_enzyme_ok = 0;
                enzyme_motor_start();
                g_water_count = 0;
                state_water_ok = 0;
                water_motor_start();
                loop_state = LOOP_ZHUYE_DETECT;
                #ifdef DEBUG_PRINT
                printf(">>>LOOP_ZHUYE, go to LOOP_ZHUYE_DETECT\r\n");
                #endif
                jiazhu_error_timer_start(60);   //计时1min
            }
            break;

        case LOOP_ZHUYE_DETECT:
            if (state_water_ok && state_enzyme_ok) {
                jiazhu_error_timer_clear();
                state_water_ok = 0;
                state_enzyme_ok = 0;
                loop_state = LOOP_CHUBEI;
                #ifdef DEBUG_PRINT
                printf(">>>LOOP_ZHUYE_DETECT, go to LOOP_CHUBEI\r\n");
                #endif
            }
            break;

        case LOOP_CHUBEI://出杯过程
            if (!self_diagnose) {
                set_error(CHUBEI_);
                clear_error(READY_);
                clear_error(STATUS_NULL);
            }
            exti_interrupt_enable(N3S_CHUBEI_EXTI_LINE);
            delay_1ms(500);
            loop_state = LOOP_CHUBEI_DETECT;
            #ifdef DEBUG_PRINT
            printf(">>>LOOP_CHUBEI, go to LOOP_CHUBEI_DETECT\r\n");
            #endif
            position_error_timer_start(10);
            exti_interrupt_disable(N2S_ZHUSHUI_EXTI_LINE);
            step_motor_move_forward(STEP_MOTOR_STEPS);      //驱动到取杯位置
            if (!self_diagnose) {
                state_qubei_count_running = 1;  //取杯15分钟超时计时，放到“LOOP_CHUBEI”此处简化逻辑，计时会有几秒误差
            }
            break;

        case LOOP_CHUBEI_DETECT://出杯位置检测
            if (read_chubei_position_switch()) {    //到位检测
                position_error_timer_clear();
                if (self_diagnose) {
                    //自检流程回退到落杯位置
                    #ifdef DEBUG_PRINT
                    printf(">>>[self diagnos]LOOP_CHUBEI_DETECT, go to LOOP_LUOBEI\r\n");
                    #endif
                    delay_1ms(500);
                    position_error_timer_start(60);
                    loop_state = LOOP_LUOBEI;
                    exti_interrupt_enable(N1S_LUOBEI_EXTI_LINE);
                    step_motor_move_reverse(STEP_MOTOR_STEPS);
                    self_diagnose = 0;          //自检流程结束
                } else if (start_work || state_qubei_timeout) { //按下启动/复位开关 or 取杯超时
                    #ifdef DEBUG_PRINT
                    printf(">>>LOOP_CHUBEI_DETECT, go to LOOP_QIBEI [%s]\r\n", start_work ? "start_work" : "qubei timeout");
                    #endif
                    loop_state = LOOP_QIBEI;
                    qubei_timer_init();

                    //如果是按键重启，有可能是错误恢复的情况，需要清空可能存在的错误
                    if (start_work && serious_error()) {
                        start_work = 0;
                        serious_error_clear();
                    }
                }
            } else {
                #ifdef DEBUG_PRINT
                printf("chubei position detect fail.\r\n");
                #endif
            }
            break;

        case LOOP_QIBEI:
            //弃杯过程，自检流程入口
            //需要同时复位自检流程标记参数：self_diag_first_time_flag = 1, self_diagnose = 1;
            if (!read_qibei_position_switch()) {//不在弃杯位置，需要返回初始位置
                #ifdef DEBUG_PRINT
                printf(">>>1 LOOP_QIBEI, go to LOOP_IDLE\r\n");
                #endif
                delay_1ms(500);
                position_error_timer_start(60);
                step_motor_move_reverse(STEP_MOTOR_STEPS);
                loop_state = LOOP_IDLE;
                set_error(HUISHOU_);
            } else {    //在弃杯位置，可以正常启动工作
                #ifdef DEBUG_PRINT
                printf(">>>2 LOOP_QIBEI, go to LOOP_IDLE\r\n");
                #endif
                loop_state = LOOP_IDLE;
            }
            break;

        case LOOP_ERROR:
            break;

        default:
            printf("invalid state.\r\n");
    }
}

void step_work_check( void )
{
    static uint8_t time_delay = 0;

    if (g_exti_chubei_protet_position_flag) {
        motor_off();
    }

    //出杯过程
    if (((loop_state == LOOP_LUOBEI) && g_exti_luobei_position_flag ) ||
        ((loop_state == LOOP_CHUBEI_DETECT) && g_exti_chubei_position_flag ) ||
        ((loop_state == LOOP_ZHUYE) && g_exti_zhushui_position_flag ) ||
        state_position_error_timeout ||
        serious_error()) {

        if (time_delay++ >= 2) {
            clear_position_flags();
            motor_off();
            time_delay = 0;
            //printf_exti_flags();
            return;
        }
    }

    //回收过程
    if (g_exti_qibei_position_flag ||
        state_position_error_timeout ||
        ((loop_state == LOOP_LUOBEI) && self_diagnose && 
            g_exti_luobei_position_flag && !self_diag_first_time_flag) ||
        serious_error()) {

        if (time_delay++ >= 2) {
            clear_position_flags();
            motor_off();
            time_delay = 0;
            //printf_exti_flags();
            return;
        }
    }

}
