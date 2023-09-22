/*!
    \file    global.c
    \brief   FOC demo

    \version 2019-07-15, V1.0.0, FOC demo
    \version 2020-12-15, V1.1.0, FOC demo
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

#include "global.h"

/* reference variable */
//uint16_t adc_reference = 800;
uint16_t adc_reference_filtered = 0;
//uint16_t speed_reference = 0;
int16_t torque_reference = 0;

/* global structure */
Generator_e g_generator_gears = GENERATOR_0, g_generator_gears_pre = GENERATOR_0;   //踩踏阻力
Gear_e g_drive_mode = GEAR_0;           //电机助力
uint16_t g_generator_power = 0, g_error_code = 0;
motor_state motor;
mc_flags motor_flag = {0,0,0,1,0,0};

flash_page_type page_type;

/* hall variables */
hall_structure hall;


/* dma buffer */
uint16_t timer_update_buffer[6] = {0};
uint16_t adc_buffer[2] = {0};

/* phase current */
int16_t current_a = 0, current_b = 0 ,current_c = 0;
/* the reload value of timer */
uint16_t pwm_top;
/* the frequency of PWM */
uint16_t frequency_now;
/* the sine and cosine of the spatial angle */
float phase_sin,phase_cos;
int16_t uphase_sin,uphase_cos;

/* debug variables */
usart_debug debug_data;

