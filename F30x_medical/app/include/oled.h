/*!
    \file    oled.h
    \brief   the header file of oled

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

#ifndef OLED_H
#define OLED_H

#include "global.h"

#define MAX_ROW                     (32)
#define MAX_COLUMN                  (128)
#define MAX_PAGE                    (MAX_ROW/8)
#define BRIGHTNESS                  (0xff)

#define I2C1_SLAVE_ADDRESS7         (0x78)//(0x72)

/* the GPIO configuration of oled module */
#define OLED_SA0_PIN                GPIO_PIN_1
#define OLED_SA0_GPIO_CLK           RCU_GPIOC
#define OLED_SA0_GPIO_PORT          GPIOC

#define OLED_CS_PIN                 GPIO_PIN_2
#define OLED_CS_GPIO_CLK            RCU_GPIOC
#define OLED_CS_GPIO_PORT           GPIOC


/* the font enumeration */
typedef enum {
    FONT_6_8 = 0,
    FONT_8_16
}oled_font;

/* the font enumeration */
typedef enum {
    OLED_CMD = 0,
    OLED_DATA
}oled_cd_selection;

/* initialize the oled */
void oled_init(void);
/* oled flashing routine */
void oled_display(void);
/* oled logo display routine */
void oled_logo(void);
/* clear the oled */
void oled_clear(uint8_t start_page, uint8_t end_page);
/* display a character at the specified location */
void oled_showchar(uint8_t x, uint8_t y, uint8_t ch, oled_font size);
/* display a number at the specified location */
void oled_shownum(uint8_t x, uint8_t y, uint16_t num, uint8_t len, oled_font size);
/* display a string at the specified location */
void oled_showstring(uint8_t x, uint8_t y, uint8_t *p, oled_font size);

#endif /* OLED_H */
