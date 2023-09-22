/*!
    \file    pmsm.c
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

/* static variables */
static int16_t angle;
static uint16_t hall_sequence[6] = {0};

/*!
    \brief      hall timer configuration
    \param[in]  none
    \param[out] none
    \retval     none
*/
void hall_timer_config(void)
{
    timer_parameter_struct timer_initpara;
    timer_ic_parameter_struct timer_icinitpara;
    timer_oc_parameter_struct timer_ocintpara;

    /* enable the clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    //rcu_periph_clock_enable(RCU_GPIOB);
    /* TIMER2 hall input--CH0/CH1/CH2 */ 
    gpio_init(GPIOA, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    //gpio_init(GPIOA, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
    //gpio_init(GPIOB, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_0);

    /* configure TIMER2 interrupt priority */
    nvic_irq_enable(TIMER2_IRQn, 1, 1);

    /* enable the clock */
    rcu_periph_clock_enable(RCU_TIMER2);

    timer_deinit(TIMER2);
    timer_initpara.prescaler         = SYSTEM_FREQUENCE/1000000-1;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 50000;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV4;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER2, &timer_initpara);

    timer_icinitpara.icpolarity  = TIMER_IC_POLARITY_RISING;
    timer_icinitpara.icselection = TIMER_IC_SELECTION_ITS;
    timer_icinitpara.icprescaler = TIMER_IC_PSC_DIV1;
    timer_icinitpara.icfilter    = 15;
    timer_input_capture_config(TIMER2, TIMER_CH_0, &timer_icinitpara);
    //TIMER2 CH0
    timer_hall_mode_config(TIMER2, TIMER_HALLINTERFACE_ENABLE);
    /* input trigger choose, TIMER internal trigger: TI0 Edge Detector */
    timer_input_trigger_source_select(TIMER2, TIMER_SMCFG_TRGSEL_CI0F_ED);
    
    timer_ocintpara.outputstate  = TIMER_CCX_DISABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_channel_output_config(TIMER2, TIMER_CH_3, &timer_ocintpara);
    
    timer_channel_output_pulse_value_config(TIMER2, TIMER_CH_3, 50000);
    timer_channel_output_mode_config(TIMER2, TIMER_CH_3, TIMER_OC_MODE_TIMING);

    /* slave mode: reset mode */
    timer_slave_mode_select(TIMER2, TIMER_SLAVE_MODE_RESTART);
    timer_master_slave_mode_config(TIMER2, TIMER_MASTER_SLAVE_MODE_ENABLE);
    /* select TIMER master mode output trigger source: O1CPRE as trigger output */
    timer_master_output_trigger_source_select(TIMER2, TIMER_TRI_OUT_SRC_O1CPRE);

    timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_TRG);
    timer_interrupt_enable(TIMER2, TIMER_INT_TRG);
    
    timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_CH3);
    timer_interrupt_enable(TIMER2, TIMER_INT_CH3);
}

/*!
    \brief      hall control mode, rotor location initialization
    \param[in]  none
    \param[out] none
    \retval     none
*/
void hall_init(void)
{
    hall.pre_state = 0;
    hall.speed = 0;
}

/*!
    \brief      hall control mode, read hall state
    \param[in]  none
    \param[out] none
    \retval     hall state, 1-6
*/
uint8_t hall_read_state(void)
{
    uint8_t state;
    state  = (uint8_t)((GPIO_ISTAT(GPIOA) & 0x000000C0) >> 6);
    state |= (uint8_t)((GPIO_ISTAT(GPIOB) & 0x00000001) << 2);
    
    return (state);
}


/*!
    \brief      hall control mode, update motor speed, which is expressed in revolutions per second
    \param[in]  none
    \param[out] none
    \retval     motor mechanical speed, 0-4000
*/
uint16_t hall_speed_update(uint8_t overflow_count)
{
    uint32_t timer2_count, speed_temp;

    timer2_count = (timer_channel_capture_value_register_read(TIMER2, TIMER_CH_0) + (overflow_count) * 50000);

    speed_temp = 1000000.0f * 60 /timer2_count /(NUMBER_OF_PAIRS*6);
    utils_add_sample(speed_samples, speed_temp, SPEED_LEN_BITS, &speed_index);
    speed_temp = utils_average_sample(speed_samples, SPEED_LEN_BITS);
    
    //if(speed_temp>6000)
    //    speed_temp = 0;
    
    return (speed_temp);
}

