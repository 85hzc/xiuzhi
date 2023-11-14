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
uint8_t lcd_enzyme_rate, lcd_temperature_set, heat_disable = 0;
uint8_t lcd_update_flag = 1, lcd_ok_flag = 0, lcd_time_set_flag = 0;
uint8_t warnning_loop = READY_;     //用于屏幕提示告警信息

void short_press_handle( void )
{

    if (ctrlOptType == OPT_TYPE_SELECT) {                   //功能选择模式，短按进入功能项

        //  启动落杯
        start_work = 1;

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
                    lcd_time_set_flag = 0;
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
    if (ctrlOptType == OPT_TYPE_SELECT) {                   //功能选择模式，短按进入功能项

        ctrlOptType = OPT_TYPE_SET;

        switch (ctrlFuncOpt) {

            case SETTING_OPTIONS_TIME:
                clockSetDomain = CLOCK_HOUR;
                lcd_timestamp_set = rtc_counter_get();
                lcd_time_set_flag = 1;
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
                ctrlOptType = OPT_TYPE_SELECT;                  //清空数据，功能选项保持选择模式
                break;
            case SETTING_OPTIONS_INJECT:
                //TODO
                static uint8_t enzyme_motor_ctrl = 1;
                if (enzyme_motor_ctrl) {
                    enzyme_motor_start();
                    enzyme_motor_ctrl = 0;
                } else {
                    enzyme_motor_stop();
                    enzyme_motor_ctrl = 1;
                }
                ctrlOptType = OPT_TYPE_SELECT;                                 //INJECT模式，可重复操作注液电机
                break;
        }
    }
}


void CCW_press_handle( void )
{

    if ((ctrlOptType == OPT_TYPE_SELECT)) {                       //功能选择模式，逆时针旋转进入功能项

        ctrlFuncOpt = (ctrlFuncOpt + 1) % SETTING_OPTIONS_MAX_NUM;
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
                //常温模式，不加热
                if (lcd_temperature_set == 0) {
                    heat_disable = TEMPERATURE_NORMAL_VALUE;
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

    if ((ctrlOptType == OPT_TYPE_SELECT)) {   //功能选择模式，厂按进入功能项

        if (ctrlFuncOpt == SETTING_OPTIONS_TIME) {
            ctrlFuncOpt = SETTING_OPTIONS_INJECT;
        } else {
            ctrlFuncOpt = ctrlFuncOpt - 1;
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
                    heat_disable = 0;
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
            #ifdef DEBUG_PRINT
            printf("short press\r\n");
            #endif
            short_press_handle();
            break;

        case COMM_OPT_BTN_OR_KNOBS_LONG_PRESS: //long press
            #ifdef DEBUG_PRINT
            printf("long press\r\n");
            #endif
            long_press_handle();
            break;

        case COMM_OPT_BTN_OR_KNOBS_CCW_ROTATE: //CCW
            #ifdef DEBUG_PRINT
            printf("ccw press\r\n");
            #endif
            CCW_press_handle();
            break;

        case COMM_OPT_BTN_OR_KNOBS_CW_ROTATE: //CW
            #ifdef DEBUG_PRINT
            printf("cw press\r\n");
            #endif
            CW_press_handle();
            break;
    }
    lcd_update_flag = 1;
}


void lcd_update( void )
{
    //lcd display
    uint16_t sn = 0;
    uint8_t figure_num = 0;

    // 建立连接
    if (lcd_check_conn_status() == COMM_DISCONN)
    {
        lcd_conn_opt(COMM_CONN);
        return;
    }
    #ifdef DEBUG_PRINT
    printf("fun:%s\n", __FUNCTION__);
    #endif
    figure_msg_t imgs[64];

    // 显示黑屏大背景
    _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_BACKGROUND_BLUE_SKY_SERIAL_NUMBER, imgs[0].figure_no);
    _u16_2_byte2_big_endian(0, imgs[0].x_coordinate);
    _u16_2_byte2_big_endian(0, imgs[0].y_coordinate);
    figure_num++;

    // 显示背景大圆圈 可选自己喜欢的颜色
    if ((error_bits_flag & 1<<POSITION_ERROR) ||
        (error_bits_flag & 1<<WUBEI_ERROR) ||
        (error_bits_flag & 1<<QIBEI_ERROR) ||
        (error_bits_flag & 1<<ZHUSHUI_ERROR) ||
        (error_bits_flag & 1<<WENDU_ERROR)) {           //错误，需停止工作
        _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_CIRCLE_RED_2_SERIAL_NUMBER, imgs[1].figure_no);
    } else if (error_bits_flag & 1<<SHUIWEI_ERROR) {    //告警提示，需人为操作清除
        _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_CIRCLE_YELLOW_2_SERIAL_NUMBER, imgs[1].figure_no);
    } else {   //
        _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_CIRCLE_GREEN_2_SERIAL_NUMBER, imgs[1].figure_no);
    }
    _u16_2_byte2_big_endian(8, imgs[1].x_coordinate);
    _u16_2_byte2_big_endian(8, imgs[1].y_coordinate);
    figure_num++;

    // 显示底部菜单背景图标
    _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_BOTTOM_MENU_2_SERIAL_NUMBER, imgs[2].figure_no);
    _u16_2_byte2_big_endian(25, imgs[2].x_coordinate);
    _u16_2_byte2_big_endian(197, imgs[2].y_coordinate);
    figure_num++;

    // 显示中间总容量信息图标
    _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_TOTAL_SET_BG_SERIAL_NUMBER, imgs[3].figure_no);
    _u16_2_byte2_big_endian(38, imgs[3].x_coordinate);
    _u16_2_byte2_big_endian(141, imgs[3].y_coordinate);
    figure_num++;

    _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_TOTAL_CAPACITY_SERIAL_NUMBER, imgs[4].figure_no);
    _u16_2_byte2_big_endian(48, imgs[4].x_coordinate);
    _u16_2_byte2_big_endian(147, imgs[4].y_coordinate);
    figure_num++;


    // 显示中间稀释比例信息图标
    _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_DILUTE_SET_BG_SERIAL_NUMBER, imgs[5].figure_no);
    _u16_2_byte2_big_endian(209, imgs[5].x_coordinate);
    _u16_2_byte2_big_endian(141, imgs[5].y_coordinate);
    figure_num++;

    _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_DILUTE_RATIO_SERIAL_NUMBER, imgs[6].figure_no);
    _u16_2_byte2_big_endian(216, imgs[6].x_coordinate);
    _u16_2_byte2_big_endian(147, imgs[6].y_coordinate);
    figure_num++;

    // 显示中间运行状态信息图标
    _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_STATUS_SET_BG_SERIAL_NUMBER, imgs[7].figure_no);
    _u16_2_byte2_big_endian(38, imgs[7].x_coordinate);
    _u16_2_byte2_big_endian(197, imgs[7].y_coordinate);
    figure_num++;
    _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_STATUS_SERIAL_NUMBER, imgs[8].figure_no);
    _u16_2_byte2_big_endian(46, imgs[8].x_coordinate);
    _u16_2_byte2_big_endian(202, imgs[8].y_coordinate);
    figure_num++;

    // 显示中间温度信息图标
    _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_TEMPERATURE_SET_BG_SERIAL_NUMBER, imgs[9].figure_no);
    _u16_2_byte2_big_endian(210, imgs[9].x_coordinate);
    _u16_2_byte2_big_endian(197, imgs[9].y_coordinate);
    figure_num++;
    _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_TEMPERATURE_SERIAL_NUMBER, imgs[10].figure_no);
    _u16_2_byte2_big_endian(220, imgs[10].x_coordinate);
    _u16_2_byte2_big_endian(201, imgs[10].y_coordinate);
    figure_num++;

    // 显示中间杯图标
    _u16_2_byte2_big_endian(IMAGES_MEASURE_DATA_CUP_SERIAL_NUMBER, imgs[11].figure_no);
    _u16_2_byte2_big_endian(240, imgs[11].x_coordinate);
    _u16_2_byte2_big_endian(282, imgs[11].y_coordinate);
    figure_num++;

    // 显示总容量
    uint8_t v1 = lcd_water_set % 10;
    uint8_t v10 = lcd_water_set / 10 % 10;
    uint8_t v100 = lcd_water_set / 100 % 10;
    uint8_t v1000 = lcd_water_set / 1000 % 10;
    _u16_2_byte2_big_endian(IMAGES_TOTAL_SET_ML_SERIAL_NUMBER, imgs[12].figure_no);
    _u16_2_byte2_big_endian(160, imgs[12].x_coordinate);
    _u16_2_byte2_big_endian(146, imgs[12].y_coordinate);
    figure_num++;

    sn = lcd_get_image_serial_number(v1, IMAGE_TYPE_TOTAL_SET);
    _u16_2_byte2_big_endian(sn, imgs[13].figure_no);
    _u16_2_byte2_big_endian(139, imgs[13].x_coordinate);
    _u16_2_byte2_big_endian(148, imgs[13].y_coordinate);
    figure_num++;

    sn = lcd_get_image_serial_number(v10, IMAGE_TYPE_TOTAL_SET);
    _u16_2_byte2_big_endian(sn, imgs[14].figure_no);
    _u16_2_byte2_big_endian(120, imgs[14].x_coordinate);
    _u16_2_byte2_big_endian(148, imgs[14].y_coordinate);
    figure_num++;

    if (v100 || v1000) {
        sn = lcd_get_image_serial_number(v100, IMAGE_TYPE_TOTAL_SET);
        _u16_2_byte2_big_endian(sn, imgs[15].figure_no);
        _u16_2_byte2_big_endian(101, imgs[15].x_coordinate);
        _u16_2_byte2_big_endian(148, imgs[15].y_coordinate);
        figure_num++;
    }

    if (v1000) {
        sn = lcd_get_image_serial_number(v1000, IMAGE_TYPE_TOTAL_SET);
        _u16_2_byte2_big_endian(sn, imgs[16].figure_no);
        _u16_2_byte2_big_endian(81, imgs[16].x_coordinate);
        _u16_2_byte2_big_endian(148, imgs[16].y_coordinate);
        figure_num++;
    }

    // 显示稀释比例
    uint8_t r_1 = lcd_enzyme_rate % 10;
    uint8_t r1 = lcd_enzyme_rate / 10 % 10;
    uint8_t r10 = lcd_enzyme_rate / 100 % 10;
    _u16_2_byte2_big_endian(IMAGES_DILUTE_SET_PERCENT_SERIAL_NUMBER, imgs[figure_num].figure_no);
    _u16_2_byte2_big_endian(327, imgs[figure_num].x_coordinate);
    _u16_2_byte2_big_endian(147, imgs[figure_num].y_coordinate);
    figure_num++;

    sn = lcd_get_image_serial_number(r_1, IMAGE_TYPE_DILUTE_SET);
    _u16_2_byte2_big_endian(sn, imgs[figure_num].figure_no);
    _u16_2_byte2_big_endian(307, imgs[figure_num].x_coordinate);
    _u16_2_byte2_big_endian(147, imgs[figure_num].y_coordinate);
    figure_num++;

    _u16_2_byte2_big_endian(IMAGES_DILUTE_SET_2_DOT_SERIAL_NUMBER, imgs[figure_num].figure_no);
    _u16_2_byte2_big_endian(300, imgs[figure_num].x_coordinate);
    _u16_2_byte2_big_endian(167, imgs[figure_num].y_coordinate);
    figure_num++;

    sn = lcd_get_image_serial_number(r1, IMAGE_TYPE_DILUTE_SET);
    _u16_2_byte2_big_endian(sn, imgs[figure_num].figure_no);
    _u16_2_byte2_big_endian(280, imgs[figure_num].x_coordinate);
    _u16_2_byte2_big_endian(147, imgs[figure_num].y_coordinate);
    figure_num++;

    if (r10) {
        sn = lcd_get_image_serial_number(r10, IMAGE_TYPE_DILUTE_SET);
        _u16_2_byte2_big_endian(sn, imgs[figure_num].figure_no);
        _u16_2_byte2_big_endian(260, imgs[figure_num].x_coordinate);
        _u16_2_byte2_big_endian(147, imgs[figure_num].y_coordinate);
        figure_num++;
    }

    // 显示出杯数量
    uint8_t c1 = cup_count % 10;
    uint8_t c10 = cup_count / 10 % 10;
    uint8_t c100 = cup_count / 100 % 10;
    uint8_t c1000 = cup_count / 1000 % 10;
    sn = lcd_get_image_serial_number(c1, IMAGE_TYPE_MEASURE_DATA);
    _u16_2_byte2_big_endian(sn, imgs[figure_num].figure_no);
    _u16_2_byte2_big_endian(209, imgs[figure_num].x_coordinate);
    _u16_2_byte2_big_endian(271, imgs[figure_num].y_coordinate);
    figure_num++;

    sn = lcd_get_image_serial_number(c10, IMAGE_TYPE_MEASURE_DATA);
    _u16_2_byte2_big_endian(sn, imgs[figure_num].figure_no);
    _u16_2_byte2_big_endian(189, imgs[figure_num].x_coordinate);
    _u16_2_byte2_big_endian(271, imgs[figure_num].y_coordinate);
    figure_num++;

    sn = lcd_get_image_serial_number(c100, IMAGE_TYPE_MEASURE_DATA);
    _u16_2_byte2_big_endian(sn, imgs[figure_num].figure_no);
    _u16_2_byte2_big_endian(169, imgs[figure_num].x_coordinate);
    _u16_2_byte2_big_endian(271, imgs[figure_num].y_coordinate);
    figure_num++;

    sn = lcd_get_image_serial_number(c1000, IMAGE_TYPE_MEASURE_DATA);
    _u16_2_byte2_big_endian(sn, imgs[figure_num].figure_no);
    _u16_2_byte2_big_endian(149, imgs[figure_num].x_coordinate);
    _u16_2_byte2_big_endian(271, imgs[figure_num].y_coordinate);
    figure_num++;

    //temperature
    if (TEMPERATURE_NORMAL_VALUE == heat_disable)
    {
        _u16_2_byte2_big_endian(IMAGES_TEMPERATURE_SHOW_NORMAL_TEMP_SERIAL_NUMBER, imgs[figure_num].figure_no);
        _u16_2_byte2_big_endian(275, imgs[figure_num].x_coordinate);
        _u16_2_byte2_big_endian(203, imgs[figure_num].y_coordinate);
        figure_num++;
    }
    else
    {
        _u16_2_byte2_big_endian(IMAGES_TEMPERATURE_SHOW_CELSIUS_SERIAL_NUMBER, imgs[figure_num].figure_no);
        _u16_2_byte2_big_endian(324, imgs[figure_num].x_coordinate);
        _u16_2_byte2_big_endian(203, imgs[figure_num].y_coordinate);
        figure_num++;

        uint16_t sn = 0;
        uint16_t t1 = lcd_temperature_set % 10;
        uint16_t t10 = lcd_temperature_set / 10 % 10;
        //uint16_t t100 = lcd_temperature_set / 100 % 10;

        sn = lcd_get_image_serial_number(t1, IMAGE_TYPE_TEMPERATURE_SHOW);
        _u16_2_byte2_big_endian(sn, imgs[figure_num].figure_no);
        _u16_2_byte2_big_endian(305, imgs[figure_num].x_coordinate);
        _u16_2_byte2_big_endian(204, imgs[figure_num].y_coordinate);
        figure_num++;

        sn = lcd_get_image_serial_number(t10, IMAGE_TYPE_TEMPERATURE_SHOW);
        _u16_2_byte2_big_endian(sn, imgs[figure_num].figure_no);
        _u16_2_byte2_big_endian(286, imgs[figure_num].x_coordinate);
        _u16_2_byte2_big_endian(204, imgs[figure_num].y_coordinate);
        figure_num++;
    }

    //setting icon
    uint16_t x = 0, y = 0;
    switch (ctrlFuncOpt)
    {
        case SETTING_OPTIONS_CLEAR:
            x = 264;
            y = 297;
            sn = IMAGES_SETTING_CLEAR_SET_SERIAL_NUMBER;
            break;
        case SETTING_OPTIONS_DILUTE:
            x = 162;
            y = 317;
            sn = IMAGES_SETTING_DILUTE_SET_SERIAL_NUMBER;
            break;
        case SETTING_OPTIONS_INJECT:
            x = 302;
            y = 257;
            sn = IMAGES_SETTING_RESET_SET_SERIAL_NUMBER;
            break;
        case SETTING_OPTIONS_TEMPERATURE:
            x = 216;
            y = 317;
            sn = IMAGES_SETTING_TEMPERATURE_SET_SERIAL_NUMBER;
            break;
        case SETTING_OPTIONS_TIME:
            x = 76;
            y = 258;
            sn = IMAGES_SETTING_TIME_SET_SERIAL_NUMBER;
            break;
        case SETTING_OPTIONS_TATAL_VOLUME:
            x = 111;
            y = 295;
            sn = IMAGES_SETTING_TOTAL_SET_SERIAL_NUMBER;
            break;
    }
    _u16_2_byte2_big_endian(sn, imgs[figure_num].figure_no);
    _u16_2_byte2_big_endian(x, imgs[figure_num].x_coordinate);
    _u16_2_byte2_big_endian(y, imgs[figure_num].y_coordinate);
    figure_num++;

    //timer
    uint32_t timestamp;
    uint8_t minutes, hours;
    if (!lcd_time_set_flag) {               //实时时钟显示
        timestamp = rtc_counter_get();
    } else {                                //时钟设置阶段显示
        timestamp = lcd_timestamp_set;
    }
    minutes = (timestamp % 3600) / 60;
    hours = timestamp / 3600;

    if (minutes < 10)
    {
        sn = lcd_get_image_serial_number(minutes, IMAGE_TYPE_TIME_SHOW);
        _u16_2_byte2_big_endian(sn, imgs[figure_num].figure_no);
        _u16_2_byte2_big_endian(237, imgs[figure_num].x_coordinate);
        _u16_2_byte2_big_endian(59, imgs[figure_num].y_coordinate);
        figure_num++;

        sn = lcd_get_image_serial_number(0, IMAGE_TYPE_TIME_SHOW);
        _u16_2_byte2_big_endian(sn, imgs[figure_num].figure_no);
        _u16_2_byte2_big_endian(206, imgs[figure_num].x_coordinate);
        _u16_2_byte2_big_endian(59, imgs[figure_num].y_coordinate);
        figure_num++;
    }
    else
    {
        uint8_t m0 = minutes % 10;
        uint8_t m1 = minutes / 10;

        sn = lcd_get_image_serial_number(m0, IMAGE_TYPE_TIME_SHOW);
        _u16_2_byte2_big_endian(sn, imgs[figure_num].figure_no);
        _u16_2_byte2_big_endian(237, imgs[figure_num].x_coordinate);
        _u16_2_byte2_big_endian(59, imgs[figure_num].y_coordinate);
        figure_num++;

        sn = lcd_get_image_serial_number(m1, IMAGE_TYPE_TIME_SHOW);
        _u16_2_byte2_big_endian(sn, imgs[figure_num].figure_no);
        _u16_2_byte2_big_endian(206, imgs[figure_num].x_coordinate);
        _u16_2_byte2_big_endian(59, imgs[figure_num].y_coordinate);
        figure_num++;
    }

    _u16_2_byte2_big_endian(IMAGES_TIME_DELIMITER_SERIAL_NUMBER, imgs[figure_num].figure_no);
    _u16_2_byte2_big_endian(194, imgs[figure_num].x_coordinate);
    _u16_2_byte2_big_endian(71, imgs[figure_num].y_coordinate);
    figure_num++;
    
    if (hours < 10)
    {
        sn = lcd_get_image_serial_number(hours, IMAGE_TYPE_TIME_SHOW);
        _u16_2_byte2_big_endian(sn, imgs[figure_num].figure_no);
        _u16_2_byte2_big_endian(164, imgs[figure_num].x_coordinate);
        _u16_2_byte2_big_endian(59, imgs[figure_num].y_coordinate);
        figure_num++;

        sn = lcd_get_image_serial_number(0, IMAGE_TYPE_TIME_SHOW);
        _u16_2_byte2_big_endian(sn, imgs[figure_num].figure_no);
        _u16_2_byte2_big_endian(135, imgs[figure_num].x_coordinate);
        _u16_2_byte2_big_endian(59, imgs[figure_num].y_coordinate);
        figure_num++;
    }
    else
    {
        uint8_t h0 = hours % 10;
        uint8_t h1 = hours / 10;

        sn = lcd_get_image_serial_number(h0, IMAGE_TYPE_TIME_SHOW);
        _u16_2_byte2_big_endian(sn, imgs[figure_num].figure_no);
        _u16_2_byte2_big_endian(164, imgs[figure_num].x_coordinate);
        _u16_2_byte2_big_endian(59, imgs[figure_num].y_coordinate);
        figure_num++;

        sn = lcd_get_image_serial_number(h1, IMAGE_TYPE_TIME_SHOW);
        _u16_2_byte2_big_endian(sn, imgs[figure_num].figure_no);
        _u16_2_byte2_big_endian(135, imgs[figure_num].x_coordinate);
        _u16_2_byte2_big_endian(59, imgs[figure_num].y_coordinate);
        figure_num++;
    }

    //warnning
    //if (error_bits_flag & 1<<warnning_loop) {

        switch (warnning_loop) {
            case POSITION_ERROR:
                //显示“走位错误”
                lcd_running_status_display(IMAGES_STATUS_POSITION_ERR_SERIAL_NUMBER, &imgs[figure_num]);
                break;

            case WUBEI_ERROR:
                //显示“无纸杯”
                lcd_running_status_display(IMAGES_STATUS_NO_CUP_SERIAL_NUMBER, &imgs[figure_num]);
                break;

            case QIBEI_ERROR:
                //显示“弃杯错误”
                lcd_running_status_display(IMAGES_STATUS_THROW_CUP_ERR_SERIAL_NUMBER, &imgs[figure_num]);
                break;

            case ZHUSHUI_ERROR:
                //显示“加注失败”
                lcd_running_status_display(IMAGES_STATUS_JIAZHU_FAIL_SERIAL_NUMBER, &imgs[figure_num]);
                break;

            case WENDU_ERROR:
                //显示“温度失控”
                lcd_running_status_display(IMAGES_STATUS_HEAT_FAIL_SERIAL_NUMBER, &imgs[figure_num]);
                break;

            case SHUIWEI_ERROR:
                //显示“水位不足”
                lcd_running_status_display(IMAGES_STATUS_LOW_WATER_SERIAL_NUMBER, &imgs[figure_num]);
                break;

            case RESET_:
                //显示“复位”
                lcd_running_status_display(IMAGES_STATUS_RESET_SERIAL_NUMBER, &imgs[figure_num]);
                break;

            case READY_:
                //显示“待机”
                lcd_running_status_display(IMAGES_STATUS_STANDBY_SERIAL_NUMBER, &imgs[figure_num]);
                break;

            case CHUBEI_:
                //显示“出杯”
                lcd_running_status_display(IMAGES_STATUS_OUT_CUP_SERIAL_NUMBER, &imgs[figure_num]);
                break;

            case HUISHOU_:
                //显示“回收”
                lcd_running_status_display(IMAGES_STATUS_RECYCLE_SERIAL_NUMBER, &imgs[figure_num]);
                break;

            default: //STATUS_NULL
                break;
                figure_num--;
                //显示空
        }
        figure_num++;
    //}

    lcd_fill_bg_and_icon_cmd(imgs, figure_num);
}


void lcd_update_timer_flag_check( void )
{
    static uint16_t minutes_last = 0xff;
    uint32_t timestamp;
    uint8_t minutes, hours;
    if (!lcd_time_set_flag) {             //实时时钟显示
        timestamp = rtc_counter_get();
    } else {            //时钟设置阶段显示
        timestamp = lcd_timestamp_set;
    }
    minutes = (timestamp % 3600) / 60;
    hours = timestamp / 3600;

    if (minutes == minutes_last) {
        return;
    }
    minutes_last = minutes;
    lcd_update_flag = 1;
}

/*
void lcd_update_flag_warnning_check( void )
{
    static uint8_t next_warnning = READY_;
    warnning_loop = next_warnning;
    while (!(error_bits_flag & 1<<warnning_loop)) {
        warnning_loop++;
        if (warnning_loop >= ERROR_max) {
            next_warnning = READY_;
            //warnning_loop = READY_;
            break;
        }
    }
    next_warnning = warnning_loop+1;

    if (warnning_loop >= ERROR_max) {  //有效状态信息，需要显示
        printf("bit=0x%x, loop=%d\r\n", error_bits_flag, warnning_loop);
    }
}*/
