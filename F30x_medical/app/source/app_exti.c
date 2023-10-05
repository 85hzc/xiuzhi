/*!
    \file    app_exti.c
    \brief   exti driver file
*/


#include "global.h"

static uint16_t delay_joggle = 0;
uint16_t cup_count, water_set;
uint8_t temperature_set, enyzme_set;

/*!
    \brief      initialize the TOR port
    \param[in]  none
    \param[out] none
    \retval     none
*/
void luobei_exti_init(void)
{
    /* enable the clock of GPIO */
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);

    /* configure button pin A as input */
    gpio_init(LUOBEI_IRQ_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_2MHZ, LUOBEI_IRQ_GPIO_PIN);

    /* connect key EXTI line to key GPIO pin */
    gpio_exti_source_select(LUOBEI_IRQ_EXTI_SRC_PORT, LUOBEI_IRQ_EXTI_SRC_PIN);

    /* configure key EXTI line */
    exti_init(LUOBEI_IRQ_EXTI_LINE, EXTI_INTERRUPT, LUOBEI_IRQ_EXTI_LINE_EDGE);
    exti_interrupt_flag_clear(LUOBEI_IRQ_EXTI_LINE);

    /* enable and set key EXTI interrupt to the lowest priority */
    nvic_irq_enable(LUOBEI_IRQ_EXTI_IRQ, 2U, 0U);
}

void protect_exit_init(void)
{
    /* enable the clock of GPIO */
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);

    /* configure button pin A as input */
    gpio_init(PROTECT_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_2MHZ, PROTECT_GPIO_PIN);

    /* connect key EXTI line to key GPIO pin */
    gpio_exti_source_select(PROTECT_EXTI_SRC_PORT, PROTECT_EXTI_SRC_PIN);

    /* configure key EXTI line */
    exti_init(PROTECT_EXTI_LINE, EXTI_INTERRUPT, PROTECT_EXTI_LINE_EDGE);
    exti_interrupt_flag_clear(PROTECT_EXTI_LINE);

    /* enable and set key EXTI interrupt to the lowest priority */
    nvic_irq_enable(PROTECT_EXTI_IRQ, 2U, 0U);
}


void position1_exit_init(void)
{
    /* enable the clock of GPIO */
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);

    /* configure button pin A as input */
    gpio_init(POSITION_1_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, POSITION_1_GPIO_PIN);

    /* connect key EXTI line to key GPIO pin */
    gpio_exti_source_select(POSITION_1_EXTI_SRC_PORT, POSITION_1_EXTI_SRC_PIN);

    /* configure key EXTI line */
    exti_init(POSITION_1_EXTI_LINE, EXTI_INTERRUPT, POSITION_1_EXTI_LINE_EDGE);
    exti_interrupt_flag_clear(POSITION_1_EXTI_LINE);

    /* enable and set key EXTI interrupt to the lowest priority */
    nvic_irq_enable(POSITION_1_EXTI_IRQ, 2U, 0U);
}

void position2_exit_init(void)
{
    /* enable the clock of GPIO */
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);

    /* configure button pin A as input */
    gpio_init(POSITION_2_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, POSITION_2_GPIO_PIN);

    /* connect key EXTI line to key GPIO pin */
    gpio_exti_source_select(POSITION_2_EXTI_SRC_PORT, POSITION_2_EXTI_SRC_PIN);

    /* configure key EXTI line */
    exti_init(POSITION_2_EXTI_LINE, EXTI_INTERRUPT, POSITION_2_EXTI_LINE_EDGE);
    exti_interrupt_flag_clear(POSITION_2_EXTI_LINE);

    /* enable and set key EXTI interrupt to the lowest priority */
    nvic_irq_enable(POSITION_2_EXTI_IRQ, 2U, 0U);
}

void position3_exit_init(void)
{
    /* enable the clock of GPIO */
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);

    /* configure button pin A as input */
    gpio_init(POSITION_3_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, POSITION_3_GPIO_PIN);

    /* connect key EXTI line to key GPIO pin */
    gpio_exti_source_select(POSITION_3_EXTI_SRC_PORT, POSITION_3_EXTI_SRC_PIN);

    /* configure key EXTI line */
    exti_init(POSITION_3_EXTI_LINE, EXTI_INTERRUPT, POSITION_3_EXTI_LINE_EDGE);
    exti_interrupt_flag_clear(POSITION_3_EXTI_LINE);

    /* enable and set key EXTI interrupt to the lowest priority */
    nvic_irq_enable(POSITION_3_EXTI_IRQ, 2U, 0U);
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
    cup_count++;
}

void calc_cadence()
{

}

