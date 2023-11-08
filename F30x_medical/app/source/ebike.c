/*!
    \file    ebile.c
    \brief   ebike riding policy

*/

#include "global.h"

uint16_t error_bits_flag = 0;
float temperature = 0.0, temperature_f = 0.0, temperature_cb = 0.0;;
PID_Parm pidParm;
uint16_t water_set;         //注水纯水容量 （ml）

void beep_on( void );
void beep_off( void );

void config_init( void )
{
    fmc_data_read();
    extern uint8_t fmc_data[3][8];
    water_set = fmc_data[0][4]<<8 | fmc_data[0][5];
    enzyme_rate = fmc_data[0][3];
    temperature_set = fmc_data[0][2];
    cup_count = fmc_data[0][0]<<8 | fmc_data[0][1];
    water_count_signals = water_set * 8.5;  //每毫升信号数量  8.5次/ml
    enzyme_count_times = (water_set * enzyme_rate/1000.0f)  * 1000; //流速：1 ml/s

    printf("water: %d ml(%d), enzyme rate: %d %%, temp_set: %d, cup_count: %d\r\n", water_set, water_count_signals, enzyme_rate
            , temperature_set, cup_count);

    if (temperature_set == 0) {
        heat_disable = TEMPERATURE_NORMAL_VALUE;
    }

    memset(&pidParm, 0, sizeof(PID_Parm));
    //温控pid参数初始化
    pidParm.qInRef = temperature_set;
    pidParm.qKp = 10.0;
    pidParm.qKi = 1.0;
    pidParm.qKc = 0.1;
    pidParm.qOutMax = TEMPERATURE_PWM_MAX;   //PWM占空比0-500范围，限制功率80%，设置最大400
    pidParm.qOutMin = TEMPERATURE_PWM_MIN;

    lcd_temperature_set = temperature_set;
    lcd_water_set = water_set;
    lcd_enzyme_rate = enzyme_rate;
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

//Rp上拉电阻值
float Rp=10;//000;
//T2为25摄氏度，折算为开尔文温度
float T2=273.15+25;
float Ka=273.15;
// 100ms period
void ebike_read_temperature(void)
{
    static uint8_t read_times = 0;
    float Rt=0.0, vol=0.0;

    vol = adc_inserted_data_read(ADC0, ADC_INSERTED_CHANNEL_0) * 3.3 / 4096.0f;
    //Rt=(3.3-vol)*10000/vol;
    Rt = vol * 10.0 / (3.3 - vol);
    //查表法判断Rt有效性
    extern const float ntc_B3950_10k[];
    //小于0度 or 大于100度，无效数据过滤
    if ((Rt > ntc_B3950_10k[0]) || ( Rt < ntc_B3950_10k[100])) {
        printf("res:%f over flow!\r\n", Rt);
        goto TODO;
    }

    temperature = 1 / (float)(1.0/T2+log(Rt/Rp) / 3950.0) - Ka + 0.5;
    HZC_LP_FAST(temperature_f, temperature, 0.1);
    pidParm.qInMeas = temperature_f;

    for ( uint8_t i=0; i<=100; i++ ) {
        if ((Rt<ntc_B3950_10k[i]) && (Rt>ntc_B3950_10k[i+1])) {
            temperature_cb = i;
        }
    }

    TODO:
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

void set_error(error_type_e err_bit)
{
    warnning_loop = err_bit;    //用于屏幕提示告警信息

    //printf("bits:0x%x\r\n", error_bits_flag);
    if (!(error_bits_flag & (1<<err_bit))) {
        error_bits_flag |= 1<<err_bit;
        lcd_update_flag = 1;
        //printf("set ok bits:0x%x\r\n", error_bits_flag);
    }
}

void clear_error(error_type_e err_bit)
{
    //if (error_bits_flag & ~(1<<err_bit)) {
        error_bits_flag &= ~(1<<err_bit);
        //lcd_update_flag = 1;
    //}
}

/*
** 调节加热导通电流；
*/
void heat_running( void )
{
    static uint8_t delay_heat_reset = 0;

    if (delay_heat_reset < 20)
        delay_heat_reset++;

    //0 ---- 500
    /* configure TIMER channel output pulse value */
    if (delay_heat_reset > 16) {
        //printf("qout=%.1f\r\n", pidParm.qOut);
        timer_channel_output_pulse_value_config(TIMER7,TIMER_CH_0, (uint32_t) pidParm.qOut);
    }
}

void ebike_check_warning()
{
    if (state_position_error_timeout) {
        state_position_error_timeout = 0;
        //输出“走位错误”
        set_error(POSITION_ERROR);
    } else if (state_jiazhu_error_timeout){
        //输出“加注失败”
        set_error(ZHUSHUI_ERROR);
    }

    if (((uint8_t)temperature_f > temperature_set+3) && heat_disable != TEMPERATURE_NORMAL_VALUE) {

        temperature_error_timer_start(15*60);
        if (state_temperature_error_timeout) {
            //输出“温度过高”
            set_error(WENDU_ERROR);
        }
    } else if (((uint8_t)temperature_f < temperature_set-3) && heat_disable != TEMPERATURE_NORMAL_VALUE) {

        temperature_error_timer_start(5*60);
        if (state_temperature_error_timeout) {
            //输出“温度过低”
            set_error(WENDU_ERROR);
        }
    } else {
        //温度正常，恢复告警
        temperature_error_timer_clear();
        clear_error(WENDU_ERROR);
    }

    if (state_youbei) {
        clear_error(WUBEI_ERROR);
    }

    if (error_bits_flag & 0xfff0) {   //
        clear_error(READY_);
        clear_error(RESET_);
        clear_error(CHUBEI_);
        clear_error(HUISHOU_);
    }
}

//四个中断信号脚
uint8_t read_qibei_position_switch()
{
    return (RESET == gpio_input_bit_get(QIBEI_GPIO_PORT, QIBEI_GPIO_PIN)) ? 1 : 0;
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

