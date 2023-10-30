/*!
    \file    lcd_ctrl.c
    \brief   
*/

#include "global.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

ctrl_opt_type_e ctrlOptType = OPT_TYPE_SELECT;
ctrl_func_opt_e ctrlFuncOpt = WATER_FUNC_SET;
ctrl_clock_domain_type_e clockSetDomain = CLOCK_HOUR;

uint16_t lcd_water_set;
uint32_t lcd_timestamp_set;


void short_press_handle( void )
{

    if (ctrlOptType == OPT_TYPE_SELECT) {   //功能选择模式，短按进入功能项

        ctrlOptType = OPT_TYPE_SET;

        switch (ctrlFuncOpt) {

            case CLOCK_FUNC_SET:
                clockSetDomain = CLOCK_HOUR;
                lcd_timestamp_set = rtc_counter_get();
                break;
            
            case WATER_FUNC_SET:
                break;

            case ENZYME_FUNC_SET:
                break;

            case TEMP_FUNC_SET:
                break;

            case CLEAR_FUNC_SET:
                break;

            case RESET_FUNC_SET:
                break;

            case ENZYME_INJECT_FUNC_SET:
                break;
        }
    } else if (ctrlOptType == OPT_TYPE_SET) {                                //功能设置模式，短按确认设置和保存参数，并且推出设置模式

        ctrlOptType = OPT_TYPE_SELECT;

        switch (ctrlFuncOpt) {
            case CLOCK_FUNC_SET:
                if (clockSetDomain == CLOCK_MINUTE) {
                    /* RTC configuration */
                    printf("RTC reconfigured....\r\n");
                    /* RTC configuration */
                    rtc_configuration();
                    /* adjust time by values entred by the user on the hyperterminal */
                    //uint16_t seconds = (lcd_timestamp_set % 3600) % 60;
                    uint16_t minutes = (lcd_timestamp_set % 3600) / 60;
                    uint16_t hours = lcd_timestamp_set / 3600;
                    time_adjust(hours, minutes, 0);
                    printf("time:%2d:%2d\r\n", hours, minutes);
                } else {
                    ctrlOptType = OPT_TYPE_SET;
                    clockSetDomain = CLOCK_MINUTE;
                }
                break;

            case WATER_FUNC_SET:
                water_set = lcd_water_set;
                flash_value_flash();
                config_init();
                break;

            case ENZYME_FUNC_SET:
                break;

            case TEMP_FUNC_SET:
                break;

            case CLEAR_FUNC_SET:
                break;

            case RESET_FUNC_SET:
                break;

            case ENZYME_INJECT_FUNC_SET:
                break;
        }
    }
}


void long_press_handle( void )
{
    //  TODO:
    //  factory reset
}



void CCW_press_handle( void )
{

    if (ctrlOptType == OPT_TYPE_SELECT) {   //功能选择模式，逆时针旋转进入功能项

        if (ctrlFuncOpt < ENZYME_INJECT_FUNC_SET) {
            ctrlFuncOpt = ctrlFuncOpt + 1;
        }
    } else if (ctrlOptType == OPT_TYPE_SET) {                                //功能设置模式，短按确认设置和保存参数，并且推出设置模式

        switch (ctrlFuncOpt) {
            case CLOCK_FUNC_SET:

                if (clockSetDomain == CLOCK_MINUTE) {

                    //uint16_t seconds = (lcd_timestamp_set % 3600) % 60;
                    //uint16_t minutes = (lcd_timestamp_set % 3600) / 60;
                    //uint16_t hours = lcd_timestamp_set / 3600;
                    if (lcd_timestamp_set > 60)
                        lcd_timestamp_set = lcd_timestamp_set - 60;

                    uint16_t minutes = (lcd_timestamp_set % 3600) / 60;
                    uint16_t hours = lcd_timestamp_set / 3600;
                    printf("time:%2d:%2d\r\n",hours,minutes);
                } else {

                    if (lcd_timestamp_set > 3600)
                        lcd_timestamp_set = lcd_timestamp_set - 3600;

                    uint16_t minutes = (lcd_timestamp_set % 3600) / 60;
                    uint16_t hours = lcd_timestamp_set / 3600;
                    printf("time:%2d:%2d\r\n",hours,minutes);
                }

                break;
            case WATER_FUNC_SET:
                break;
            case ENZYME_FUNC_SET:
                break;
            case TEMP_FUNC_SET:
                break;
            case CLEAR_FUNC_SET:
                break;
            case RESET_FUNC_SET:
                break;
            case ENZYME_INJECT_FUNC_SET:
                break;
        }
    }
}



void CW_press_handle( void )
{

    if (ctrlOptType == OPT_TYPE_SELECT) {   //功能选择模式，厂按进入功能项

        if (ctrlFuncOpt > CLOCK_FUNC_SET) {
            ctrlFuncOpt = ctrlFuncOpt - 1;
        }
    } else if (ctrlOptType == OPT_TYPE_SET) {                                //功能设置模式，短按确认设置和保存参数，并且推出设置模式

        switch (ctrlFuncOpt) {
            case CLOCK_FUNC_SET:
                if (clockSetDomain == CLOCK_MINUTE) {

                    //uint16_t seconds = (lcd_timestamp_set % 3600) % 60;
                    //uint16_t minutes = (lcd_timestamp_set % 3600) / 60;
                    //uint16_t hours = lcd_timestamp_set / 3600;
                    if (lcd_timestamp_set < 0xffffffff - 60)
                        lcd_timestamp_set = lcd_timestamp_set + 60;

                    uint16_t minutes = (lcd_timestamp_set % 3600) / 60;
                    uint16_t hours = lcd_timestamp_set / 3600;
                    printf("time:%2d:%2d\r\n",hours,minutes);
                } else {

                    if (lcd_timestamp_set < 0xffffffff - 3600)
                        lcd_timestamp_set = lcd_timestamp_set + 3600;

                    uint16_t minutes = (lcd_timestamp_set % 3600) / 60;
                    uint16_t hours = lcd_timestamp_set / 3600;
                    printf("time:%2d:%2d\r\n",hours,minutes);
                }
                break;
            case WATER_FUNC_SET:
                break;
            case ENZYME_FUNC_SET:
                break;
            case TEMP_FUNC_SET:
                break;
            case CLEAR_FUNC_SET:
                break;
            case RESET_FUNC_SET:
                break;
            case ENZYME_INJECT_FUNC_SET:
                break;
        }
    }
}


void report_operation_handle(uint8_t opt)
{
    switch (opt) {
        case 1: //short press
            short_press_handle();
            break;

        case 2: //long press
            long_press_handle();
            break;

        case 3: //CCW
            CCW_press_handle();
            break;

        case 4: //CW
            CW_press_handle();
            break;
    }
}
