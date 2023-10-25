/*!
    \file    ebile.c
    \brief   ebike riding policy

*/

#include "global.h"

uint16_t error_bits_flag = 0;
float temperature = 0.0;
PID_Parm pidParm;

void beep_on( void );
void beep_off( void );

void config_init( void )
{
    fmc_data_read();
    extern uint8_t fmc_data[3][8];
    water_set = fmc_data[0][3];
    enzyme_rate = fmc_data[0][2];
    temperature_set = fmc_data[0][1];
    cup_count = fmc_data[0][0];
    
    water_count_signals = water_set * 8.5;  //每毫升信号数量  8.5次/ml
    enzyme_count_times = ( (water_set * enzyme_rate) / (float)(100.0 - enzyme_rate) ) * 1000; //流速：1 ml/s

    printf("water: %dml, enzyme rate: %d\%, temp_set: %d, cup_count: %d\r\n", water_set, enzyme_rate
            , temperature_set, cup_count);

    memset(&pidParm, 0, sizeof(PID_Parm));
    //温控pid参数初始化
    pidParm.qInRef = temperature_set;
    pidParm.qKp = 0.5;
    pidParm.qKi = 0.1;
    pidParm.qKc = 0.1;
    pidParm.qOutMax = TEMPERATURE_PWM_MAX;   //PWM占空比0-500范围，限制功率80%，设置最大400
    pidParm.qOutMin = TEMPERATURE_PWM_MIN;
    //g_torque_offset = (float)(fmc_data[1][0] + fmc_data[1][1]) / 100.0f;

    beep_off();
}

//校验和计算
uint8_t calculateCRC8(const uint8_t *data, int  len)
{
    uint8_t crc = 0;
    int  i;

    for (i = 0; i < len; i++) {
        crc += data[i];
    }
    return crc;
}

void EBI_calcPI( PID_Parm *pParm)
{
    float Err;
    float U;
    float Exc;

    Err  = pParm->qInRef - pParm->qInMeas;
    pParm->qErr =  Err; 
    U  = pParm->qdSum + pParm->qKp * Err;

    if( U > pParm->qOutMax )
    {
        pParm->qOut = pParm->qOutMax;
    }    
    else if( U < pParm->qOutMin )
    {
        pParm->qOut = pParm->qOutMin;
    }
    else
    {
        pParm->qOut = U;  
    }

    Exc = U - pParm->qOut;

    pParm->qdSum = pParm->qdSum + pParm->qKi * Err - pParm->qKc * Exc;
}

/*!
    \brief      current offset calibration
    \param[in]  none
    \param[out] none
    \retval     none
*/
void flash_value_flash(void)
{
    printf("flash update.\r\n");
    fmc_erase_pages(MODE_PAGE);
    fmc_data_program(MODE_PAGE);
}


/*!
    \brief      read the sampling
    \param[in]  none
    \param[out] none
    \retval     none
*/
void display_process(void)
{
    /*
    ** 控制指令：（空闲时间≥20ms）+ 0x55 + PAR1 + PAR2 + CheckSum
    **    PAR1：PWM占空比,PWM宽度0-100,100表示100%,0表示0%。
    **    PAR2：母线电流限流值百分比,100表示100%,0表示0%。
    */
    uint8_t usart_data[COM_BUFFER_SIZE];

    memset(usart_data, 0, COM_BUFFER_SIZE);
    usart_data[0] = 0x55;
    usart_data[1] = 0;
    usart_data[2] = 0;
    usart_data[3] = calculateCRC8(usart_data, 3);

    usart2_data_transfer(usart_data, COM_BUFFER_SIZE);
}


// 50ms period
void ebike_read_temperature(void)
{
    float Rt=0.0;
    //Rp上拉电阻值
    float Rp=10000;
    //T2为25摄氏度，折算为开尔文温度
    float T2=273.15+25;
    float Bx=3950;
    float Ka=273.15;
    float vol=0;
/*
    uint16_t temp;
    temp = adc_inserted_data_read(ADC0, ADC_INSERTED_CHANNEL_0);
    temperature = temp * 0.003223;
*/
    vol=(float)(adc_inserted_data_read(ADC0, ADC_INSERTED_CHANNEL_0) * 3.3 / 4096.0f);
    Rt=(3.3-vol)*10000/vol;
    temperature=1/(1/T2+log(Rt/Rp)/Bx)-Ka+0.5;
    pidParm.qInMeas = temperature;

    //TODO:
    /* ADC software trigger enable */
    adc_software_trigger_enable(ADC0, ADC_INSERTED_CHANNEL);
}


/*
** 蜂鸣器
*/
void beep_on( void )
{
    /* configure TIMER channel output pulse value */
    timer_channel_output_pulse_value_config(TIMER7,TIMER_CH_1, 250);
}

void beep_off( void )
{
    /* configure TIMER channel output pulse value */
    timer_channel_output_pulse_value_config(TIMER7,TIMER_CH_1, 0);
}


/*
** 调节加热导通电流；
*/
void heat_running( void )
{
    //need PID
    #if 1
    EBI_calcPI(&pidParm);
    #else
    uint16_t pwm = 0;
    if ((temperature > 42.0)){
        //关闭加热
        pwm = 0;
    } else if ((temperature < 42.0)){
        //打开发电环路
        pwm = 500;
    }
    #endif

    //0 ---- 500
    /* configure TIMER channel output pulse value */
    //printf("qout=%f\r\n", pidParm.qOut);
    timer_channel_output_pulse_value_config(TIMER7,TIMER_CH_0, (uint32_t) pidParm.qOut);
}

void ebike_check_warning()
{
    if (state_position_error_timeout) {
        state_position_error_timeout = 0;
        //输出“走位错误”
        error_bits_flag |= 1<<POSITION_ERROR;
    } else if (state_jiazhu_error_timeout){
        //输出“加注失败”
        error_bits_flag |= 1<<ZHUSHUI_ERROR;
    }

    if ((uint8_t)temperature > temperature_set+3) {

        temperature_error_timer_start(15*60);
        if (state_temperature_error_timeout) {
            //输出“温度过高”
            error_bits_flag |= 1<<WENDU_ERROR;
        }
    } else if ((uint8_t)temperature < temperature_set-3) {

        temperature_error_timer_start(5*60);
        if (state_temperature_error_timeout) {
            //输出“温度过低”
            error_bits_flag |= 1<<WENDU_ERROR;
        }
    } else {
        //温度正常，恢复告警
        temperature_error_timer_clear();
        error_bits_flag &= ~(1<<WENDU_ERROR);
    }

    if (state_youbei) {
        error_bits_flag &= ~(1<<WUBEI_ERROR);
    }
    
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

