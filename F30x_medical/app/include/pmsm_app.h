/*!
    \file    pmsm_app.h
    \brief   the header file of pmsm

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

#ifndef PMSM_APP_H
#define PMSM_APP_H

#include "global.h"

/* selftest rouine for power on operation */
//void startup_self_test(void);
/* start motor */
void motor_start(void);
/* stop motor */
void motor_stop(void);
/* mosfet driver enable routine */
//void IR2136S_enable(void);
/* mosfet driver disable routine */
//void IR2136S_disable(void);
/* set PWM frequency */
void update_period(uint16_t frequency);
/* update the timer 3-phase output and sample point, the configuration works in the next period */
//void update_timer(uint16_t ta, uint16_t tb, uint16_t tc, uint16_t tsample);
/* update the timer 3-phase output and sample point, the configuration works immediately */
//void update_timer_immediate(uint16_t ta, uint16_t tb, uint16_t tc, uint16_t tsample);
/* zero voltage vector output */
void update_timer_duty(uint8_t duty);
/* update the current sample point */
void update_timer_sample(uint16_t tsample);
/* the bus voltage mesurement */
float get_input_voltage(uint16_t data);
/* the mosfet temperature mesurement */
float get_temperature(uint16_t data);
/* security check routine */
void pmsm_motor_check(void);
/* overcurrent check routine */
void pmsm_overcurrent_check(float id_filtered, float iq_filtered);
/* power calculation */
void pmsm_power_calculation(int16_t vq, int16_t vd);


#endif /* PMSM_APP_H */
