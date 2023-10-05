/*!
    \file    ebile.c
    \brief   ebike riding policy

*/

#include "global.h"

void motor_sent_data(uint8_t pwm, uint8_t current);
void motor_sent_control(uint8_t pwm, uint8_t current);

float torque_ebike_f = 0.0, vbus_f = 0.0, g_velocity = 0.0, g_velocity_f = 0.0;
uint16_t vbus, i_Sensor, i_Input;
float g_torque_offset = 0.0;
uint8_t g_velocity_count = 0;


uint8_t calculateCRC8(const uint8_t *data, int  len)
{
    uint8_t crc = 0;
    int  i;

    for (i = 0; i < len; i++) {
        crc += data[i];
    }
    return crc;
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
    \brief      current offset calibration
    \param[in]  none
    \param[out] none
    \retval     none
*/
void torque_value_calibration(void)
{
    #if 0
    uint32_t i, torque = 0;
    /* use regular group to sample the offset */
    delay_1ms(100);
    /* the current offset of phase A */
    adc_regular_channel_config(ADC0, 0, TORQUE_SENSOR_PIN, ADC_SAMPLETIME_239POINT5);
    for(i=0; i<26; i++){
        adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);
        while(RESET == adc_flag_get(ADC0, ADC_FLAG_EOC));
        if(i >= 10){
            torque += adc_regular_data_read(ADC0);
        }
    }
    g_torque_offset = torque/16.0f;
    /* restore to default configuration */
    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, DISABLE);
    #else
    uint16_t torque = 0;
    float torqueSum = 0.0;
    uint8_t torque_offset_flag = 1, times=0;

    while (torque_offset_flag && times < 16) {

        /* ADC software trigger enable */
        adc_software_trigger_enable(ADC0, ADC_INSERTED_CHANNEL);
        
        //if (times >= 10) {
            delay_1ms(100);
            torque = adc_inserted_data_read(ADC0, ADC_INSERTED_CHANNEL_1);
            torqueSum += torque;
            printf("torque = %d\r\n", torque);
        //}

        if (times == 15) {
            g_torque_offset = (float)torqueSum / 16.0f;
            torque_offset_flag = 0;
        }
        times++;
    }
    #endif
    printf("g_torque_offset = %.2f\r\n", g_torque_offset);

    fmc_erase_pages(CONFIG_PAGE);
    fmc_data_program(CONFIG_PAGE);
}


/*!
    \brief      read the sampling
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ebike_process(void)
{
    uint8_t pwm;
    uint8_t I_ctrl;

    I_ctrl = (uint8_t)((torque_ebike_f / 1400.0f) * 100) % 100;
    pwm = (g_velocity_f >= 50.0f) ? 0 : 100;

    g_generator_power = I_ctrl * 1.2f;
    if (cadence_f >= 80) {
        (I_ctrl <= 80) ? (I_ctrl+=20) : (I_ctrl=100) ;
    } else {
        I_ctrl += (cadence_f / 80.0f) * 20;
    }

    motor_sent_control(pwm, I_ctrl);
}


void motor_sent_set(uint8_t vbus, uint8_t vbus_I)
{
    /*
    ** 设置指令：（空闲时间≥20ms）+ 0xAA + PAR1 + PAR2 + CheckSum
    **    PAR1：当前控制器工作电压等级 24V/36V/48V/60V/72V
    **    PAR2：当前控制器最大母线控制电流  单位：A
    */
    uint8_t usart_data[COM_BUFFER_SIZE];

    memset(usart_data, 0, COM_BUFFER_SIZE);
    usart_data[0] = 0xAA;
    usart_data[1] = vbus;
    usart_data[2] = vbus_I;
    usart_data[3] = calculateCRC8(usart_data, 3);

    //printf("motor_sent_set:0x%x 0x%x 0x%x 0x%x\r\n", usart_data[0],usart_data[1],usart_data[2],usart_data[3]);
    usart2_data_transfer(usart_data, COM_BUFFER_SIZE);
}

void motor_sent_control(uint8_t pwm, uint8_t current)
{
    /*
    ** 控制指令：（空闲时间≥20ms）+ 0x55 + PAR1 + PAR2 + CheckSum
    **    PAR1：PWM占空比,PWM宽度0-100,100表示100%,0表示0%。
    **    PAR2：母线电流限流值百分比,100表示100%,0表示0%。
    */
    uint8_t usart_data[COM_BUFFER_SIZE];

    memset(usart_data, 0, COM_BUFFER_SIZE);
    usart_data[0] = 0x55;
    usart_data[1] = pwm;
    usart_data[2] = current;
    usart_data[3] = calculateCRC8(usart_data, 3);

    //printf("motor_sent_control:0x%x 0x%x 0x%x 0x%x\r\n", usart_data[0],usart_data[1],usart_data[2],usart_data[3]);
    usart2_data_transfer(usart_data, COM_BUFFER_SIZE);
}

// 50ms period
void ebike_read_sample(void)
{
    uint16_t torque_ebike = 0.0;

    #if 0
    static uint32_t torque = 0;
    static uint8_t torque_offset_flag = 1, times=0;
    if (torque_offset_flag && times++ < 26) {
        if (times >= 10) {
            torque += adc_inserted_data_read(ADC0, ADC_INSERTED_CHANNEL_1);
        }
        if (times == 25) {
            g_torque_offset = (float)torque / 16.0f;
            torque_offset_flag = 0;
        }
        goto TODO;
    }
    #endif

    i_Sensor = adc_inserted_data_read(ADC0, ADC_INSERTED_CHANNEL_0);
    i_Input = i_Sensor * 0.003223;
    torque_ebike = adc_inserted_data_read(ADC0, ADC_INSERTED_CHANNEL_1) - g_torque_offset;
    HZC_LP_FAST(torque_ebike_f, torque_ebike, 0.1f);
    vbus     = adc_inserted_data_read(ADC0, ADC_INSERTED_CHANNEL_2) * 0.0129f;//3.3f * 16.0f / 4096.0f;//VBUS_CHANNEL
    HZC_LP_FAST(vbus_f, vbus, 0.1f);
    //printf("[AD]ibus:%d, vbus:%.1f, tor:%.2f(%.2f)\r\n", i_Sensor, vbus_f, torque_ebike_f, g_torque_offset);

    //TODO:
    /* ADC software trigger enable */
    adc_software_trigger_enable(ADC0, ADC_INSERTED_CHANNEL);
}

void hall_start(void)
{
    hall_init();

    timer_enable(TIMER0);
    timer_event_software_generate(TIMER0, TIMER_EVENT_SRC_UPG);

    /* clear TIMER2 trigger interrupt flag */
    timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_TRG);
    timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_TRG|TIMER_INT_FLAG_CH3);

    /* enable TIMER2 */
    timer_enable(TIMER2);
}

void calc_velocity_1_second()//
{
    /* 传感器1圈1个脉冲 */
    g_velocity = g_velocity_count * 0.68 * 3.14 * 3.6;
    g_velocity_count = 0;

    HZC_LP_FAST(g_velocity_f, g_velocity, 0.5f);
}

