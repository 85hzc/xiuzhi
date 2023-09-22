/*!
    \file    debug.h
    \brief   the header file of debug module

    \version 2019-07-15, V1.0.0, FOC demo
    \version 2021-02-01, V1.2.0, FOC demo
    \version 2021-02-01, V1.2.0, FOC demo
    \version 2021-08-16, V1.3.0, FOC demo
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

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

#ifndef DEBUG_H
#define DEBUG_H

#include "gd32f30x.h"

#define USART1_BUFFER_SIZE (3)

/* the structure of debug */
typedef struct
{
    int16_t data1;
    int16_t data2;
    int16_t data3;
    int16_t data4;
    int16_t data5;
    int16_t data6;
    int16_t data7;
    int16_t data8;
} usart_debug;

/* initialize the hardware of debug function */
void debug_init(void);
/* data transmission with USART */
void debug_usart_data_transfer(usart_debug data);
/* data transmission with DAC */
void debug_dac_data_transfer(int16_t a, int16_t b);

extern uint8_t debug_rx_buffer[],debug_rx_buffer_app[];
extern  uint16_t debug_rx_counter, debug_rx_ok;

extern float torque_ebike_f, vbus_f;
extern uint16_t vbus, i_Sensor, i_Input;
extern float g_torque_offset;
extern uint8_t caden;

#endif /* DEBUG_H */

