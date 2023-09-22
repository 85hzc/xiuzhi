/*!
    \file    timeflag.c
    \brief   

    \version 2023
*/

//==================================================================================================
// Included header files
//==================================================================================================
#include "timeflag.h"


/*
    Private variable definitions
*/
static uint16_t g_rCounter_5ms   = 0;
static uint16_t g_rCounter_20ms  = 0;
static uint16_t g_rCounter_50ms  = 0;
static uint16_t g_rCounter_500ms = 0;
static uint16_t g_rCounter_100ms = 0;
static uint16_t g_rCounter_1s    = 0;
static uint16_t g_rCounter_3s    = 0;

/* 
    Public variable definitions
*/
uint8_t bTimeFlag_100ms;
uint8_t bTimeFlag_500ms;
uint8_t bTimeFlag_50ms;
uint8_t bTimeFlag_20ms;
uint8_t bTimeFlag_5ms;
uint8_t bTimeFlag_3s;
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

    g_rCounter_3s++;
    if (g_rCounter_3s >= drCnt3s)
    {
        g_rCounter_3s = 0;
        bTimeFlag_3s = 1;
    }

    g_rCounter_500ms++;
    if (g_rCounter_500ms >= drCnt500ms)
    {
        g_rCounter_500ms = 0;
        bTimeFlag_500ms = 1;
    }

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


