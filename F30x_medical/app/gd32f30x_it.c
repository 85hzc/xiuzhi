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
    periodTask_1ms();

    /* systick dealy process */
    delay_decrement();

    if (state_enzyme_count_running)
        check_enzyme_timer();

    if (state_qubei_count_running)
        check_qubei_timer();
    
    if (state_position_error_count_running)
        check_position_error_timer();

    if (state_temperature_error_count_running)
        check_temperature_error_timer();

    if (state_jiazhu_error_count_running)
        check_jiazhu_error_timer();

}


/*!
    \brief      this function handles EXTI5_9 interrupts handler
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI5_9_IRQHandler(void)
{
    //出杯位置，限位控制开关中断
    if(exti_interrupt_flag_get(EXTI_5) != RESET){
        exti_interrupt_flag_clear(EXTI_5);
        joggle_delay(10000);
        if(read_chubei_protect_position_switch()){
            g_exti_chubei_protet_position_flag = 1;
        }
    }

    //限位控制开关中断
    if(exti_interrupt_flag_get(EXTI_6) != RESET){
        exti_interrupt_flag_clear(EXTI_6);
        joggle_delay(10000);
        if(read_qibei_position_switch()){
            led_toggle(LED_RUNNING_GPIO_PORT, LED_RUNNING_PIN);
            g_exti_qibei_position_flag = 1;
        }
    }

    //落杯信号中断
    if(exti_interrupt_flag_get(EXTI_7) != RESET){
        exti_interrupt_flag_clear(EXTI_7);
        joggle_delay(10000);
        if(SET == gpio_input_bit_get(LUOBEI_IRQ_GPIO_PORT, LUOBEI_IRQ_GPIO_PIN)){
            led_toggle(LED_BRAKE_GPIO_PORT, LED_BRAKE_PIN);
            trige_count();
            luobei_retry += 1;   //完成一次落杯操作，作为二次尝试落杯判断依据（如果空杯情况）
            //luobei_motor_stop();
            lcd_update_flag = 1;
        }
    }

    //脚踏开关信号中断
    if(exti_interrupt_flag_get(EXTI_8) != RESET){
        exti_interrupt_flag_clear(EXTI_8);
        joggle_delay(10000);
        if(SET == gpio_input_bit_get(PEDAL_GPIO_PORT, PEDAL_GPIO_PIN)){
            led_toggle(LED_BRAKE_GPIO_PORT, LED_BRAKE_PIN);
            start_work = 1;
        }
    }

    //流量信号
    if(exti_interrupt_flag_get(EXTI_9) != RESET){
        exti_interrupt_flag_clear(EXTI_9);
        joggle_delay(10000);
        if(SET == gpio_input_bit_get(VELOCITY_GPIO_PORT, VELOCITY_GPIO_PIN)){
            led_toggle(LED_BRAKE_GPIO_PORT, LED_BRAKE_PIN);
            g_water_count++;

            if (g_water_count > water_count_signals) {
                water_motor_stop();
                exti_interrupt_disable(VELOCITY_EXTI_LINE);
                g_water_count = 0;
                state_water_ok = 1;
            }
        }
    }
}



/*!
    \brief      this function handles EXTI10_15 interrupts handler
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI10_15_IRQHandler(void)
{
    //位置1中断
    if(exti_interrupt_flag_get(EXTI_10) != RESET){
        exti_interrupt_flag_clear(EXTI_10);
        joggle_delay(10000);
        if(read_luobei_position_switch()){
            led_toggle(LED_RUNNING_GPIO_PORT, LED_RUNNING_PIN);
            g_exti_luobei_position_flag = 1;
            exti_interrupt_disable(N1S_LUOBEI_EXTI_LINE);
        }
    }

    //位置2中断
    if(exti_interrupt_flag_get(EXTI_11) != RESET){
        exti_interrupt_flag_clear(EXTI_11);
        joggle_delay(10000);
        if(read_zhushui_position_switch()){
            led_toggle(LED_RUNNING_GPIO_PORT, LED_RUNNING_PIN);
            g_exti_zhushui_position_flag = 1;
            exti_interrupt_disable(N2S_ZHUSHUI_EXTI_LINE);
        }
    }

    //位置3中断
    if(exti_interrupt_flag_get(EXTI_12) != RESET){
        exti_interrupt_flag_clear(EXTI_12);
        joggle_delay(10000);
        if(read_chubei_position_switch()){
            led_toggle(LED_RUNNING_GPIO_PORT, LED_RUNNING_PIN);
            g_exti_chubei_position_flag = 1;
            exti_interrupt_disable(N3S_CHUBEI_EXTI_LINE);
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
        /* clear flag first */
        usart_interrupt_flag_clear(USART2, USART_INT_FLAG_RBNE);
        uint8_t ch = (uint8_t)usart_data_receive(USART2);
        buffer_write(ch);
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

#if 0
/*!
    \brief      this function handles RTC global interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void RTC_IRQHandler(void)
{
    if (rtc_flag_get(RTC_FLAG_SECOND) != RESET){
        /* clear the RTC second interrupt flag*/
        rtc_flag_clear(RTC_FLAG_SECOND);

        //led_toggle(LED_BRAKE_GPIO_PORT, LED_BRAKE_PIN);
        /* wait until last write operation on RTC registers has finished */
        rtc_lwoff_wait();
        /* reset RTC counter when time is 23:59:59 */
        if (rtc_counter_get() >= TIME_23_59_59){
            rtc_counter_set(rtc_counter_get() % TIME_23_59_59);
            /* wait until last write operation on RTC registers has finished */
            rtc_lwoff_wait();
        }
    }
}
#endif

/**
  * @brief  This function handles TIMER1 interrupt request.
  * @param  None
  * @retval None
  */
void TIMER1_IRQHandler(void)
{
    if(SET == timer_interrupt_flag_get(TIMER1, TIMER_INT_UP)) {
        /* clear update interrupt bit */
        timer_interrupt_flag_clear(TIMER1, TIMER_INT_UP);
        /* toggle selected led */
        //led_toggle(LED_BRAKE_GPIO_PORT, LED_BRAKE_PIN);
        lcd_display_inform();
    }
}

