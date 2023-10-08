/*!
    \file    ebile.c
    \brief   ebike riding policy

*/

#include "global.h"

void motor_sent_data(uint8_t pwm, uint8_t current);
void motor_sent_control(uint8_t pwm, uint8_t current);

uint16_t g_water_count = 0, water_count_signals = 0, enzyme_count_times = 0;
float temperature = 0.0;


uint8_t calculateCRC8(const uint8_t *data, int  len)
{
    uint8_t crc = 0;
    int  i;

    for (i = 0; i < len; i++) {
        crc += data[i];
    }
    return crc;
}


/*!
    \brief      current offset calibration
    \param[in]  none
    \param[out] none
    \retval     none
*/
void torque_value_calibration(void)
{
    printf("g_torque_offset\r\n");

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
    uint16_t temp;

    temp = adc_inserted_data_read(ADC0, ADC_INSERTED_CHANNEL_0);
    temperature = temp * 0.003223;

    //TODO:
    /* ADC software trigger enable */
    adc_software_trigger_enable(ADC0, ADC_INSERTED_CHANNEL);
}


/*
** 调节加热导通电流；
*/
void heat_running()
{
	  uint16_t pwm = 0;
    printf("[AD]temperature:%.1f\r\n", temperature);

    //need PID
    if ((temperature > 42.0)){
        //关闭加热
        pwm = 0;
    } else if ((temperature < 42.0)){
        //打开发电环路
        pwm = 500;
    }

    //0 ---- 500
    /* configure TIMER channel output pulse value */
    timer_channel_output_pulse_value_config(TIMER7,TIMER_CH_0, (uint32_t) 500 /** g_generator_gears / 5.0f*/);

    //g_generator_gears_pre = g_generator_gears;
    //fmc_erase_pages(MODE_PAGE);
    //fmc_data_program(MODE_PAGE);
}

//四个中断信号脚
uint8_t read_qibei_position_switch()
{
    return (SET == gpio_input_bit_get(PROTECT_GPIO_PORT, PROTECT_GPIO_PIN)) ? 1 : 0;
}

uint8_t read_luobei_position_switch()
{
    return (SET == gpio_input_bit_get(N1S_LUOBEI_GPIO_PORT, N1S_LUOBEI_GPIO_PIN)) ? 1 : 0;
}

uint8_t read_zhushui_position_switch()
{
    return (SET == gpio_input_bit_get(N2S_ZHUSHUI_GPIO_PORT, N2S_ZHUSHUI_GPIO_PIN)) ? 1 : 0;
}

uint8_t read_chubei_position_switch()
{
    return (SET == gpio_input_bit_get(N3S_CHUBEI_GPIO_PORT, N3S_CHUBEI_GPIO_PIN)) ? 1 : 0;
}

