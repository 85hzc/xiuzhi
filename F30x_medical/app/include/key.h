/*!
    \file    key.h
    \brief   the header file of key

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

#ifndef KEY_H
#define KEY_H

#include "gd32f30x.h"

/* the GPIO configuration of key and other input */
#define KEY_GENERATOR_UP_PIN            GPIO_PIN_4
#define KEY_GENERATOR_UP_GPIO_CLK       RCU_GPIOA
#define KEY_GENERATOR_UP_GPIO_PORT      GPIOA

#define KEY_GENERATOR_DOWN_PIN          GPIO_PIN_5
#define KEY_GENERATOR_DOWN_GPIO_CLK     RCU_GPIOA
#define KEY_GENERATOR_DOWN_GPIO_PORT    GPIOA

#define KEY_MODE_UP_PIN                 GPIO_PIN_6
#define KEY_MODE_UP_GPIO_CLK            RCU_GPIOA
#define KEY_MODE_UP_GPIO_PORT           GPIOA

#define KEY_MODE_DOWN_PIN               GPIO_PIN_7
#define KEY_MODE_DOWN_GPIO_CLK          RCU_GPIOA
#define KEY_MODE_DOWN_GPIO_PORT         GPIOA


#define MOTOR_A_PIN                     GPIO_PIN_8
#define MOTOR_A_GPIO_CLK                RCU_GPIOA
#define MOTOR_A_GPIO_PORT               GPIOA
#define MOTOR_B_PIN                     GPIO_PIN_13
#define MOTOR_B_GPIO_CLK                RCU_GPIOB
#define MOTOR_B_GPIO_PORT               GPIOB

/* the key enumeration */
typedef enum {
    KEY_GENERATOR_UP = 0,
    KEY_GENERATOR_DOWN,
    KEY_MODE_UP,
    KEY_MODE_DOWN,
} key_switch;

/* function declarations */
/* initialize key */
extern void key_init(void);
/* key process */
extern void key_process(void);
#endif /* KEY_H */
