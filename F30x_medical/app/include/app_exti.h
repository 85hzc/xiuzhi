/*!
    \file    app_exti.h
    \brief   the header file of app_exti.c

*/

#ifndef EXTI_H
#define EXTI_H

#include "global.h"

/* 落杯器微动开关信号 */
#define LUOBEI_IRQ_GPIO_PORT           GPIOB
#define LUOBEI_IRQ_GPIO_PIN            GPIO_PIN_6
#define LUOBEI_IRQ_GPIO_CLK            RCU_GPIOB
#define LUOBEI_IRQ_EXTI_LINE           EXTI_6
#define LUOBEI_IRQ_EXTI_LINE_EDGE      EXTI_TRIG_RISING
#define LUOBEI_IRQ_EXTI_SRC_PORT       GPIO_PORT_SOURCE_GPIOB
#define LUOBEI_IRQ_EXTI_SRC_PIN        GPIO_PIN_SOURCE_6
#define LUOBEI_IRQ_EXTI_IRQ            EXTI5_9_IRQn

/* 流量传感器信号 */
#define VELOCITY_GPIO_PORT              GPIOA
#define VELOCITY_GPIO_PIN               GPIO_PIN_9
#define VELOCITY_GPIO_CLK               RCU_GPIOA
#define VELOCITY_EXTI_LINE              EXTI_9
#define VELOCITY_EXTI_LINE_EDGE         EXTI_TRIG_RISING
#define VELOCITY_EXTI_SRC_PORT          GPIO_PORT_SOURCE_GPIOA
#define VELOCITY_EXTI_SRC_PIN           GPIO_PIN_SOURCE_9
#define VELOCITY_EXTI_IRQ               EXTI5_9_IRQn

/* 安全限位，弃杯微动开关信号 */
#define PROTECT_GPIO_PORT               GPIOB
#define PROTECT_GPIO_PIN                GPIO_PIN_7
#define PROTECT_GPIO_CLK                RCU_GPIOB
#define PROTECT_EXTI_LINE               EXTI_7
#define PROTECT_EXTI_LINE_EDGE          EXTI_TRIG_FALLING
#define PROTECT_EXTI_SRC_PORT           GPIO_PORT_SOURCE_GPIOB
#define PROTECT_EXTI_SRC_PIN            GPIO_PIN_SOURCE_7
#define PROTECT_EXTI_IRQ                EXTI5_9_IRQn

/* 位置N1S落杯开关信号 */
#define N1S_LUOBEI_GPIO_PORT            GPIOB
#define N1S_LUOBEI_GPIO_PIN             GPIO_PIN_10
#define N1S_LUOBEI_GPIO_CLK             RCU_GPIOB
#define N1S_LUOBEI_EXTI_LINE            EXTI_10
#define N1S_LUOBEI_EXTI_LINE_EDGE       EXTI_TRIG_FALLING
#define N1S_LUOBEI_EXTI_SRC_PORT        GPIO_PORT_SOURCE_GPIOB
#define N1S_LUOBEI_EXTI_SRC_PIN         GPIO_PIN_SOURCE_10
#define N1S_LUOBEI_EXTI_IRQ             EXTI10_15_IRQn
/* 位置N2S注水开关信号 */
#define N2S_ZHUSHUI_GPIO_PORT            GPIOB
#define N2S_ZHUSHUI_GPIO_PIN             GPIO_PIN_11
#define N2S_ZHUSHUI_GPIO_CLK             RCU_GPIOB
#define N2S_ZHUSHUI_EXTI_LINE            EXTI_11
#define N2S_ZHUSHUI_EXTI_LINE_EDGE       EXTI_TRIG_FALLING
#define N2S_ZHUSHUI_EXTI_SRC_PORT        GPIO_PORT_SOURCE_GPIOB
#define N2S_ZHUSHUI_EXTI_SRC_PIN         GPIO_PIN_SOURCE_11
#define N2S_ZHUSHUI_EXTI_IRQ             EXTI10_15_IRQn
/* 位置N3储备开关信号 */
#define N3S_CHUBEI_GPIO_PORT            GPIOB
#define N3S_CHUBEI_GPIO_PIN             GPIO_PIN_12
#define N3S_CHUBEI_GPIO_CLK             RCU_GPIOB
#define N3S_CHUBEI_EXTI_LINE            EXTI_12
#define N3S_CHUBEI_EXTI_LINE_EDGE       EXTI_TRIG_FALLING
#define N3S_CHUBEI_EXTI_SRC_PORT        GPIO_PORT_SOURCE_GPIOB
#define N3S_CHUBEI_EXTI_SRC_PIN         GPIO_PIN_SOURCE_12
#define N3S_CHUBEI_EXTI_IRQ             EXTI10_15_IRQn



extern uint16_t g_water_count;


/* delay stabilization */
void joggle_delay(uint16_t num);

extern uint8_t cadence, cadence_f;

#endif /* EXTI_H */
