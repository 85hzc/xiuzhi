/*!
    \file    step_motor.c
    \brief   A- A+ B -B+ driver
*/


#include "global.h"

#if 0
#define MOVE_CYCLE   4

#define MOVE_STEP_1  0
#define MOVE_STEP_2  1
#define MOVE_STEP_3  2
#define MOVE_STEP_4  3
#define MOVE_STEP_S  4

#define MOTOR_MOVE_STEP1 \
    gpio_bit_set(MOTOR_STEP_h_GPIO_PORT, MOTOR_STEP_PIN_A_h);   \
    gpio_bit_reset(MOTOR_STEP_l_GPIO_PORT, MOTOR_STEP_PIN_A_l); \
    gpio_bit_reset(MOTOR_STEP_l_GPIO_PORT, MOTOR_STEP_PIN_B_l); \
    gpio_bit_set(MOTOR_STEP_h_GPIO_PORT, MOTOR_STEP_PIN_B_h);

#define MOTOR_MOVE_STEP2 \
    gpio_bit_set(MOTOR_STEP_h_GPIO_PORT, MOTOR_STEP_PIN_A_h);   \
    gpio_bit_reset(MOTOR_STEP_l_GPIO_PORT, MOTOR_STEP_PIN_A_l); \
    gpio_bit_set(MOTOR_STEP_l_GPIO_PORT, MOTOR_STEP_PIN_B_l);   \
    gpio_bit_reset(MOTOR_STEP_h_GPIO_PORT, MOTOR_STEP_PIN_B_h);
    
#define MOTOR_MOVE_STEP3 \
    gpio_bit_reset(MOTOR_STEP_h_GPIO_PORT, MOTOR_STEP_PIN_A_h); \
    gpio_bit_set(MOTOR_STEP_l_GPIO_PORT, MOTOR_STEP_PIN_A_l);   \
    gpio_bit_set(MOTOR_STEP_l_GPIO_PORT, MOTOR_STEP_PIN_B_l);   \
    gpio_bit_reset(MOTOR_STEP_h_GPIO_PORT, MOTOR_STEP_PIN_B_h);
    
#define MOTOR_MOVE_STEP4 \
    gpio_bit_reset(MOTOR_STEP_h_GPIO_PORT, MOTOR_STEP_PIN_A_h); \
    gpio_bit_set(MOTOR_STEP_l_GPIO_PORT, MOTOR_STEP_PIN_A_l);   \
    gpio_bit_reset(MOTOR_STEP_l_GPIO_PORT, MOTOR_STEP_PIN_B_l); \
    gpio_bit_set(MOTOR_STEP_h_GPIO_PORT, MOTOR_STEP_PIN_B_h);

#define MOTOR_MOVE_STOP \
    gpio_bit_reset(MOTOR_STEP_l_GPIO_PORT, MOTOR_STEP_PIN_A_l); \
    gpio_bit_reset(MOTOR_STEP_h_GPIO_PORT, MOTOR_STEP_PIN_A_h); \
    gpio_bit_reset(MOTOR_STEP_l_GPIO_PORT, MOTOR_STEP_PIN_B_l); \
    gpio_bit_reset(MOTOR_STEP_h_GPIO_PORT, MOTOR_STEP_PIN_B_h);

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
    rcu_periph_clock_enable(MOTOR_STEP_h_GPIO_CLK);
    rcu_periph_clock_enable(MOTOR_STEP_l_GPIO_CLK);

    /* step motor IOs */
    gpio_init(MOTOR_STEP_l_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, MOTOR_STEP_PIN_A_l);
    gpio_init(MOTOR_STEP_h_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, MOTOR_STEP_PIN_A_h);
    gpio_init(MOTOR_STEP_l_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, MOTOR_STEP_PIN_B_l);
    gpio_init(MOTOR_STEP_h_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, MOTOR_STEP_PIN_B_h);
    gpio_bit_reset(MOTOR_STEP_l_GPIO_PORT, MOTOR_STEP_PIN_A_l);
    gpio_bit_reset(MOTOR_STEP_h_GPIO_PORT, MOTOR_STEP_PIN_A_h);
    gpio_bit_reset(MOTOR_STEP_l_GPIO_PORT, MOTOR_STEP_PIN_B_l);
    gpio_bit_reset(MOTOR_STEP_h_GPIO_PORT, MOTOR_STEP_PIN_B_h);
}


void step_motor_move_forward(uint16_t steps)
{
    uint16_t i;
    static uint8_t time_delay = 0;

    for (i=0; i<steps; i++)
    {
        if (((loop_state == LOOP_LUOBEI) && g_exti_luobei_position_flag ) ||
            ((loop_state == LOOP_CHUBEI_DETECT) && g_exti_chubei_position_flag ) ||
            ((loop_state == LOOP_ZHUYE) && g_exti_zhushui_position_flag ) ||
            state_position_error_timeout ||
            serious_error()) {
                        
            if (time_delay++ >= 20) {
                clear_position_flags();
                drv_motor_move_execute(MOVE_STEP_S);
                time_delay = 0;
                //printf_exti_flags();
                return;
            }
        }
        drv_motor_move_execute(motor_steps[step]);
        step = (step + MOVE_CYCLE - 1) % MOVE_CYCLE;
        delay_1ms(1);
    }
    drv_motor_move_execute(MOVE_STEP_S);
}

void step_motor_move_reverse(uint16_t steps)
{  
    uint16_t i;
    static uint8_t time_delay = 0;

    for (i=0; i<steps; i++)
    {
        if (g_exti_qibei_position_flag ||
            state_position_error_timeout ||
            ((loop_state == LOOP_LUOBEI) && self_diagnose && 
                g_exti_luobei_position_flag && !self_diag_first_time_flag) ||
            serious_error()) {

            if (time_delay++ >= 20) {
                clear_position_flags();
                drv_motor_move_execute(MOVE_STEP_S);
                time_delay = 0;
                //printf_exti_flags();
                return;
            }
        }
        drv_motor_move_execute(motor_steps[step]);
        step = (step + 1) % MOVE_CYCLE;
        delay_1ms(1);
    }
    drv_motor_move_execute(MOVE_STEP_S);
}

void step_motor_move_stop()
{  
    drv_motor_move_execute(MOVE_STEP_S);
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
            //printf("----step stop\r\n");
            break;
    }
}

#else
void step_motor_init(void)
{
    /* enable the clock of GPIO */
    //rcu_periph_clock_enable(MOTOR_STEP_h_GPIO_CLK);
    //rcu_periph_clock_enable(MOTOR_STEP_l_GPIO_CLK);

    /* step motor direction gpio */
    gpio_init(STEP_DIR_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, STEP_DIR_GPIO_PIN);
    gpio_bit_reset(STEP_DIR_GPIO_PORT, STEP_DIR_GPIO_PIN);

}

/*
** 步进电机
*/
void motor_on( void )
{
    /* configure TIMER channel output pulse value */
    timer_channel_output_pulse_value_config(TIMER2,TIMER_CH_2, 250);
}

void motor_off( void )
{
    /* configure TIMER channel output pulse value */
    timer_channel_output_pulse_value_config(TIMER2,TIMER_CH_2, 0);
}

void step_motor_move_stop()
{
    motor_off();
}

void step_motor_move_forward(uint16_t steps)
{
    //direct gpio set
    gpio_bit_set(STEP_DIR_GPIO_PORT, STEP_DIR_GPIO_PIN);
    motor_on();
}

void step_motor_move_reverse(uint16_t steps)
{
    //direct gpio reset
    gpio_bit_reset(STEP_DIR_GPIO_PORT, STEP_DIR_GPIO_PIN);
    motor_on();
}

#endif
