/*!
    \file    lcd_ctrl.h
    \brief   the header file of lcd
*/


#ifndef LCD_CTRL_H
#define LCD_CTRL_H

#include "gd32f30x.h"

typedef enum {
    CLOCK_FUNC_SET = 0,
    WATER_FUNC_SET,
    ENZYME_FUNC_SET,
    TEMP_FUNC_SET,
    CLEAR_FUNC_SET,
    RESET_FUNC_SET,
    ENZYME_INJECT_FUNC_SET,

    CTRL_FUNC_OPT_MAX_NUM

} ctrl_func_opt_e;

typedef enum {
    OPT_TYPE_SELECT = 0,
    OPT_TYPE_SET,
} ctrl_opt_type_e;

typedef enum {
    CLOCK_HOUR = 0,
    CLOCK_MINUTE,
} ctrl_clock_domain_type_e;

#endif /* LCD_H */
