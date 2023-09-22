/*!
    \file    ebike.h
    \brief   the header file of ebike

*/


#ifndef EBIKE_H
#define EBIKE_H

#include "type.h"

/* the GPIO configuration of ebike */
#define VELOCITY_GPIO_PORT              GPIOA
#define VELOCITY_GPIO_PIN               GPIO_PIN_9
#define VELOCITY_GPIO_CLK               RCU_GPIOA
#define VELOCITY_EXTI_LINE              EXTI_9
#define VELOCITY_EXTI_LINE_EDGE         EXTI_TRIG_RISING
#define VELOCITY_EXTI_SRC_PORT          GPIO_PORT_SOURCE_GPIOA
#define VELOCITY_EXTI_SRC_PIN           GPIO_PIN_SOURCE_9
#define VELOCITY_EXTI_IRQ               EXTI5_9_IRQn
/*
#define PROTECT_IN2_GPIO_PORT           GPIOB
#define PROTECT_IN2_GPIO_PIN            GPIO_PIN_7
#define PROTECT_IN2_GPIO_CLK            RCU_GPIOB
#define PROTECT_IN2_EXTI_LINE           EXTI_7
#define PROTECT_IN2_EXTI_LINE_EDGE      EXTI_TRIG_FALLING
#define PROTECT_IN2_EXTI_SRC_PORT       GPIO_PORT_SOURCE_GPIOB
#define PROTECT_IN2_EXTI_SRC_PIN        GPIO_PIN_SOURCE_7
#define PROTECT_IN2_EXTI_IRQ            EXTI5_9_IRQn
*/
extern uint8_t g_velocity_count;
extern float g_velocity, g_velocity_f;

#endif /* EBIKE_H */
