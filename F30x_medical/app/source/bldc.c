/*!
    \file    blcd.c
    \brief   L9110S driver
*/


#include "global.h"

/* static function declaration */
void led_on(mc_led led);
void led_off(mc_led led);
#if 0
/*!
    \brief      initialize the GPIO port of led
    \param[in]  led
    \param[out] none
    \retval     none
*/
void bldc_init(void)
{
    /* enable the clock of GPIO */
    rcu_periph_clock_enable(MOTOR_A_GPIO_CLK);
    rcu_periph_clock_enable(MOTOR_B_GPIO_CLK);
    
    /* bldc L9110S */
    gpio_init(MOTOR_A_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, MOTOR_A_PIN);
    gpio_init(MOTOR_B_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, MOTOR_B_PIN);
    gpio_bit_reset(MOTOR_A_GPIO_PORT, MOTOR_A_PIN);
    gpio_bit_reset(MOTOR_B_GPIO_PORT, MOTOR_B_PIN);
}

void position_exit_init(void)
{
    /* enable the clock of GPIO */
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);

    /* configure button pin A as input */
    gpio_init(PROTECT_IN2_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, PROTECT_IN2_GPIO_PIN);

    /* connect key EXTI line to key GPIO pin */
    gpio_exti_source_select(PROTECT_IN2_EXTI_SRC_PORT, PROTECT_IN2_EXTI_SRC_PIN);

    /* configure key EXTI line */
    exti_init(PROTECT_IN2_EXTI_LINE, EXTI_INTERRUPT, PROTECT_IN2_EXTI_LINE_EDGE);
    exti_interrupt_flag_clear(PROTECT_IN2_EXTI_LINE);

    /* enable and set key EXTI interrupt to the lowest priority */
    nvic_irq_enable(PROTECT_IN2_EXTI_IRQ, 2U, 0U);
}
#endif

