/*!
    \file    blcd.c
    \brief   L9110S driver
*/


#include "global.h"

/* static function declaration */
void led_on(mc_led led);
void led_off(mc_led led);

/*!
    \brief      initialize the GPIO port of led
    \param[in]  led
    \param[out] none
    \retval     none
*/
void luobei_motor_init(void)
{
    /* enable the clock of GPIO */
    rcu_periph_clock_enable(MOTOR_LUOBEI_GPIO_CLK);
    //rcu_periph_clock_enable(MOTOR_LUOBEI_GPIO_CLK);
    
    /* bldc */
    gpio_init(MOTOR_LUOBEI_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, MOTOR_LUOBEI_PIN);
    //gpio_init(MOTOR_LUOBEI_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, MOTOR_LUOBEI_PIN);
    gpio_bit_reset(MOTOR_LUOBEI_GPIO_PORT, MOTOR_LUOBEI_PIN);
    //gpio_bit_reset(MOTOR_LUOBEI_GPIO_PORT, MOTOR_LUOBEI_PIN);
}

void luobei_motor_start(void)
{
    /* bldc */
    gpio_bit_set(MOTOR_LUOBEI_GPIO_PORT, MOTOR_LUOBEI_PIN);
    //gpio_bit_set(MOTOR_LUOBEI_GPIO_PORT, MOTOR_LUOBEI_PIN);
}

void luobei_motor_stop(void)
{
    /* bldc */
    gpio_bit_reset(MOTOR_LUOBEI_GPIO_PORT, MOTOR_LUOBEI_PIN);
    //gpio_bit_reset(MOTOR_LUOBEI_GPIO_PORT, MOTOR_LUOBEI_PIN);
}

void water_motor_init(void)
{
    /* enable the clock of GPIO */
    rcu_periph_clock_enable(MOTOR_WATER_GPIO_CLK);
    
    /* bldc */
    gpio_init(MOTOR_WATER_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, MOTOR_WATER_PIN);
    gpio_bit_reset(MOTOR_WATER_GPIO_PORT, MOTOR_WATER_PIN);
}

void water_motor_start(void)
{
    /* bldc */
    gpio_bit_set(MOTOR_WATER_GPIO_PORT, MOTOR_WATER_PIN);
}

void water_motor_stop(void)
{
    /* bldc */
    gpio_bit_reset(MOTOR_WATER_GPIO_PORT, MOTOR_WATER_PIN);
}

void enzyme_motor_init(void)
{
    /* enable the clock of GPIO */
    rcu_periph_clock_enable(MOTOR_ENZYME_GPIO_CLK);
    
    /* bldc */
    gpio_init(MOTOR_ENZYME_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, MOTOR_ENZYME_PIN);
    gpio_bit_reset(MOTOR_ENZYME_GPIO_PORT, MOTOR_ENZYME_PIN);
}

void enzyme_motor_start(void)
{
    /* bldc */
    gpio_bit_set(MOTOR_ENZYME_GPIO_PORT, MOTOR_ENZYME_PIN);
}

void enzyme_motor_stop(void)
{
    /* bldc */
    gpio_bit_reset(MOTOR_ENZYME_GPIO_PORT, MOTOR_ENZYME_PIN);
}