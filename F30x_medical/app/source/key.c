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
    rcu_periph_clock_enable(KEY_GENERATOR_UP_GPIO_CLK);
    rcu_periph_clock_enable(KEY_GENERATOR_DOWN_GPIO_CLK);
    rcu_periph_clock_enable(KEY_MODE_UP_GPIO_CLK);
    rcu_periph_clock_enable(KEY_MODE_DOWN_GPIO_CLK);
    rcu_periph_clock_enable(RCU_AF);

    /* key initialize */
    gpio_init(KEY_GENERATOR_UP_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, KEY_GENERATOR_UP_PIN);
    gpio_init(KEY_GENERATOR_DOWN_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, KEY_GENERATOR_DOWN_PIN);
    gpio_init(KEY_MODE_UP_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, KEY_MODE_UP_PIN);
    gpio_init(KEY_MODE_DOWN_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, KEY_MODE_DOWN_PIN);
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
        case KEY_GENERATOR_UP:
            data = gpio_input_bit_get(KEY_GENERATOR_UP_GPIO_PORT, KEY_GENERATOR_UP_PIN);
            break;
        case KEY_GENERATOR_DOWN:
            data = gpio_input_bit_get(KEY_GENERATOR_DOWN_GPIO_PORT, KEY_GENERATOR_DOWN_PIN);
            break;
        case KEY_MODE_UP:
            data = gpio_input_bit_get(KEY_MODE_UP_GPIO_PORT, KEY_MODE_UP_PIN);
            break;
        case KEY_MODE_DOWN:
            data = gpio_input_bit_get(KEY_MODE_DOWN_GPIO_PORT, KEY_MODE_DOWN_PIN);
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
        delay_1ms(10);
        if(key_read(key)){
            /* intermittent examination, press release to start */
            while(SET == key_read(key));
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
    /* generator up key is pressed */
    if(key_read_debouncing(KEY_GENERATOR_UP)){

        if (g_generator_gears < GENERATOR_5)
        {
            g_generator_gears++;
            printf("gear=%d\r\n", g_generator_gears);
        }
    }

    /* generator down key is pressed */
    if(key_read_debouncing(KEY_GENERATOR_DOWN)){
        
        if (g_generator_gears > GENERATOR_0)
        {
            g_generator_gears--;
            printf("gear=%d\r\n", g_generator_gears);
        }
    }

    /* drive mode up key is pressed */
    if(key_read_debouncing(KEY_MODE_UP)){

        if (g_drive_mode < GEAR_5)
        {
            g_drive_mode++;
            printf("drive=%d\r\n", g_drive_mode);
            fmc_erase_pages(MODE_PAGE);
            fmc_data_program(MODE_PAGE);
        }
    }

    /* drive mode down key is pressed */
    if(key_read_debouncing(KEY_MODE_DOWN)){
        
        if (g_drive_mode > GEAR_0)
        {
            g_drive_mode--;
            printf("drive=%d\r\n", g_drive_mode);
            fmc_erase_pages(MODE_PAGE);
            fmc_data_program(MODE_PAGE);
        }
    }
}

/*
** 设置放电/充电电路开关比例，调节发电机阻力；
*/
void Reluctance_motor_running()
{
    //充电电压检测，过压关闭充电电路
    static uint8_t switch_ON = 0;
    if ((vbus_f > 42.0) && switch_ON){
        //关闭发电环路
        gpio_bit_reset(GENERATE_SWITCH_GPIO_PORT, GENERATE_SWITCH_PIN);
        switch_ON = 0;
    } else if (!switch_ON){
        //打开发电环路
        gpio_bit_set(GENERATE_SWITCH_GPIO_PORT, GENERATE_SWITCH_PIN);
        switch_ON = 1;
    }

    //阻力档位调节
    if (g_generator_gears == g_generator_gears_pre)
        return;
    
    //0 ---- 500
    /* configure TIMER channel output pulse value */
    timer_channel_output_pulse_value_config(TIMER7,TIMER_CH_0, (uint32_t) 500 * g_generator_gears / 5.0f);

    g_generator_gears_pre = g_generator_gears;
    fmc_erase_pages(MODE_PAGE);
    fmc_data_program(MODE_PAGE);
}

