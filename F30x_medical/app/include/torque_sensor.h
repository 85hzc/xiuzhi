/*!
    \file    torque_sensor.h
    \brief   the header file of torque_sensor.c

*/

#ifndef TOR_H
#define TOR_H

#include "global.h"

/* TOR pin config */
#define TOR_IN1_GPIO_PORT               GPIOB
#define TOR_IN1_GPIO_PIN                GPIO_PIN_6
#define TOR_IN1_GPIO_CLK                RCU_GPIOB
#define TOR_IN1_EXTI_LINE               EXTI_6
#define TOR_IN1_EXTI_LINE_EDGE          EXTI_TRIG_RISING
#define TOR_IN1_EXTI_SRC_PORT           GPIO_PORT_SOURCE_GPIOB
#define TOR_IN1_EXTI_SRC_PIN            GPIO_PIN_SOURCE_6
#define TOR_IN1_EXTI_IRQ                EXTI5_9_IRQn


/* delay stabilization */
void joggle_delay(uint16_t num);

extern uint8_t cadence, cadence_f;

#endif /* TOR_H */
