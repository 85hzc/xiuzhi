/*!
    \file    app_exti.c
    \brief   exti driver file
*/


#include "global.h"

static uint16_t delay_joggle = 0;
uint16_t cup_count, cup_flag; //统计数量 （杯）

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
    gpio_init(N1S_LUOBEI_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_2MHZ, N1S_LUOBEI_GPIO_PIN);

    /* connect key EXTI line to key GPIO pin */
    gpio_exti_source_select(N1S_LUOBEI_EXTI_SRC_PORT, N1S_LUOBEI_EXTI_SRC_PIN);

    /* configure key EXTI line */
    exti_init(N1S_LUOBEI_EXTI_LINE, EXTI_INTERRUPT, N1S_LUOBEI_EXTI_LINE_EDGE);
    exti_interrupt_flag_clear(N1S_LUOBEI_EXTI_LINE);

    /* enable and set key EXTI interrupt to the lowest priority */
    nvic_irq_enable(N1S_LUOBEI_EXTI_IRQ, 2U, 0U);
}

void position2_exit_init(void)
{
    /* enable the clock of GPIO */
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);

    /* configure button pin A as input */
    gpio_init(N2S_ZHUSHUI_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_2MHZ, N2S_ZHUSHUI_GPIO_PIN);

    /* connect key EXTI line to key GPIO pin */
    gpio_exti_source_select(N2S_ZHUSHUI_EXTI_SRC_PORT, N2S_ZHUSHUI_EXTI_SRC_PIN);

    /* configure key EXTI line */
    exti_init(N2S_ZHUSHUI_EXTI_LINE, EXTI_INTERRUPT, N2S_ZHUSHUI_EXTI_LINE_EDGE);
    exti_interrupt_flag_clear(N2S_ZHUSHUI_EXTI_LINE);

    /* enable and set key EXTI interrupt to the lowest priority */
    nvic_irq_enable(N2S_ZHUSHUI_EXTI_IRQ, 2U, 0U);
}

void position3_exit_init(void)
{
    /* enable the clock of GPIO */
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);

    /* configure button pin A as input */
    gpio_init(N3S_CHUBEI_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_2MHZ, N3S_CHUBEI_GPIO_PIN);

    /* connect key EXTI line to key GPIO pin */
    gpio_exti_source_select(N3S_CHUBEI_EXTI_SRC_PORT, N3S_CHUBEI_EXTI_SRC_PIN);

    /* configure key EXTI line */
    exti_init(N3S_CHUBEI_EXTI_LINE, EXTI_INTERRUPT, N3S_CHUBEI_EXTI_LINE_EDGE);
    exti_interrupt_flag_clear(N3S_CHUBEI_EXTI_LINE);

    /* enable and set key EXTI interrupt to the lowest priority */
    nvic_irq_enable(N3S_CHUBEI_EXTI_IRQ, 2U, 0U);
}


void velocity_exti_init(void)
{
    /* enable the clock of GPIO */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_AF);

    /* configure button pin A as input */
    gpio_init(VELOCITY_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, VELOCITY_GPIO_PIN);

    /* connect key EXTI line to key GPIO pin */
    gpio_exti_source_select(VELOCITY_EXTI_SRC_PORT, VELOCITY_EXTI_SRC_PIN);

    /* configure key EXTI line */
    exti_init(VELOCITY_EXTI_LINE, EXTI_INTERRUPT, VELOCITY_EXTI_LINE_EDGE);
    exti_interrupt_flag_clear(VELOCITY_EXTI_LINE);

    /* enable and set key EXTI interrupt to the lowest priority */
    nvic_irq_enable(VELOCITY_EXTI_IRQ, 2U, 0U);
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
    \brief      落杯数量统计
    \param[in]  num
    \param[out] none
    \retval     none
*/
void trige_count(void)
{
    cup_count++;
    cup_flag = 1;
}


