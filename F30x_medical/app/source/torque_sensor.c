/*!
    \file    torque_sensor.c
    \brief   torque driver file
*/


#include "global.h"

static uint16_t delay_joggle = 0;
static uint8_t pulse_count = 0, pulse_count_pre = 0;
uint8_t cadence = 0, cadence_f = 0;

/*!
    \brief      initialize the TOR port
    \param[in]  none
    \param[out] none
    \retval     none
*/
void torque_exti_init(void)
{
    /* enable the clock of GPIO */
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);

    /* configure button pin A as input */
    gpio_init(TOR_IN1_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, TOR_IN1_GPIO_PIN);

    /* connect key EXTI line to key GPIO pin */
    gpio_exti_source_select(TOR_IN1_EXTI_SRC_PORT, TOR_IN1_EXTI_SRC_PIN);

    /* configure key EXTI line */
    exti_init(TOR_IN1_EXTI_LINE, EXTI_INTERRUPT, TOR_IN1_EXTI_LINE_EDGE);
    exti_interrupt_flag_clear(TOR_IN1_EXTI_LINE);

    /* enable and set key EXTI interrupt to the lowest priority */
    nvic_irq_enable(TOR_IN1_EXTI_IRQ, 2U, 0U);
}

/*!
    \brief      delay stabilization
    \param[in]  num
    \param[out] none
    \retval     none
*/
void joggle_delay(uint16_t num)
{
    for(delay_joggle=0; delay_joggle<num; delay_joggle++);
}


/*!
    \brief      count at one effective edge
    \param[in]  num
    \param[out] none
    \retval     none
*/
void trige_count(void)
{
    if (pulse_count == 0xff) {

        pulse_count = (0xff - pulse_count_pre) + 1;

    } else {
        pulse_count++;
    }
}

void calc_cadence()
{
    /* 传感器圈36个脉冲 */
    cadence = ( ((float)pulse_count * 60.0f) / (36.0f * 0.5f) );
    HZC_LP_FAST(cadence_f, cadence, 0.5f);

    pulse_count_pre = pulse_count;
    pulse_count = 0;
}

