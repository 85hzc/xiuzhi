/*!
    \file    pmsm_app.c
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

static void motor_mos_all_open(void);
static void motor_mos_all_close(void);
static void motor_patial_brake(uint8_t duty);
static void motor_full_brake(void);
static void parameter_clear(void);
uint16_t mos_polarity = 0;

/*!
    \brief      start motor
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void parameter_clear(void)
{
    /* clear parameters */
    hall.speed = 0;
    adc_reference_filtered = 0;
    torque_reference = 0;
    utils_sample_init();
}
/*!
    \brief      start motor
    \param[in]  none
    \param[out] none
    \retval     none
*/
void motor_start(void)
{
    hall_init();
    

    /* enable the channels of TIMER */
    motor_mos_all_open();

    timer_enable(TIMER0);
    timer_event_software_generate(TIMER0, TIMER_EVENT_SRC_UPG);
}

/*!
    \brief      stop motor
    \param[in]  none
    \param[out] none
    \retval     none
*/
void motor_stop(void)
{
    /* disable TIMER0 three-phase output */
    motor_mos_all_close();
    
    /* disable TIMER0 */
    timer_disable(TIMER0);

    /*disable TIMER2 interrupt */
    timer_interrupt_disable (TIMER2,TIMER_INT_TRG|TIMER_INT_CH3);
    /* disable TIMER2 */
    timer_disable(TIMER2);
    /* clear TIMER2 counter */
    timer_counter_value_config(TIMER2, 0);

    /* clear parameters */
    parameter_clear();
}

/*!
    \brief      open all the 6 mosfet
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void motor_mos_all_open(void)
{
    timer_channel_output_state_config(TIMER0, TIMER_CH_0, TIMER_CCX_ENABLE);
    timer_channel_output_state_config(TIMER0, TIMER_CH_1, TIMER_CCX_ENABLE);
    timer_channel_output_state_config(TIMER0, TIMER_CH_2, TIMER_CCX_ENABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_0, TIMER_CCXN_ENABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_1, TIMER_CCXN_ENABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_2, TIMER_CCXN_ENABLE);
}

/*!
    \brief      close all the 6 mosfet
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void motor_mos_all_close(void)
{
    timer_channel_output_state_config(TIMER0, TIMER_CH_0, TIMER_CCX_DISABLE);
    timer_channel_output_state_config(TIMER0, TIMER_CH_1, TIMER_CCX_DISABLE);
    timer_channel_output_state_config(TIMER0, TIMER_CH_2, TIMER_CCX_DISABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_0, TIMER_CCXN_DISABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_1, TIMER_CCXN_DISABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_2, TIMER_CCXN_DISABLE);
}

/*!
    \brief      the low-side mosfets output the fixed duty pwm
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void motor_patial_brake(uint8_t duty)
{
    update_timer_duty(duty);
    timer_channel_output_state_config(TIMER0, TIMER_CH_0, TIMER_CCX_DISABLE);
    timer_channel_output_state_config(TIMER0, TIMER_CH_1, TIMER_CCX_DISABLE);
    timer_channel_output_state_config(TIMER0, TIMER_CH_2, TIMER_CCX_DISABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_0, TIMER_CCXN_ENABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_1, TIMER_CCXN_ENABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_2, TIMER_CCXN_ENABLE);
}

/*!
    \brief      the low-side mosfets open to brake the motor
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void motor_full_brake(void)
{
    timer_channel_output_state_config(TIMER0, TIMER_CH_0, TIMER_CCX_DISABLE);
    timer_channel_output_state_config(TIMER0, TIMER_CH_1, TIMER_CCX_DISABLE);
    timer_channel_output_state_config(TIMER0, TIMER_CH_2, TIMER_CCX_DISABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_0, TIMER_CCXN_DISABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_1, TIMER_CCXN_DISABLE);
    timer_channel_complementary_output_state_config(TIMER0, TIMER_CH_2, TIMER_CCXN_DISABLE);
    
    mos_polarity = TIMER_CHCTL2(TIMER0);
    
    TIMER_CHCTL2(TIMER0) ^= 0x0888;
}

/*!
    \brief      set PWM frequency
    \param[in]  frequency: the frequence to set
    \param[out] none
    \retval     none
*/
void update_period(uint16_t frequency)
{
    frequency_now = frequency;

    pwm_top = SYSTEM_FREQUENCE / (frequency_now*2);
    
    timer_autoreload_value_config(TIMER0, pwm_top);
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_3, pwm_top-4);
}

/*!
    \brief      zero voltage vector output
    \param[in]  duty: the duration of all the high-side switch in the three-phase source inverter open
    \param[out] none
    \retval     none
*/
void update_timer_duty(uint8_t duty)
{
    uint16_t pwm_set;

    pwm_set = pwm_top - pwm_top * duty / 100;

    timer_update_event_disable(TIMER0);

    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, pwm_set);
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_1, pwm_set);
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_2, pwm_set);

    timer_update_event_enable(TIMER0);
}

/*!
    \brief      update the current sample point
    \param[in]  duty: the duration of all the high-side switch in the three-phase source inverter open
    \param[out] none
    \retval     none
*/
void update_timer_sample(uint16_t tsample)
{
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_3, tsample);
}

/*!
    \brief      the bus voltage mesurement
    \param[in]  none
    \param[out] none
    \retval     the bus voltage
*/
float get_input_torque(uint16_t data)
{
    float voltage;

    voltage = data*3.3f*16/(4096);

    return (voltage);
}

