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
    /* systick initialization */
    systick_config();

    /* peripheral initialization */
    hardware_config();

    /* hardware initialization */
    luobei_exti_init();             //落杯器完成落杯动作，计数
    protect_exit_init();            //安全限位开关
    velocity_exti_init();           //流量传感器信号中断输入
    position1_exit_init();
    position2_exit_init();
    position3_exit_init();

    luobei_motor_init();
    water_motor_init();
    enzyme_motor_init();

    key_init();                 //开关输入信号初始化
    led_init();

    delay_1ms(100);
    printf("\r\nsystem ready!\r\n");

    /* tool function initialization */
    utils_sample_init();
    /* read the hall */
    fmc_data_read();
    extern uint8_t fmc_data[3][8];
    water_set = fmc_data[0][3];
    enyzme_set = fmc_data[0][2];
    temperature_set = fmc_data[0][1];
    cup_count = fmc_data[0][0];
    //g_torque_offset = (float)(fmc_data[1][0] + fmc_data[1][1]) / 100.0f;

    printf("[fmc][1]: %x %x\r\n", fmc_data[1][0], fmc_data[1][1]);

    while(1){

        if (bTimeFlag_5ms)
        {
            bTimeFlag_5ms = 0;

            if (rx_ok)
            {
                rx_ok = 0;
                //printf("rx from controller:%x %x %x %x\r\n", \
                        rx_buffer_app[0], rx_buffer_app[1],\
                        rx_buffer_app[2], rx_buffer_app[3]);
                controller_msg_process(rx_buffer_app);
            }

            if (debug_rx_ok)
            {
                debug_rx_ok = 0;
                //printf("debug echo:%s\r\n", debug_rx_buffer_app);
                debug_msg_process(debug_rx_buffer_app);
                memset(debug_rx_buffer_app, 0, USART1_BUFFER_SIZE);
            }
        }

        if (bTimeFlag_50ms)
        {
            bTimeFlag_50ms = 0;
            
            /*
            //状态诊断   状态进入 和 状态恢复
            key_read_state(KEY_guangdian);
            key_read_state(KEY_fuzi);
            key_read_state(KEY_T1);
            key_read_state(KEY_T2);
            */
            ebike_read_sample();

            if (controller_ready_flag) {
                ebike_process();
            }
            #if 0
            static FlagStatus breathe_flag = SET;
            static int16_t i = 0;
            if (SET == breathe_flag){
                i = i + 10;
            }else{
                i = i - 10;
            }
            if(i > 500){
                breathe_flag = RESET;
            }
            if(i <= 0){
                breathe_flag = SET;
            }
            /* configure TIMER channel output pulse value */
            timer_channel_output_pulse_value_config(TIMER7,TIMER_CH_0,i);
            #endif
        }

        if (bTimeFlag_100ms)
        {
            bTimeFlag_100ms = 0;

            /* key process routine */
            key_process();

            work_loop();
        }

        if (bTimeFlag_500ms)
        {
            bTimeFlag_500ms = 0;

            /* oled update */
            //oled_display();
            //led_toggle(LED_RUNNING_GPIO_PORT, LED_RUNNING_PIN);
        }

        if (bTimeFlag_1s)
        {
            bTimeFlag_1s = 0;
            calc_cadence();
            calc_velocity_1_second();

            heat_running();

            if (!controller_ready_flag) {
                motor_sent_set(36, 10);//36V,10A
            }
        }

        if (bTimeFlag_3s)
        {
            bTimeFlag_3s = 0;
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

