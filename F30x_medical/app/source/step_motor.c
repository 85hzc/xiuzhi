/*!
    \file    step_motor.c
    \brief   A- A+ B -B+ driver
*/


#include "global.h"

#define MOVE_CYCLE   4

#define MOVE_STEP_1  0
#define MOVE_STEP_2  1
#define MOVE_STEP_3  2
#define MOVE_STEP_4  3
#define MOVE_STEP_S  4

#define MOTOR_MOVE_STEP1 \
    gpio_bit_set(MOTOR_STEP_GPIO_PORT, MOTOR_STEP_PIN_A_h);   \
    gpio_bit_reset(MOTOR_STEP_GPIO_PORT, MOTOR_STEP_PIN_A_l); \
    gpio_bit_reset(MOTOR_STEP_GPIO_PORT, MOTOR_STEP_PIN_B_l); \
    gpio_bit_set(MOTOR_STEP_GPIO_PORT, MOTOR_STEP_PIN_B_h);

#define MOTOR_MOVE_STEP2 \
    gpio_bit_set(MOTOR_STEP_GPIO_PORT, MOTOR_STEP_PIN_A_h);   \
    gpio_bit_reset(MOTOR_STEP_GPIO_PORT, MOTOR_STEP_PIN_A_l); \
    gpio_bit_set(MOTOR_STEP_GPIO_PORT, MOTOR_STEP_PIN_B_l);   \
    gpio_bit_reset(MOTOR_STEP_GPIO_PORT, MOTOR_STEP_PIN_B_h);
    
#define MOTOR_MOVE_STEP3 \
    gpio_bit_reset(MOTOR_STEP_GPIO_PORT, MOTOR_STEP_PIN_A_h); \
    gpio_bit_set(MOTOR_STEP_GPIO_PORT, MOTOR_STEP_PIN_A_l);   \
    gpio_bit_set(MOTOR_STEP_GPIO_PORT, MOTOR_STEP_PIN_B_l);   \
    gpio_bit_reset(MOTOR_STEP_GPIO_PORT, MOTOR_STEP_PIN_B_h);
    
#define MOTOR_MOVE_STEP4 \
    gpio_bit_reset(MOTOR_STEP_GPIO_PORT, MOTOR_STEP_PIN_A_h); \
    gpio_bit_set(MOTOR_STEP_GPIO_PORT, MOTOR_STEP_PIN_A_l);   \
    gpio_bit_reset(MOTOR_STEP_GPIO_PORT, MOTOR_STEP_PIN_B_l); \
    gpio_bit_set(MOTOR_STEP_GPIO_PORT, MOTOR_STEP_PIN_B_h);

#define MOTOR_MOVE_STOP \
    gpio_bit_reset(MOTOR_STEP_GPIO_PORT, MOTOR_STEP_PIN_A_l); \
    gpio_bit_reset(MOTOR_STEP_GPIO_PORT, MOTOR_STEP_PIN_A_h); \
    gpio_bit_reset(MOTOR_STEP_GPIO_PORT, MOTOR_STEP_PIN_B_l); \
    gpio_bit_reset(MOTOR_STEP_GPIO_PORT, MOTOR_STEP_PIN_B_h);

/* Private variables ---------------------------------------------------------*/
static uint8_t motor_steps[MOVE_CYCLE] = {MOVE_STEP_1 ,MOVE_STEP_2 ,MOVE_STEP_3, MOVE_STEP_4};
static uint8_t step = 0;

/* Private function prototypes -----------------------------------------------*/
static void drv_motor_move_execute(uint8_t step);


/*!
    \brief      initialize the GPIO port of led
    \param[in]  none
    \param[out] none
    \retval     none
*/
void step_motor_init(void)
{
    /* enable the clock of GPIO */
    rcu_periph_clock_enable(MOTOR_STEP_GPIO_CLK);
    
    /* bldc */
    gpio_init(MOTOR_STEP_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, MOTOR_STEP_PIN_A_l);
    gpio_init(MOTOR_STEP_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, MOTOR_STEP_PIN_A_h);
    gpio_init(MOTOR_STEP_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, MOTOR_STEP_PIN_B_l);
    gpio_init(MOTOR_STEP_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, MOTOR_STEP_PIN_B_h);
    gpio_bit_reset(MOTOR_STEP_GPIO_PORT, MOTOR_STEP_PIN_A_l);
    gpio_bit_reset(MOTOR_STEP_GPIO_PORT, MOTOR_STEP_PIN_A_h);
    gpio_bit_reset(MOTOR_STEP_GPIO_PORT, MOTOR_STEP_PIN_B_l);
    gpio_bit_reset(MOTOR_STEP_GPIO_PORT, MOTOR_STEP_PIN_B_h);
}


void step_motor_move_forward(uint16_t steps)
{
    uint16_t i;

    for (i=0,step=0; i<steps; i++)
    {
        if (g_exti_luobei_position_flag ||
            g_exti_chubei_position_flag ||
            g_exti_zhushui_position_flag) {
            clear_position_flags();
            drv_motor_move_execute(MOVE_STEP_S);
            return;
        }
        drv_motor_move_execute(motor_steps[step]);
        step = (step + 1) % MOVE_CYCLE;
        delay_1ms(5);
    }
    //drv_motor_move_execute(MOVE_STEP_S);
}

void step_motor_move_reverse(uint16_t steps)
{  
    uint16_t i;

    for (i=0,step=0; i<steps; i++)
    {
        if (g_exti_qibei_position_flag) {
            clear_position_flags();
            drv_motor_move_execute(MOVE_STEP_S);
            return;
        }
        drv_motor_move_execute(motor_steps[step]);
        step = (step + MOVE_CYCLE - 1) % MOVE_CYCLE;
        delay_1ms(5);
    }
    //drv_motor_move_execute(MOVE_STEP_S);
}

static void drv_motor_move_execute(uint8_t step)
{
    switch (step)
    {
        case MOVE_STEP_1:
            MOTOR_MOVE_STEP1;
            break;
        case MOVE_STEP_2:
            MOTOR_MOVE_STEP2;
            break;
        case MOVE_STEP_3:
            MOTOR_MOVE_STEP3;
            break;
        case MOVE_STEP_4:
            MOTOR_MOVE_STEP4;
            break;
        case MOVE_STEP_S:
            MOTOR_MOVE_STOP;
            break;
    }
}

