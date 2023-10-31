/*!
    \file    lcd_ctrl.h
    \brief   the header file of lcd
*/


#ifndef LCD_CTRL_H
#define LCD_CTRL_H

#include "gd32f30x.h"


typedef enum {
    OPT_TYPE_SELECT = 0,
    OPT_TYPE_SET,
} ctrl_opt_type_e;

typedef enum {
    CLOCK_HOUR = 0,
    CLOCK_MINUTE,
} ctrl_clock_domain_type_e;

#endif /* LCD_H */
