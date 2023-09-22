/*!
    \file    led.c
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
void led_on(mc_led led);
void led_off(mc_led led);

/*!
    \brief      initialize the GPIO port of led
    \param[in]  led
    \param[out] none
    \retval     none
*/
void led_init(void)
{
    /* enable the clock of GPIO */
    rcu_periph_clock_enable(LED_RUNNING_GPIO_CLK);
    rcu_periph_clock_enable(LED_BRAKE_GPIO_CLK);
    
    /* RUNNING, BRAKE, FAULT led initialize */
    gpio_init(LED_RUNNING_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LED_RUNNING_PIN);
    gpio_init(LED_BRAKE_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LED_BRAKE_PIN);
    
    /* turn off all the led */
    led_off(LED_ALL);
}

/*!
    \brief      turn off the selection led 
    \param[in]  led: the selection led
    \param[out] none
    \retval     none
*/
void led_off(mc_led led)
{
    switch(led){
        case LED_RUNNING:
            gpio_bit_set(LED_RUNNING_GPIO_PORT, LED_RUNNING_PIN);
            break;
        case LED_BRAKE:
            gpio_bit_set(LED_BRAKE_GPIO_PORT, LED_BRAKE_PIN);
            break;
        case LED_ALL:
            gpio_bit_set(LED_RUNNING_GPIO_PORT, LED_RUNNING_PIN);
            gpio_bit_set(LED_BRAKE_GPIO_PORT, LED_BRAKE_PIN);
            break;
        default:
            break;
    }
}

/*!
    \brief      turn on the selection led 
    \param[in]  led: the selection led
    \param[out] none
    \retval     none
*/
void led_on(mc_led led)
{
    switch(led){
        case LED_RUNNING:
            gpio_bit_reset(LED_RUNNING_GPIO_PORT, LED_RUNNING_PIN);
            break;
        case LED_BRAKE:
            gpio_bit_reset(LED_BRAKE_GPIO_PORT, LED_BRAKE_PIN);
            break;
        case LED_ALL:
            gpio_bit_reset(LED_RUNNING_GPIO_PORT, LED_RUNNING_PIN);
            gpio_bit_reset(LED_BRAKE_GPIO_PORT, LED_BRAKE_PIN);
            break;
        default:
            break;
    }
}


void led_toggle(uint32_t gpio_periph,uint32_t pin)
{
    gpio_bit_write(gpio_periph, pin, (bit_status)(1-gpio_input_bit_get(gpio_periph, pin)));
}

