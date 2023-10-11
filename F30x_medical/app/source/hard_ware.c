/*!
    \file    hardware.c
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

/* GPIO configuration */
static void gpio_config(void);
/* RCU configuration */
static void rcu_config(void);
/* NVIC configuration */
static void nvic_config(void);
/* ADC configuration */
static void adc_config(void);


/*!
    \brief      hardware configuration
    \param[in]  none
    \param[out] none
    \retval     none
*/
void hardware_config()
{ 
    rcu_config();
    nvic_config();
    gpio_config();

    timer7_config();
    adc_config();
    i2c_init();

    debug_init();
    usart2_init();
}

/*!
    \brief      RCU configuration
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void rcu_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_AF);
    
    //rcu_periph_clock_enable(RCU_TIMER0);
    rcu_periph_clock_enable(RCU_TIMER7);

    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV6);
    rcu_periph_clock_enable(RCU_ADC0);
}

/*!
    \brief      GPIO configuration
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void gpio_config(void)
{
    //加热丝  导通电流PWM控制
    /*Configure PC6(TIMER7_CH0) as alternate function*/
    gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);


    /* configure ADC pin, temperature sampling -- ADC_IN0(PA0) */
    gpio_init(TEMPERATURE_PORT, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, TEMPERATURE_PIN);


    /* SWD remap */
    gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
}

/*!
    \brief      NVIC configuration
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void nvic_config(void)
{
    /* configure the NVIC Preemption Priority Bits */
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);

    /* configure ADC0 interrupt priority */
    //nvic_irq_enable(ADC0_1_IRQn, 0, 0);

    /* USART interrupt configuration */
    nvic_irq_enable(USART1_IRQn, 0, 0);
    nvic_irq_enable(USART2_IRQn, 0, 0);
    nvic_irq_enable(RTC_IRQn,1,0);
}

/*!
    \brief      TIMER7 configuration, generator control timer
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer7_config(void)
{
    /*  TIMER7 configuration: generate PWM signals with different duty cycles:
        TIMER7CLK = SystemCoreClock / 120 = 1MHz */
    timer_parameter_struct       timer_initpara;
    timer_oc_parameter_struct    timer_ocintpara;
    timer_deinit(TIMER7);

    timer_initpara.prescaler         = 119;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 500;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER7, &timer_initpara);

    /* TIMER7 output disable -- CH0 */
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER7, TIMER_CH_0, &timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER7, TIMER_CH_0, 250);
    timer_channel_output_mode_config(TIMER7, TIMER_CH_0, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER7, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);

    timer_primary_output_config(TIMER7, ENABLE);
    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER7);
    timer_enable(TIMER7);
}

/*!
    \brief      ADC configuration, current sampling
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void adc_config(void)
{
    /* configure ADC mode */
    adc_mode_config(ADC_MODE_FREE);
    /* ADC scan mode function enable */
    adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);

    /* configure ADC data alignment */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);

    adc_interrupt_enable(ADC0, ADC_INT_EOIC);   
    /* enable ADC interface */
    adc_enable(ADC0);

    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);
    //torque_value_calibration();

    /* configure ADC inserted channel trigger */
    adc_external_trigger_source_config(ADC0, ADC_INSERTED_CHANNEL, ADC0_1_2_EXTTRIG_INSERTED_NONE);

    /* ADC external trigger enable */
    adc_external_trigger_config(ADC0, ADC_INSERTED_CHANNEL, ENABLE);

    /* configure ADC inserted channel */
    adc_channel_length_config(ADC0, ADC_INSERTED_CHANNEL, 1);
    adc_inserted_channel_config(ADC0, 0, TEMPERATURE_CHANNEL, ADC_SAMPLE_TIME);           //CH9
    //adc_inserted_channel_config(ADC0, 1, TORQUE_SENSOR_CHANNEL, ADC_SAMPLE_TIME);   //CH0
    //adc_inserted_channel_config(ADC0, 2, VBUS_CHANNEL, ADC_SAMPLE_TIME);            //CH1

    //adc_dma_mode_enable(ADC0);
    //motor_flag.adc_ready_flag = 1;
}

