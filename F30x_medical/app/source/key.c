/*!
    \file    key.c
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

/* static function */
/* key read */
static FlagStatus key_read(key_switch key_sw);
/* key verify with debouncing  */
static FlagStatus key_read_debouncing(key_switch key_sw);

/*!
    \brief      initialize keys
    \param[in]  none
    \param[out] none
    \retval     none
*/
void key_init(void)
{
    /* enable the clock of GPIO and alternate function */
    rcu_periph_clock_enable(SWITCH_YOUBEI_GPIO_CLK);
    rcu_periph_clock_enable(SWITCH_WATER_GPIO_CLK);
    rcu_periph_clock_enable(RCU_AF);

    /* key initialize */
    gpio_init(SWITCH_YOUBEI_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, SWITCH_YOUBEI_PIN);
    gpio_init(SWITCH_WATER_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, SWITCH_WATER_PIN);
}

/*!
    \brief      read the state of the key
    \param[in]  none
    \param[out] none
    \retval     the state of the key
*/
static FlagStatus key_read(key_switch key)
{
    uint8_t data=0;

    switch(key){
        case KEY_youbei:
            data = gpio_input_bit_get(SWITCH_YOUBEI_GPIO_PORT, SWITCH_YOUBEI_PIN);
            break;
        case KEY_fuzi:
            data = gpio_input_bit_get(SWITCH_WATER_GPIO_PORT, SWITCH_WATER_PIN);
            break;
        default: data = 0;
    }

    if(data){
        return RESET;
    }

    return SET;
}

/*!
    \brief      key read debouncing
    \param[in]  none
    \param[out] none
    \retval     none
*/
static FlagStatus key_read_debouncing(key_switch key)
{
    if(key_read(key)){
        delay_1ms(5);
        if(key_read(key)){
            /* intermittent examination, press release to start */
            //while(SET == key_read(key));
            return SET;
        }
    }
    return RESET;
}

/*!
    \brief      key read state
    \param[in]  none
    \param[out] none
    \retval     none
*/
static FlagStatus key_read_state(key_switch key)
{
    if(key_read(key)){
        delay_1ms(5);
        if(key_read(key)){
            /* intermittent examination, press release to start */
            return SET;
        }
    }
    return RESET;
}

/*!
    \brief      key process
    \param[in]  none
    \param[out] none
    \retval     none
*/
void key_process(void)
{
    static uint8_t yb=0,fz=0;

    /* drive mode up key is pressed */
    if(!key_read_debouncing(KEY_youbei)){
        if (!state_youbei) {
            yb++;
            if (yb>=SWITCH_LUOBEI_TIME) {
                printf("you bei\r\n");
                state_youbei = 1;
                yb = 0;
            }
        }
    } else {
        if (state_youbei) {
            yb++;
            if (yb>=SWITCH_LUOBEI_TIME) {
                printf("wu bei\r\n");
                state_youbei = 0;
                yb = 0;
            }
        }
    }

    if(key_read_debouncing(KEY_fuzi)){
        if (state_fuzi == EMPTY_WATER_LEVEL || state_fuzi == INIT_WATER_LEVEL) {
            fz++;
            if (fz>=SWITCH_DETECT_TIME) {
                printf("Full Water\r\n");
                state_fuzi = FULL_WATER_LEVEL;
                fz = 0;
                //输出“水位不足”
                clear_error(SHUIWEI_ERROR);
                lcd_update_flag = 1;
            }
        }
    } else {
        if (state_fuzi == FULL_WATER_LEVEL || state_fuzi == INIT_WATER_LEVEL) {
            fz++;
            if (fz>=SWITCH_DETECT_TIME) {
                printf("Lack water\r\n");
                state_fuzi = EMPTY_WATER_LEVEL;
                fz = 0;
                //清除“水位不足”
                set_error(SHUIWEI_ERROR);
            }
        }
    }
}

