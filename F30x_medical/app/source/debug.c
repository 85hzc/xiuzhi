/*!
    \file    debug.c
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

/* static function declaration */

/* debug variables */
uint8_t debug_rx_buffer[USART1_BUFFER_SIZE];
uint8_t debug_rx_buffer_app[USART1_BUFFER_SIZE];
uint16_t debug_rx_counter = 0, debug_rx_ok = 0;


/*!
    \brief      configure USART
    \param[in]  none
    \param[out] none
    \retval     none
*/
void debug_init(void)
{

    /* clock enable */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* USART1_RX: PA3 */
    gpio_init(DEBUG_USART_RX_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, DEBUG_USART_RX_PIN);
    /* USART1_TX: PA2 */
    gpio_init(DEBUG_USART_TX_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, DEBUG_USART_TX_PIN);

    /* clock enable */
    rcu_periph_clock_enable(DEBUG_USART_CLOCK);

    /* USART0 configure */
    usart_deinit(DEBUG_USART);
    usart_baudrate_set(DEBUG_USART, 115200);
    usart_stop_bit_set(DEBUG_USART, USART_STB_1BIT);
    usart_word_length_set(DEBUG_USART, USART_WL_8BIT);
    usart_parity_config(DEBUG_USART, USART_PM_NONE);
    usart_hardware_flow_rts_config(DEBUG_USART, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(DEBUG_USART, USART_CTS_DISABLE);
    usart_receive_config(DEBUG_USART, USART_RECEIVE_ENABLE);
    usart_transmit_config(DEBUG_USART, USART_TRANSMIT_ENABLE);
    //usart_flag_clear(DEBUG_USART, USART_FLAG_TC);
    usart_enable(DEBUG_USART);

    /* enable USART1 receive interrupt */
    usart_interrupt_enable(DEBUG_USART, USART_INT_RBNE);
    /* enable USART1 transmit interrupt */
    //usart_interrupt_enable(DEBUG_USART, USART_INT_TBE);
}


void debug_msg_process(uint8_t *msg)
{
    uint8_t cmd;

    cmd = msg[0];
    switch(cmd)
    {
        case 1://set real time temperature for pid working
            temperature = msg[1];
            printf("temperature = %f\r\n", temperature);
            pidParm.qInMeas = temperature;  //for test
            break;

        case 2://加热温度设定值
            temperature_set = msg[1];
            printf("temperature_set = %d\r\n", temperature_set);
            flash_value_flash();
            break;

        case 3://落杯电机启停
            if (msg[1])
                luobei_motor_start();
            else
                luobei_motor_stop();

            printf("luo bei.\r\n");
            break;

        case 4://注水量设置
            water_set = msg[1] | msg[2];
            printf("water_set = %d\r\n", water_set);
            flash_value_flash();
            break;

        case 5://稀释比例
            enzyme_rate = msg[1];
            printf("enzyme_rate = %d\r\n", enzyme_rate);
            flash_value_flash();
            break;

        case 6://
            if (msg[1]==1) {
                printf("step motor f\r\n");
                step_motor_move_forward(STEP_MOTOR_STEPS);
            } else if (msg[1]==2) {
                printf("step motor r\r\n");
                step_motor_move_reverse(STEP_MOTOR_STEPS);
            } else {
                step_motor_move_stop();
            }
            break;

        case 7:
            printf("time:%2d:%2d:%2d\r\n",msg[1],msg[2],msg[3]);
            time_adjust(msg[1],msg[2],msg[3]);
            break;

        case 8://纯水泵调试
            if (msg[1]) {
                printf("water motor start\r\n");
                water_motor_start();
            } else {
                printf("water motor stop\r\n");
                water_motor_stop();
            }
            break;
            

        default:
            break;
    }
}
