/*!
    \file  rtc.c
    \brief RTC check and config,time_show and time_adjust function

    \version 2017-02-10, V1.0.0, firmware for GD32F30x
    \version 2018-10-10, V1.1.0, firmware for GD32F30x
    \version 2018-12-25, V2.0.0, firmware for GD32F30x
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "rtc.h"


uint32_t RTCSRC_FLAG = 0; 


/*!
    \brief      configure the RTC
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_configuration(void)
{
    /* enable PMU and BKPI clocks */
    rcu_periph_clock_enable(RCU_BKPI);
    rcu_periph_clock_enable(RCU_PMU);
    /* allow access to BKP domain */
    pmu_backup_write_enable();

    /* reset backup domain */
    bkp_deinit();

#if 0   //老板子
    /* enable LXTAL */
    rcu_osci_on(RCU_HXTAL);
    /* wait till LXTAL is ready */
    rcu_osci_stab_wait(RCU_HXTAL);
    /* select RCU_LXTAL as RTC clock source */
    rcu_rtc_clock_config(RCU_RTCSRC_HXTAL_DIV_128);
#else
    /* enable LXTAL */
    rcu_osci_on(RCU_LXTAL);
    /* wait till LXTAL is ready */
    rcu_osci_stab_wait(RCU_LXTAL);
    /* select RCU_LXTAL as RTC clock source */
    rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);
#endif

    /* enable RTC Clock */
    rcu_periph_clock_enable(RCU_RTC);

    /* wait for RTC registers synchronization */
    rtc_register_sync_wait();

    /* wait until last write operation on RTC registers has finished */
    rtc_lwoff_wait();

    /* enable the RTC second interrupt*/
    rtc_interrupt_enable(RTC_INT_SECOND);

    /* wait until last write operation on RTC registers has finished */
    rtc_lwoff_wait();

    /* set RTC prescaler: set RTC period to 1s */
    #if 0   //老板子
    rtc_prescaler_set(62500);
    #else
    rtc_prescaler_set(32767);
    #endif

    /* wait until last write operation on RTC registers has finished */
    rtc_lwoff_wait();
}

#if 0
/*!
    \brief      return the time entered by user, using Hyperterminal
    \param[in]  none
    \param[out] none
    \retval     current time of RTC counter value
*/
uint32_t time_regulate(uint32_t tmp_hh, uint32_t tmp_mm, uint32_t tmp_ss)
{
    printf("\r\n==============Time Settings=====================================");
    printf("\r\n  Please Set Hours");

    while (tmp_hh == 0xFF){
        tmp_hh = usart_scanf(23);
    }
    printf(":  %d", tmp_hh);
    printf("\r\n  Please Set Minutes");
    while (tmp_mm == 0xFF){
        tmp_mm = usart_scanf(59);
    }
    printf(":  %d", tmp_mm);
    printf("\r\n  Please Set Seconds");
    while (tmp_ss == 0xFF){
        tmp_ss = usart_scanf(59);
    }
    printf(":  %d", tmp_ss);

    /* return the value  store in RTC counter register */
    return((tmp_hh*3600 + tmp_mm*60 + tmp_ss));
}
#endif

/*!
    \brief      adjust time 
    \param[in]  none
    \param[out] none
    \retval     none
*/
void time_adjust(uint32_t hh, uint32_t mm, uint32_t ss)
{
    /* wait until last write operation on RTC registers has finished */
    rtc_lwoff_wait();
    /* change the current time */
    rtc_counter_set(hh*3600 + mm*60 + ss);
    /* wait until last write operation on RTC registers has finished */
    rtc_lwoff_wait();
}

/*!
    \brief      display the current time 
    \param[in]  timeVar: RTC counter value
    \param[out] none
    \retval     none
*/
void time_display(uint32_t timevar)
{
    uint32_t thh = 0, tmm = 0, tss = 0;

    /* compute  hours */
    thh = timevar / 3600;
    /* compute minutes */
    tmm = (timevar % 3600) / 60;
    /* compute seconds */
    tss = (timevar % 3600) % 60;

    printf(" Time: %0.2d:%0.2d:%0.2d\r\n", thh, tmm, tss);
}

/*!
    \brief      show the current time (HH:MM:SS) on the Hyperterminal
    \param[in]  none
    \param[out] none
    \retval     none
*/
void time_show(void)
{
    /* display current time */
    time_display(rtc_counter_get());
}

#if 0
/*!
    \brief      get numeric values from the hyperterminal
    \param[in]  value: input value from the hyperterminal
    \param[out] none
    \retval     input value in BCD mode
*/
uint8_t usart_scanf(uint32_t value)
{
    uint32_t index = 0;
    uint32_t tmp[2] = {0, 0};

    while (index < 2){
        /* loop until RBNE = 1 */
        while (usart_flag_get(USART0, USART_FLAG_RBNE) == RESET);
        tmp[index++] = (usart_data_receive(USART0));

        if ((tmp[index - 1] < 0x30) || (tmp[index - 1] > 0x39)){
            printf("\n\rPlease enter valid number between 0 and 9\n");
            index--;
        }
    }
    /* calculate the Corresponding value */
    index = (tmp[1] - 0x30) + ((tmp[0] - 0x30) * 10);
    /* check */
    if (index > value){
        printf("\n\rPlease enter valid number between 0 and %d\n", value);
        return 0xFF;
    }
    return index;
}
#endif

void rtc_init( void )
{
    /* get RTC clock entry selection */
    RTCSRC_FLAG = GET_BITS(RCU_BDCTL, 8, 9);

    if ((0xA5A5 != bkp_read_data(BKP_DATA_0)) || (0x00 == RTCSRC_FLAG)){
        /* backup data register value is not correct or not yet programmed
        or RTC clock source is not configured (when the first time the program 
        is executed or data in RCU_BDCTL is lost due to Vbat feeding) */
        printf("\r\nThis is a RTC demo!");
        printf("RTC not yet configured....\r\n");

        /* RTC configuration */
        rtc_configuration();

        printf("RTC configured....");
        /* adjust time by values entred by the user on the hyperterminal */
        time_adjust(0x0, 0x0, 0x0);
        bkp_write_data(BKP_DATA_0, 0xA5A5);
    }else{
        if (rcu_flag_get(RCU_FLAG_PORRST) != RESET){
            /* check if the power on reset flag is set */
            printf("\r\n\n Power On Reset occurred....");
        }else if (rcu_flag_get(RCU_FLAG_SWRST) != RESET){
            /* check if the pin reset flag is set */
            printf("\r\n\n External Reset occurred....");
        }

        /* allow access to BKP domain */
        rcu_periph_clock_enable(RCU_PMU);
        pmu_backup_write_enable();

        printf("\r\n No need to configure RTC....");
        /* wait for RTC registers synchronization */
        rtc_register_sync_wait();

        /* enable the RTC second */
        rtc_interrupt_enable(RTC_INT_SECOND);

        /* wait until last write operation on RTC registers has finished */
        rtc_lwoff_wait();
    }
}
