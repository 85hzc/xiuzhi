/*!
    \file    main.c
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
#include "systick.h"


/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint8_t seconds = 0;
    /* systick initialization */
    systick_config();

    /* peripheral initialization */
    hardware_config();

    rtc_init();
    /* clear reset flags */
    rcu_all_reset_flag_clear();

    /* hardware initialization */
    luobei_exti_init();             //落杯器完成落杯动作，计数
    qibei_exit_init();              //安全限位开关
    velocity_exti_init();           //流量传感器信号中断输入
    position1_exit_init();
    position2_exit_init();
    position3_exit_init();

    step_motor_init();
    luobei_motor_init();
    water_motor_init();
    enzyme_motor_init();

    key_init();                 //开关输入信号初始化
    led_init();

    delay_1ms(100);
    printf("\r\nsystem ready!\r\n");

    /* tool function initialization */
    utils_sample_init();
    config_init();
    self_test_init();
    ringbuff_init();

    //判断两个限位开关的状态，是否为到位状态，避免损坏机器
    //if(){}

    while(1){

        if (bTimeFlag_5ms)
        {
            bTimeFlag_5ms = 0;

            if (debug_rx_ok)
            {
                debug_rx_ok = 0;
                debug_msg_process(debug_rx_buffer_app);
                memset(debug_rx_buffer_app, 0, USART1_BUFFER_SIZE);
            }

            /*
            ** Handle knob screen cmd
            */
            if (len_ok()) {
                controller_msg_process();
            }
        }

        if (bTimeFlag_50ms)
        {
            bTimeFlag_50ms = 0;
            EBI_calcPI(&pidParm);

            //step_motor_work_check();
            if (lcd_ok_flag) {  //因为开机启动自检流程，电机占用cpu资源，会阻塞屏幕显示
                work_loop();
            }
        }

        if (bTimeFlag_100ms)
        {
            bTimeFlag_100ms = 0;

            #ifndef DEBUG
            ebike_read_temperature();
            #endif
            /* key process routine */
            key_process();
        }

        if (bTimeFlag_500ms)
        {
            bTimeFlag_500ms = 0;
            ebike_check_warning();
            heat_running();
        }

        if (bTimeFlag_1s)
        {
            bTimeFlag_1s = 0;

            if (cup_flag) {
                cup_flag = 0;
                flash_value_flash();
            }

            lcd_update_timer_flag_check();
            //printf("[AD]temperature:%.1f  %.1f, pwm=%.1f  %d\r\n", temperature_f, temperature_cb, pidParm.qOut, (uint32_t)pidParm.qOut);
        
            //发生错误，人为修之后，短按进入弃杯流程，重启设备,同时恢复错误标记位
            if (start_work && serious_error()) {
                start_work = 0;
                loop_state = LOOP_QIBEI;
                serious_error_clear();
            }

            if (lcd_check_conn_status() == COMM_DISCONN) {
                // init knob screen display
                lcd_init_display();
            }
        }
    }
}

/* retarget the C library printf function to the usart */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART1, (uint8_t) ch);
    while(RESET == usart_flag_get(USART1, USART_FLAG_TBE));
    return ch;
}

