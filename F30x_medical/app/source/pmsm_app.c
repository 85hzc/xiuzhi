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
#if 0
/*!
    \brief      selftest rouine for power on operation 
    \param[in]  none
    \param[out] none
    \retval     none
*/
void startup_self_test(void)
{
    uint8_t i;
    float sample = 0;

    /* enable the clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_ADC0);
    rcu_periph_clock_enable(RCU_TIMER0);
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV4);

    timer_deinit(TIMER0);

    /* TIMER0 output -- CH0/CH1/CH2/CH0N/CH1N/CH2N */
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10);
    gpio_bit_set(GPIOA, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10);
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);
    gpio_bit_set(GPIOB, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);

    /* configure ADC pin, current sampling -- ADC_IN1(PA1) ADC_IN12(PC2) ADC_IN13(PC3) */
    gpio_init(I_U_PORT, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, I_U_PIN);
    gpio_init(I_V_PORT, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, I_V_PIN);
    //gpio_init(CURRENT_C_PORT, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, CURRENT_C_PIN);

    /* ADC scan mode function enable */
    adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);
    /* configure ADC data alignment */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
    /* configure ADC inserted channel trigger */
    adc_external_trigger_source_config(ADC0, ADC_INSERTED_CHANNEL, ADC0_1_2_EXTTRIG_INSERTED_NONE);
    adc_external_trigger_config(ADC0, ADC_INSERTED_CHANNEL, ENABLE);
    /* enable ADC interface */
    adc_enable(ADC0);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);
    /* configure ADC inserted channel */
    adc_channel_length_config(ADC0, ADC_INSERTED_CHANNEL, 1);
    adc_inserted_channel_config(ADC0, 0, VBUS_CHANNEL, ADC_SAMPLE_TIME);

    /* bus voltage check */
    for(i=0; i<20; i++){
        adc_software_trigger_enable(ADC0, ADC_INSERTED_CHANNEL);
        while(RESET == adc_flag_get(ADC0, ADC_FLAG_EOIC));
        adc_flag_clear(ADC0, ADC_FLAG_EOIC);
        sample += adc_inserted_data_read(ADC0, ADC_INSERTED_CHANNEL_0);
    }
    sample = sample*3.3f*16/4096/20;
    
    if((sample < INPUT_VOLTAGE_MIN) || (sample > INPUT_VOLTAGE_MAX)){
        //fault = FAULT_VOLTAGE;
        return;
    }
    
    /* configure ADC inserted channel */
    adc_channel_length_config(ADC0, ADC_INSERTED_CHANNEL, 3);
    adc_inserted_channel_config(ADC0, 0, I_U_CHANNEL, ADC_SAMPLE_TIME);
    adc_inserted_channel_config(ADC0, 1, I_V_CHANNEL, ADC_SAMPLE_TIME);
    //adc_inserted_channel_config(ADC0, 2, CURRENT_C_CHANNEL, ADC_SAMPLE_TIME);

    /* open the three lower leg switch */
    gpio_bit_reset(GPIOB, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);
    for(i=0; i<100; i++){
        adc_software_trigger_enable(ADC0, ADC_INSERTED_CHANNEL);
        while(RESET == adc_flag_get(ADC0, ADC_FLAG_EOIC));
        adc_flag_clear(ADC0, ADC_FLAG_EOIC);
        sample = adc_inserted_data_read(ADC0, ADC_INSERTED_CHANNEL_0);
        sample += adc_inserted_data_read(ADC0, ADC_INSERTED_CHANNEL_1);
        #if 1
        sample /= 2.0f;
        #else
        sample += adc_inserted_data_read(ADC0, ADC_INSERTED_CHANNEL_2);
        sample /= 3;
        #endif
        if((sample - CURRENT_AMP_SHIFT/3.3f*4096) > 800){
            //IR2136S_disable();
            motor_mos_all_close();
            //fault = FAULT_HARDWARE;
            return;
        }
    }

    /* open the three higher leg switch */
    gpio_bit_set(GPIOB, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);
    gpio_bit_reset(GPIOA, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10);
    for(i=0;i<100;i++){
        adc_software_trigger_enable(ADC0, ADC_INSERTED_CHANNEL);
        while(RESET == adc_flag_get(ADC0, ADC_FLAG_EOIC));
        adc_flag_clear(ADC0, ADC_FLAG_EOIC);

        sample = adc_inserted_data_read(ADC0, ADC_INSERTED_CHANNEL_0);
        sample += adc_inserted_data_read(ADC0, ADC_INSERTED_CHANNEL_1);
        sample += adc_inserted_data_read(ADC0, ADC_INSERTED_CHANNEL_2);
        sample /= 3;

        if((sample - CURRENT_AMP_SHIFT/3.3f*4096) > 800){
            motor_mos_all_close();
            //fault = FAULT_HARDWARE;
            return;
        }
    }

    motor_mos_all_close();

    /* reset the peripherals */
    rcu_periph_reset_enable(RCU_GPIOARST);
    rcu_periph_reset_enable(RCU_GPIOBRST);
    rcu_periph_reset_enable(RCU_GPIOCRST);
    rcu_periph_reset_enable(RCU_ADC0RST);
    rcu_periph_reset_enable(RCU_TIMER0RST);
    rcu_periph_reset_disable(RCU_GPIOARST);
    rcu_periph_reset_disable(RCU_GPIOBRST);
    rcu_periph_reset_disable(RCU_GPIOCRST);
    rcu_periph_reset_disable(RCU_ADC0RST);
    rcu_periph_reset_disable(RCU_TIMER0RST);

    /* close the clock */
    rcu_periph_clock_disable(RCU_GPIOA);
    rcu_periph_clock_disable(RCU_GPIOB);
    rcu_periph_clock_disable(RCU_GPIOC);
    rcu_periph_clock_disable(RCU_ADC0);
    rcu_periph_clock_disable(RCU_TIMER0);
}
#endif

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

#if (defined HALL_SENSOR_ENABLE)
    /* clear TIMER2 trigger interrupt flag */
    timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_TRG);
    timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_TRG|TIMER_INT_FLAG_CH3);
    /* enable TIMER2 */
    timer_enable(TIMER2);
#endif /* HALL_SENSOR_ENABLE  */

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
#if 0
/*!
    \brief      update the TIMER 3-phase output and sample point, the configuration works in the next period
    \param[in]  ta: the outputcompare point of phase-A
    \param[in]  tb: the outputcompare point of phase-B
    \param[in]  tc: the outputcompare point of phase-C
    \param[in]  tsample: the time point to sample the current
    \param[out] none
    \retval     none
*/
void update_timer(uint16_t ta, uint16_t tb, uint16_t tc, uint16_t tsample)
{
    timer_update_event_disable(TIMER0);
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, ta);
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_1, tb);
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_2, tc);
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_3, tsample);
    timer_update_event_enable(TIMER0);
}

/*!
    \brief      update the timer 3-phase output and sample point, the configuration works immediately
    \param[in]  ta: the outputcompare point of phase-A
    \param[in]  tb: the outputcompare point of phase-B
    \param[in]  tc: the outputcompare point of phase-C
    \param[in]  tsample: the time point to sample the current
    \param[out] none
    \retval     none
*/
void update_timer_immediate(uint16_t ta, uint16_t tb, uint16_t tc, uint16_t tsample)
{
    timer_update_event_disable(TIMER0);
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, ta);
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_1, tb);
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_2, tc);
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_3, tsample);
    timer_update_event_enable(TIMER0);
    timer_event_software_generate(TIMER0, TIMER_EVENT_SRC_UPG);
}
#endif
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



/*!
    \brief      power calculation
    \param[in]  none
    \param[out] none
    \retval     none
*/
void pmsm_power_calculation(int16_t vq, int16_t vd)
{
    motor.vq = (float)vq/32767*motor.torque;

    UTILS_LP_FAST(motor.vq_filter, motor.vq, 0.01f);
    arm_sqrt_f32((motor.vd_filter*motor.vd_filter + motor.vq_filter*motor.vq_filter), &motor.v_abs);

    /* real power */
    motor.P = fabs(motor.vq_filter*motor.iq_filter + motor.vd_filter*motor.id_filter)*1.5f;
    /* reactive power */
    motor.Q = fabs(motor.vq_filter*motor.id_filter - motor.vd_filter*motor.iq_filter)*1.5f;
    /* apparent power */
    arm_sqrt_f32((motor.P*motor.P + motor.Q*motor.Q), &motor.S);
}

