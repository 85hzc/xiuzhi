/*!
    \file    gd32f30x_it.c
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
#include "gd32f30x_it.h"

/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/

void NMI_Handler(void)
{
}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while (1){
    }
}

/*!
    \brief      this function handles MemManage exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MemManage_Handler(void)
{
    /* if Memory Manage exception occurs, go to infinite loop */
    while (1){
    }
}

/*!
    \brief      this function handles BusFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void BusFault_Handler(void)
{
    /* if Bus Fault exception occurs, go to infinite loop */
    while (1){
    }
}

/*!
    \brief      this function handles UsageFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void UsageFault_Handler(void)
{
    /* if Usage Fault exception occurs, go to infinite loop */
    while (1){
    }
}

/*!
    \brief      this function handles SVC exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SVC_Handler(void)
{
}

/*!
    \brief      this function handles DebugMon exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DebugMon_Handler(void)
{
}

/*!
    \brief      this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PendSV_Handler(void)
{
}

/*!
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/   
void SysTick_Handler(void)
{
    //static int16_t speed_filtered;

    periodTask_1ms();

    /* systick dealy process */
    delay_decrement();

    /* ADC sampling */
    //if(motor_flag.adc_ready_flag == 1){
    //    adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);
    //}

    /* the filtered speed */
    //UTILS_LP_FAST(speed_filtered, hall.speed, 0.05f);
}


/*!
    \brief      this function handles TIMER2 exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void TIMER2_IRQHandler(void)
{
    static uint8_t speed_timer_overflow = 0;

    if(timer_interrupt_flag_get(TIMER2, TIMER_INT_FLAG_CH3)){
        /* clear interrupt flag */
        timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_CH3);
        speed_timer_overflow++;
        hall.speed = 0;
    }
    
    if(timer_interrupt_flag_get(TIMER2, TIMER_INT_FLAG_TRG)){
        /* clear interrupt flag */
        timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_TRG);

        /* calculate the speed of the rotor */
        hall.speed = hall_speed_update(speed_timer_overflow);
        speed_timer_overflow = 0;
    }

    if(timer_interrupt_flag_get(TIMER2, TIMER_INT_FLAG_CH2)){
        /* clear interrupt flag */
        timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_CH2);
        timer_event_software_generate(TIMER2, TIMER_EVENT_SRC_UPG);
    }
}

#if 0
/*!
    \brief      this function handles ADC0 and ADC1 exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
//int16_t current_alpha, current_beta, current_q, current_d;
//int16_t voltage_d, voltage_q, voltage_alpha, voltage_beta;
//int16_t voltage_a, voltage_b, voltage_c;
//int32_t voltage_q_filtered, voltage_d_filtered;
//float voltage_q_amplitude;
//static uint8_t svpwm_flag = 1;
void ADC0_1_IRQHandler(void)
{
    /* clear the interrupt flag */
    adc_interrupt_flag_clear(ADC0, ADC_INT_EOIC);

    ebike_read_sample();

    adc_external_trigger_config(ADC0, ADC_INSERTED_CHANNEL, DISABLE);

    /* power calculation */
    //pmsm_power_calculation(voltage_q, voltage_d);

#if 0//def USART_DEBUG_ENABLE
    debug_data.data1 = position;
    debug_data.data2 = vbus;
    debug_data.data3 = torque_ebike_f;
    debug_data.data4 = caden;
    debug_usart_data_transfer(debug_data);
#endif /* USART_DEBUG_ENABLE */

    adc_external_trigger_config(ADC0, ADC_INSERTED_CHANNEL, ENABLE);
}

/*!
    \brief      this function handles DMA interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DMA0_Channel0_IRQHandler(void)
{
    dma_interrupt_flag_clear(DMA0, DMA_CH0, DMA_INT_FLAG_FTF);

    motor.torque = get_input_torque(adc_buffer[0]);
}

/*!
    \brief      this function handles EXTI4 interrupts handler
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI4_IRQHandler(void)
{
    led_toggle(LED_RUNNING_GPIO_PORT, LED_RUNNING_PIN);

    /* Port A: DATA */
    if(exti_interrupt_flag_get(EXTI_6) != RESET){
        exti_interrupt_flag_clear(EXTI_6);

        led_toggle(LED_RUNNING_GPIO_PORT, LED_RUNNING_PIN);

        joggle_delay(10000);
        if(RESET == gpio_input_bit_get(TOR_IN1_GPIO_PORT, TOR_IN1_GPIO_PIN)){
            trige_count();
        }else{
        }
    }
}
#endif

/*!
    \brief      this function handles EXTI5_9 interrupts handler
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI5_9_IRQHandler(void)
{
    /* Port IN1 */ //踏频信号
    if(exti_interrupt_flag_get(EXTI_6) != RESET){
        exti_interrupt_flag_clear(EXTI_6);
        joggle_delay(10000);
        if(SET == gpio_input_bit_get(TOR_IN1_GPIO_PORT, TOR_IN1_GPIO_PIN)){
            led_toggle(LED_BRAKE_GPIO_PORT, LED_BRAKE_PIN);
            trige_count();
        }else{
        }
    }

    /* Port IN2 */ //限位控制
    /*if(exti_interrupt_flag_get(EXTI_7) != RESET){
        exti_interrupt_flag_clear(EXTI_7);
        joggle_delay(10000);
        if(RESET == gpio_input_bit_get(PROTECT_IN2_GPIO_PORT, PROTECT_IN2_GPIO_PIN)){

            bldc_brake();
            g_generator_gears_pre = GENERATOR_5;
            g_generator_gears = GENERATOR_5;
        }else{
        }
    }*/

    /* Port TP2 */ //轮速信号
    if(exti_interrupt_flag_get(EXTI_9) != RESET){
        exti_interrupt_flag_clear(EXTI_9);
        joggle_delay(10000);
        if(SET == gpio_input_bit_get(VELOCITY_GPIO_PORT, VELOCITY_GPIO_PIN)){
            led_toggle(LED_RUNNING_GPIO_PORT, LED_RUNNING_PIN);
            g_velocity_count++;
        }else{
        }
    }
}


/*!
    \brief      this function handles USART2 exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USART2_IRQHandler(void)
{
    if(RESET != usart_interrupt_flag_get(USART2, USART_INT_FLAG_RBNE)){
        /* read one byte from the receive data register */
        rx_buffer[rx_counter++] = (uint8_t)usart_data_receive(USART2);

        //\0X0d\0X0a
        if(rx_counter >= COM_BUFFER_SIZE){
            memcpy(rx_buffer_app, rx_buffer, rx_counter);
            rx_counter = 0;
            rx_ok = 1;
        }
    }
}


/*!
    \brief      debug function handles USART1 exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USART1_IRQHandler(void)
{
    if(RESET != usart_interrupt_flag_get(USART1, USART_INT_FLAG_RBNE)){
        /* read one byte from the receive data register */
        debug_rx_buffer[debug_rx_counter++] = (uint8_t)usart_data_receive(USART1);

        //\0X0d\0X0a
        if(debug_rx_counter >= USART1_BUFFER_SIZE){

            memcpy(debug_rx_buffer_app, debug_rx_buffer, debug_rx_counter);
            debug_rx_counter = 0;
            debug_rx_ok = 1;
        }
    }
}

