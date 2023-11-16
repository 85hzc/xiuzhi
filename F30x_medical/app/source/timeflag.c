/*!
    \file    timeflag.c
    \brief   

    \version 2023
*/

//==================================================================================================
// Included header files
//==================================================================================================
#include "global.h"
#include "timeflag.h"


/*
    Private variable definitions
*/
static uint16_t g_rCounter_5ms   = 0;
static uint16_t g_rCounter_50ms  = 0;
static uint16_t g_rCounter_100ms = 0;
static uint16_t g_rCounter_200ms = 0;
static uint16_t g_rCounter_500ms = 0;
static uint16_t g_rCounter_1s    = 0;
static uint16_t g_rCounter_enzyme = 0;  //酶液注入量计时
static uint32_t g_rCounter_qubei = 0;  //取杯操作超时计时  15分钟=15*60*1000
static uint32_t g_rCounter_position_error = 0;  //取杯操作超时计时  10秒=10*1000
static uint32_t g_rCounter_temperature_error = 0;  //温控超时计时
static uint32_t g_rCounter_jiazhu_error = 0; //注水超时计时
/* 
    Public variable definitions
*/

uint8_t bTimeFlag_5ms;
uint8_t bTimeFlag_50ms;
uint8_t bTimeFlag_100ms;
uint8_t bTimeFlag_200ms;
uint8_t bTimeFlag_500ms;
uint8_t bTimeFlag_1s;

void periodTask_1ms(void)
{

    g_rCounter_5ms++;
    if (g_rCounter_5ms >= drCnt5ms)
    {
        g_rCounter_5ms = 0;
        bTimeFlag_5ms = 1;
    }

    g_rCounter_1s++;
    if (g_rCounter_1s >= drCnt1s)
    {
        g_rCounter_1s = 0;
        bTimeFlag_1s = 1;
    }

    g_rCounter_500ms++;
    if (g_rCounter_500ms >= drCnt500ms)
    {
        g_rCounter_500ms = 0;
        bTimeFlag_500ms = 1;
    }
    /*
    g_rCounter_200ms++;
    if (g_rCounter_200ms >= drCnt200ms)
    {
        g_rCounter_200ms = 0;
        bTimeFlag_200ms = 1;
    }
    */
    g_rCounter_100ms++;
    if (g_rCounter_100ms >= drCnt100ms)
    {
        g_rCounter_100ms = 0;
        bTimeFlag_100ms = 1;
    }

    g_rCounter_50ms++;
    if (g_rCounter_50ms >= drCnt50ms)
    {
        g_rCounter_50ms = 0;
        bTimeFlag_50ms = 1;
    }
}

void temperature_error_timer_start(uint8_t secs)
{
    temperature_error_count_times = secs*1000;

    if (!state_temperature_error_count_running)
        g_rCounter_temperature_error = 0;
    
    state_temperature_error_count_running = 1;
}

void temperature_error_timer_clear( void )
{
    state_temperature_error_count_running = 0;
    g_rCounter_temperature_error = 0;
}

void position_error_timer_start(uint8_t secs)
{
    position_error_count_times = secs*1000;
    g_rCounter_position_error = 0;
    state_position_error_count_running = 1;
}

void position_error_timer_clear( void )
{
    g_rCounter_position_error = 0;
    state_position_error_count_running = 0;
}

void jiazhu_error_timer_start(uint8_t secs)
{
    jiazhu_error_count_times = secs*1000;
    g_rCounter_jiazhu_error = 0;
    state_jiazhu_error_count_running = 1;
}

void jiazhu_error_timer_clear( void )
{
    g_rCounter_jiazhu_error = 0;
    state_jiazhu_error_count_running = 0;
}

void check_enzyme_timer( void )
{
    g_rCounter_enzyme++;
    if (g_rCounter_enzyme == enzyme_count_times) {
        
        enzyme_motor_stop();
        state_enzyme_count_running = 0;
        state_enzyme_ok = 1;
        g_rCounter_enzyme = 0;
    }
}
void qubei_timer_init( void )
{
    state_qubei_timeout = 0;
    state_qubei_count_running = 0;
    g_rCounter_qubei = 0;
}

void check_qubei_timer( void )
{
    g_rCounter_qubei++;
    if (g_rCounter_qubei == QIBEI_TIMEROUT_TIMER) {   //15min
        
        state_qubei_count_running = 0;
        state_qubei_timeout = 1;
        g_rCounter_qubei = 0;
    }
}

void check_position_error_timer( void )
{
    g_rCounter_position_error++;
    if (g_rCounter_position_error == position_error_count_times) {
        
        state_position_error_count_running = 0;
        state_position_error_timeout = 1;
        g_rCounter_position_error = 0;
    }
}


void check_jiazhu_error_timer( void )
{
    g_rCounter_jiazhu_error++;
    if (g_rCounter_jiazhu_error == jiazhu_error_count_times) {
        
        state_jiazhu_error_count_running = 0;
        state_jiazhu_error_timeout = 1;
        g_rCounter_jiazhu_error = 0;
    }
}

void check_temperature_error_timer( void )
{
    g_rCounter_temperature_error++;
    if (g_rCounter_temperature_error == temperature_error_count_times) {
        
        state_temperature_error_count_running = 0;
        state_temperature_error_timeout = 1;
        g_rCounter_temperature_error = 0;
    }
}
