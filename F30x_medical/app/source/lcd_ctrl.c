/*!
    \file    lcd_ctrl.c
    \brief   
*/

#include "global.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

ctrl_opt_type_e ctrlOptType = OPT_TYPE_SELECT;
setting_option_type_e ctrlFuncOpt = SETTING_OPTIONS_TATAL_VOLUME;
ctrl_clock_domain_type_e clockSetDomain = CLOCK_HOUR;

uint16_t lcd_water_set;
uint32_t lcd_timestamp_set;
uint8_t lcd_enzyme_rate, lcd_temperature_set;
uint8_t lcd_update_flag = 1;

void short_press_handle( void )
{

    if (ctrlOptType == OPT_TYPE_SELECT) {   //功能选择模式，短按进入功能项

        ctrlOptType = OPT_TYPE_SET;

        switch (ctrlFuncOpt) {

            case SETTING_OPTIONS_TIME:
                clockSetDomain = CLOCK_HOUR;
                lcd_timestamp_set = rtc_counter_get();
                break;
            
            case SETTING_OPTIONS_TATAL_VOLUME:
                lcd_water_set = water_set;
                break;
            case SETTING_OPTIONS_DILUTE:
                lcd_enzyme_rate = enzyme_rate;
                break;
            case SETTING_OPTIONS_TEMPERATURE:
                lcd_temperature_set = temperature_set;
                break;
            case SETTING_OPTIONS_CLEAR:
                cup_count = 0;
                flash_value_flash();
                config_init();
                break;
            case SETTING_OPTIONS_INJECT:
                //TODO
                break;
        }
    } else if (ctrlOptType == OPT_TYPE_SET) {                                //功能设置模式，短按确认设置和保存参数，并且推出设置模式

        ctrlOptType = OPT_TYPE_SELECT;

        switch (ctrlFuncOpt) {
            case SETTING_OPTIONS_TIME:
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
                    bkp_write_data(BKP_DATA_0, 0xA5A5);
                } else {
                    ctrlOptType = OPT_TYPE_SET;
                    clockSetDomain = CLOCK_MINUTE;
                }
                break;

            case SETTING_OPTIONS_TATAL_VOLUME:
                water_set = lcd_water_set;
                flash_value_flash();
                config_init();
                break;

            case SETTING_OPTIONS_DILUTE:
                enzyme_rate = lcd_enzyme_rate;
                flash_value_flash();
                config_init();
                break;

            case SETTING_OPTIONS_TEMPERATURE:
                temperature_set = lcd_temperature_set;
                flash_value_flash();
                config_init();
                break;
            /*
            case SETTING_OPTIONS_CLEAR:
                break;
            case SETTING_OPTIONS_INJECT:
                break;
            */
        }
    }
}


void long_press_handle( void )
{
    //  TODO:
    //  factory reset
    printf("long press\r\n");
}


void CCW_press_handle( void )
{

    if (ctrlOptType == OPT_TYPE_SELECT) {                       //功能选择模式，逆时针旋转进入功能项

        if (ctrlFuncOpt == SETTING_OPTIONS_INJECT) {
            ctrlFuncOpt = SETTING_OPTIONS_TIME;
        } else {
            ctrlFuncOpt = (ctrlFuncOpt + 1) % SETTING_OPTIONS_MAX_NUM;
        }
    } else if (ctrlOptType == OPT_TYPE_SET) {                   //功能设置模式，短按确认设置和保存参数，并且推出设置模式

        switch (ctrlFuncOpt) {
            case SETTING_OPTIONS_TIME:

                if (clockSetDomain == CLOCK_MINUTE) {

                    //uint16_t seconds = (lcd_timestamp_set % 3600) % 60;
                    uint16_t minutes = (lcd_timestamp_set % 3600) / 60;
                    uint16_t hours = lcd_timestamp_set / 3600;
                    if (minutes > 0)
                        lcd_timestamp_set = lcd_timestamp_set - 60;
                    minutes = (lcd_timestamp_set % 3600) / 60;
                    printf("minute time:%2d:%2d\r\n",hours,minutes);
                } else if (clockSetDomain == CLOCK_HOUR){

                    uint16_t minutes = (lcd_timestamp_set % 3600) / 60;
                    uint16_t hours = lcd_timestamp_set / 3600;
                    if (hours > 0)
                        lcd_timestamp_set = lcd_timestamp_set - 3600;
                    hours = lcd_timestamp_set / 3600;
                    printf("hours time:%2d:%2d\r\n",hours,minutes);
                }

                break;
            case SETTING_OPTIONS_TATAL_VOLUME:
                if (lcd_water_set > 0) {
                    lcd_water_set--;
                }
                break;
            case SETTING_OPTIONS_DILUTE:
                if (lcd_enzyme_rate > 0) {
                    lcd_enzyme_rate--;
                }
                break;
            case SETTING_OPTIONS_TEMPERATURE:
                if (lcd_temperature_set > 0) {
                    lcd_temperature_set--;
                }
                break;
            /*
            case SETTING_OPTIONS_CLEAR:
                break;
            case SETTING_OPTIONS_INJECT:
                break;
            */
        }
    }
}



void CW_press_handle( void )
{

    if (ctrlOptType == OPT_TYPE_SELECT) {   //功能选择模式，厂按进入功能项

        if (ctrlFuncOpt == SETTING_OPTIONS_TIME) {
            ctrlFuncOpt = SETTING_OPTIONS_INJECT;
        } else {
            ctrlFuncOpt = (ctrlFuncOpt - 1) % SETTING_OPTIONS_MAX_NUM;
        }
    } else if (ctrlOptType == OPT_TYPE_SET) {                                //功能设置模式，短按确认设置和保存参数，并且推出设置模式

        switch (ctrlFuncOpt) {
            case SETTING_OPTIONS_TIME:
                if (clockSetDomain == CLOCK_MINUTE) {

                    //uint16_t seconds = (lcd_timestamp_set % 3600) % 60;
                    uint16_t minutes = (lcd_timestamp_set % 3600) / 60;
                    uint16_t hours = lcd_timestamp_set / 3600;
                    if (minutes < 59)
                        lcd_timestamp_set = lcd_timestamp_set + 60;

                    minutes = (lcd_timestamp_set % 3600) / 60;
                    printf("minute time:%2d:%2d\r\n",hours,minutes);
                } else if (clockSetDomain == CLOCK_HOUR) {

                    uint16_t minutes = (lcd_timestamp_set % 3600) / 60;
                    uint16_t hours = lcd_timestamp_set / 3600;
                    if (hours < 23)
                        lcd_timestamp_set = lcd_timestamp_set + 3600;
                    hours = lcd_timestamp_set / 3600;
                    printf("hours time:%2d:%2d\r\n",hours,minutes);
                }
                break;
            case SETTING_OPTIONS_TATAL_VOLUME:
                if (lcd_water_set < 999) {
                    lcd_water_set++;
                }
                break;
            case SETTING_OPTIONS_DILUTE:
                if (lcd_enzyme_rate < 255) {
                    lcd_enzyme_rate++;
                }
                break;
            case SETTING_OPTIONS_TEMPERATURE:
                if (lcd_temperature_set < 99) {
                    lcd_temperature_set++;
                }
                break;
            case SETTING_OPTIONS_CLEAR:
                break;
            case SETTING_OPTIONS_INJECT:
                break;
        }
    }
}


void report_operation_handle(uint8_t opt)
{

    switch (opt) {
        case COMM_OPT_BTN_OR_KNOBS_SHORT_PRESS: //short press
            short_press_handle();
            break;

        case COMM_OPT_BTN_OR_KNOBS_LONG_PRESS: //long press
            long_press_handle();
            break;

        case COMM_OPT_BTN_OR_KNOBS_CCW_ROTATE: //CCW
            CCW_press_handle();
            break;

        case COMM_OPT_BTN_OR_KNOBS_CW_ROTATE: //CW
            CW_press_handle();
            break;
    }
}


void lcd_update( void )
{
    lcd_display_update();
    // 显示温度
    lcd_temperature_display(lcd_temperature_set);
    // 显示默认设置
    lcd_setting_display(ctrlFuncOpt);
    lcd_time_display(rtc_counter_get());
    /* lcd status update */
    lcd_status_display();
}

