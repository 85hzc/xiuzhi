/*!
    \file    lcd.h
    \brief   the header file of lcd
*/


#ifndef LCD_H
#define LCD_H

#include "gd32f30x.h"


#define COMM_HEADER (0xAA)

#define COMM_CONN (0x01)
#define COMM_DISCONN (0x00)

#define COMM_CTRL_DISPLAY_ON (0x01)
// 如果没有填图 显示原来的图片 原来没有图片则显示黑屏
#define COMM_CTRL_DISPLAY_OFF (0x00)

#define COMM_OPT_BTN_OR_KNOBS_SHORT_PRESS (0x01)
#define COMM_OPT_BTN_OR_KNOBS_LONG_PRESS (0x02)
// 左旋即用户正面面对旋钮模组屏逆时针旋转
#define COMM_OPT_BTN_OR_KNOBS_LEFT_ROTATE (0x03)
// 右旋即用户正面面对旋钮模组屏顺时针旋转
#define COMM_OPT_BTN_OR_KNOBS_RIGHT_ROTATE (0x04)

#define COMM_MAX_RESEND_TIMES (3)
#define COMM_MAX_DELAY_TIME (50*2)
#define COMM_SEND_TIME_INTERVAL (50)
#define COMM_DELAY_TIME_INTERVAL (10)

#define TEMPERATURE_NORMAL_VALUE (0xFFFF)

#define IMAGES_DILUTE_SET_0_SERIAL_NUMBER   (0x0)
#define IMAGES_DILUTE_SET_1_SERIAL_NUMBER   (0x1)
#define IMAGES_DILUTE_SET_2_SERIAL_NUMBER   (0x2)
#define IMAGES_DILUTE_SET_3_SERIAL_NUMBER   (0x3)
#define IMAGES_DILUTE_SET_4_SERIAL_NUMBER   (0x4)
#define IMAGES_DILUTE_SET_5_SERIAL_NUMBER   (0x5)
#define IMAGES_DILUTE_SET_6_SERIAL_NUMBER   (0x6)
#define IMAGES_DILUTE_SET_7_SERIAL_NUMBER   (0x7)
#define IMAGES_DILUTE_SET_8_SERIAL_NUMBER   (0x8)
#define IMAGES_DILUTE_SET_9_SERIAL_NUMBER   (0x9)
#define IMAGES_DILUTE_SET_PERCENT_SERIAL_NUMBER   (0xa)
#define IMAGES_MAIN_PICTURE_BACKGROUND_BLACK_SERIAL_NUMBER   (0xb)
#define IMAGES_MAIN_PICTURE_BOTTOM_MENU_SERIAL_NUMBER   (0xc)
#define IMAGES_MAIN_PICTURE_CIRCLE_BLUE_SERIAL_NUMBER   (0xd)
#define IMAGES_MAIN_PICTURE_CIRCLE_GREEN_SERIAL_NUMBER   (0xe)
#define IMAGES_MAIN_PICTURE_CIRCLE_RED_SERIAL_NUMBER   (0xf)
#define IMAGES_MAIN_PICTURE_CIRCLE_YELLOW_SERIAL_NUMBER   (0x10)
#define IMAGES_MAIN_PICTURE_DILUTE_RATIO_SERIAL_NUMBER   (0x11)
#define IMAGES_MAIN_PICTURE_DILUTE_SET_BG_SERIAL_NUMBER   (0x12)
#define IMAGES_MAIN_PICTURE_STATUS_SERIAL_NUMBER   (0x13)
#define IMAGES_MAIN_PICTURE_STATUS_SET_BG_SERIAL_NUMBER   (0x14)
#define IMAGES_MAIN_PICTURE_TEMPERATURE_SERIAL_NUMBER   (0x15)
#define IMAGES_MAIN_PICTURE_TEMPERATURE_SET_BG_SERIAL_NUMBER   (0x16)
#define IMAGES_MAIN_PICTURE_TOTAL_CAPACITY_SERIAL_NUMBER   (0x17)
#define IMAGES_MAIN_PICTURE_TOTAL_SET_BG_SERIAL_NUMBER   (0x18)
#define IMAGES_MEASURE_DATA_0_SERIAL_NUMBER   (0x19)
#define IMAGES_MEASURE_DATA_1_SERIAL_NUMBER   (0x1a)
#define IMAGES_MEASURE_DATA_2_SERIAL_NUMBER   (0x1b)
#define IMAGES_MEASURE_DATA_3_SERIAL_NUMBER   (0x1c)
#define IMAGES_MEASURE_DATA_4_SERIAL_NUMBER   (0x1d)
#define IMAGES_MEASURE_DATA_5_SERIAL_NUMBER   (0x1e)
#define IMAGES_MEASURE_DATA_6_SERIAL_NUMBER   (0x1f)
#define IMAGES_MEASURE_DATA_7_SERIAL_NUMBER   (0x20)
#define IMAGES_MEASURE_DATA_8_SERIAL_NUMBER   (0x21)
#define IMAGES_MEASURE_DATA_9_SERIAL_NUMBER   (0x22)
#define IMAGES_MEASURE_DATA_CUP_SERIAL_NUMBER   (0x23)
#define IMAGES_SETTING_CLEAR_SET_SERIAL_NUMBER   (0x24)
#define IMAGES_SETTING_DILUTE_SET_SERIAL_NUMBER   (0x25)
#define IMAGES_SETTING_RESET_SET_SERIAL_NUMBER   (0x26)
#define IMAGES_SETTING_TEMPERATURE_SET_SERIAL_NUMBER   (0x27)
#define IMAGES_SETTING_TIME_SET_SERIAL_NUMBER   (0x28)
#define IMAGES_SETTING_TOTAL_SET_SERIAL_NUMBER   (0x29)
#define IMAGES_STATUS_HEAT_FAIL_SERIAL_NUMBER   (0x2a)
#define IMAGES_STATUS_JIAZHU_FAIL_SERIAL_NUMBER   (0x2b)
#define IMAGES_STATUS_LOW_WATER_SERIAL_NUMBER   (0x2c)
#define IMAGES_STATUS_NO_CUP_SERIAL_NUMBER   (0x2d)
#define IMAGES_STATUS_OUT_CUP_SERIAL_NUMBER   (0x2e)
#define IMAGES_STATUS_POSITION_ERR_SERIAL_NUMBER   (0x2f)
#define IMAGES_STATUS_RECYCLE_SERIAL_NUMBER   (0x30)
#define IMAGES_STATUS_RESET_SERIAL_NUMBER   (0x31)
#define IMAGES_STATUS_STANDBY_SERIAL_NUMBER   (0x32)
#define IMAGES_STATUS_THROW_CUP_ERR_SERIAL_NUMBER   (0x33)
#define IMAGES_TEMPERATURE_SET_BLACK_0_SERIAL_NUMBER   (0x34)
#define IMAGES_TEMPERATURE_SET_BLACK_1_SERIAL_NUMBER   (0x35)
#define IMAGES_TEMPERATURE_SET_BLACK_2_SERIAL_NUMBER   (0x36)
#define IMAGES_TEMPERATURE_SET_BLACK_3_SERIAL_NUMBER   (0x37)
#define IMAGES_TEMPERATURE_SET_BLACK_4_SERIAL_NUMBER   (0x38)
#define IMAGES_TEMPERATURE_SET_BLACK_5_SERIAL_NUMBER   (0x39)
#define IMAGES_TEMPERATURE_SET_BLACK_6_SERIAL_NUMBER   (0x3a)
#define IMAGES_TEMPERATURE_SET_BLACK_7_SERIAL_NUMBER   (0x3b)
#define IMAGES_TEMPERATURE_SET_BLACK_8_SERIAL_NUMBER   (0x3c)
#define IMAGES_TEMPERATURE_SET_BLACK_9_SERIAL_NUMBER   (0x3d)
#define IMAGES_TEMPERATURE_SET_BLUE_0_SERIAL_NUMBER   (0x3e)
#define IMAGES_TEMPERATURE_SET_BLUE_1_SERIAL_NUMBER   (0x3f)
#define IMAGES_TEMPERATURE_SET_BLUE_2_SERIAL_NUMBER   (0x40)
#define IMAGES_TEMPERATURE_SET_BLUE_3_SERIAL_NUMBER   (0x41)
#define IMAGES_TEMPERATURE_SET_BLUE_4_SERIAL_NUMBER   (0x42)
#define IMAGES_TEMPERATURE_SET_BLUE_5_SERIAL_NUMBER   (0x43)
#define IMAGES_TEMPERATURE_SET_BLUE_6_SERIAL_NUMBER   (0x44)
#define IMAGES_TEMPERATURE_SET_BLUE_7_SERIAL_NUMBER   (0x45)
#define IMAGES_TEMPERATURE_SET_BLUE_8_SERIAL_NUMBER   (0x46)
#define IMAGES_TEMPERATURE_SET_BLUE_9_SERIAL_NUMBER   (0x47)
#define IMAGES_TEMPERATURE_SET_CELSIUS_BLACK_SERIAL_NUMBER   (0x48)
#define IMAGES_TEMPERATURE_SET_CELSIUS_BLUE_SERIAL_NUMBER   (0x49)
#define IMAGES_TEMPERATURE_SET_NORMAL_BLACK_SERIAL_NUMBER   (0x4a)
#define IMAGES_TEMPERATURE_SET_NORMAL_BLUE_SERIAL_NUMBER   (0x4b)
#define IMAGES_TEMPERATURE_SET_NORMAL_SET_BG_SERIAL_NUMBER   (0x4c)
#define IMAGES_TEMPERATURE_SET_TEMPERATURE_SET_BG_SERIAL_NUMBER   (0x4d)
#define IMAGES_TEMPERATURE_SHOW_0_SERIAL_NUMBER   (0x4e)
#define IMAGES_TEMPERATURE_SHOW_1_SERIAL_NUMBER   (0x4f)
#define IMAGES_TEMPERATURE_SHOW_2_SERIAL_NUMBER   (0x50)
#define IMAGES_TEMPERATURE_SHOW_3_SERIAL_NUMBER   (0x51)
#define IMAGES_TEMPERATURE_SHOW_4_SERIAL_NUMBER   (0x52)
#define IMAGES_TEMPERATURE_SHOW_5_SERIAL_NUMBER   (0x53)
#define IMAGES_TEMPERATURE_SHOW_6_SERIAL_NUMBER   (0x54)
#define IMAGES_TEMPERATURE_SHOW_7_SERIAL_NUMBER   (0x55)
#define IMAGES_TEMPERATURE_SHOW_8_SERIAL_NUMBER   (0x56)
#define IMAGES_TEMPERATURE_SHOW_9_SERIAL_NUMBER   (0x57)
#define IMAGES_TEMPERATURE_SHOW_CELSIUS_SERIAL_NUMBER   (0x58)
#define IMAGES_TEMPERATURE_SHOW_NORMAL_TEMP_SERIAL_NUMBER   (0x59)
#define IMAGES_TIME_0_SERIAL_NUMBER   (0x5a)
#define IMAGES_TIME_1_SERIAL_NUMBER   (0x5b)
#define IMAGES_TIME_2_SERIAL_NUMBER   (0x5c)
#define IMAGES_TIME_3_SERIAL_NUMBER   (0x5d)
#define IMAGES_TIME_4_SERIAL_NUMBER   (0x5e)
#define IMAGES_TIME_5_SERIAL_NUMBER   (0x5f)
#define IMAGES_TIME_6_SERIAL_NUMBER   (0x60)
#define IMAGES_TIME_7_SERIAL_NUMBER   (0x61)
#define IMAGES_TIME_8_SERIAL_NUMBER   (0x62)
#define IMAGES_TIME_9_SERIAL_NUMBER   (0x63)
#define IMAGES_TIME_DELIMITER_SERIAL_NUMBER   (0x64)
#define IMAGES_TOTAL_SET_0_SERIAL_NUMBER   (0x65)
#define IMAGES_TOTAL_SET_1_SERIAL_NUMBER   (0x66)
#define IMAGES_TOTAL_SET_2_SERIAL_NUMBER   (0x67)
#define IMAGES_TOTAL_SET_3_SERIAL_NUMBER   (0x68)
#define IMAGES_TOTAL_SET_4_SERIAL_NUMBER   (0x69)
#define IMAGES_TOTAL_SET_5_SERIAL_NUMBER   (0x6a)
#define IMAGES_TOTAL_SET_6_SERIAL_NUMBER   (0x6b)
#define IMAGES_TOTAL_SET_7_SERIAL_NUMBER   (0x6c)
#define IMAGES_TOTAL_SET_8_SERIAL_NUMBER   (0x6d)
#define IMAGES_TOTAL_SET_9_SERIAL_NUMBER   (0x6e)
#define IMAGES_TOTAL_SET_ML_SERIAL_NUMBER   (0x6f)

#define IMAGES_DILUTE_SERIAL_NUMBER(_num_)  (IMAGES_DILUTE_SET_##_num_##_SERIAL_NUMBER)
#define IMAGES_MEASURE_SERIAL_NUMBER(_num_)  (IMAGES_MEASURE_DATA_##_num_##_SERIAL_NUMBER)
#define IMAGES_TEMPERATURE_BLACK_SERIAL_NUMBER(_num_)  (IMAGES_TEMPERATURE_SET_BLACK_##_num_##_SERIAL_NUMBER)
#define IMAGES_TEMPERATURE_BLUE_SERIAL_NUMBER(_num_)  (IMAGES_TEMPERATURE_SET_BLUE_##_num_##_SERIAL_NUMBER)
#define IMAGES_TEMPERATURE_SHOW_SERIAL_NUMBER(_num_)  (IMAGES_TEMPERATURE_SHOW_##_num_##_SERIAL_NUMBER)
#define IMAGES_TIME_SHOW_SERIAL_NUMBER(_num_)  (IMAGES_TIME_##_num_##_SERIAL_NUMBER)
#define IMAGES_TOTAL_SET_SERIAL_NUMBER(_num_)  (IMAGES_TOTAL_SET_##_num_##_SERIAL_NUMBER)

/** 图像数字类型枚举 */
typedef enum
{
    IMAGE_TYPE_DILUTE_SET,
    IMAGE_TYPE_MEASURE_DATA,
    IMAGE_TYPE_TEMPERATURE_SET_BLACK,
    IMAGE_TYPE_TEMPERATURE_SET_BLUE,
    IMAGE_TYPE_TEMPERATURE_SHOW,
    IMAGE_TYPE_TIME_SHOW,
    IMAGE_TYPE_TOTAL_SET,
} image_type_e;

/** 与旋钮屏通信协议命令码定义 */
typedef enum
{
    // host to slave need ack
    COMM_CONN_DISCONNECT_CMD = 0x01,//
    COMM_FILL_BG_AND_ICON_CMD = 0x02,//
    COMM_FILL_AREA_COLOR_CMD = 0x03,//
    COMM_CRTL_SCREEN_DISPLAY_CMD = 0x04,//
    COMM_PREPARE_BIN_SEND_CMD = 0x05,//
    COMM_GET_SLAVE_SOFTWARE_VER_CMD = 0x06,//

    // slave to host need ack
    COMM_SOFTWARE_VER_REPORT_CMD = 0xF1,//
    COMM_OPT_BTN_OR_KNOBS_CMD = 0xF2,//
    COMM_FILL_BG_AND_ICON_COMPLETE_CMD = 0xF3,//

    COMM_NACK_SYSTEM_START_OR_NOT_CONN = 0xFA,//
    COMM_NACK_BUSY = 0xFB,//
    COMM_NACK_NOT_SUPPORT = 0xFC,//
    COMM_NACK_CHECKSUM_ERROR = 0xFD,//不需要响应校验错误
    COMM_ACK_NORMAL = 0xFE,//响应正常
    COMM_RESPONSE_CMD = 0xFF,//响应命令
} comm_cmd_e;

/** 与旋钮屏通信交互状态 */
typedef enum
{
    COMM_UNCONN_STAGE,//未连接初始状态阶段
    COMM_INIT_CONN_STAGE,//发起连接状态阶段
    COMM_FILL_BG_AND_IMAGE_STAGE,//填图状态阶段
    COMM_FILL_AREA_COLOR_STAGE,//填充颜色状态阶段
    COMM_CRTL_SCREEN_DISPLAY_STAGE,//控制屏显状态阶段
    COMM_GET_SLAVE_SOFTWARE_VER_STAGE,//获取屏显信息状态阶段
    COMM_CONN_STAGE,//已连接状态阶段
    COMM_DIS_CONN_STAGE,//断开连接状态阶段
} comm_status_stage_e;

/** 屏显通信结构体 */
typedef struct
{
    uint8_t head; /**< must be "0xAA" */
    uint8_t cmd; /**< command type */
    uint8_t payload_len; /**< the length of payload */
    uint8_t payload[0]; /**< the payload */
} __attribute__((packed)) comm_msg_t;

/** 图标信息结构体
 * 注意填入图的先后顺序 背景图最先发送后再发送图标
 * 图标序号为图片命名排列需要0-999
 * 因为长度占一个字节 当超过时可分批次重复发送
 */
typedef struct
{
    unsigned char figure_no[2]; /** big-end */
    unsigned char x_coordinate[2]; /**< big-end */
    unsigned char y_coordinate[2]; /**< big-end */
} __attribute__((packed)) figure_msg_t;

/** 区域颜色结构体 */
typedef struct
{
    unsigned char color[2]; /** big-end */
    unsigned char x_coordinate[2]; /**< big-end */
    unsigned char y_coordinate[2]; /**< big-end */
    unsigned char width[2]; /**< big-end */
    unsigned char height[2]; /**< big-end */
} __attribute__((packed)) area_color_msg_t;


/* function declarations */
uint8_t lcd_conn_opt(uint8_t conn);
uint8_t lcd_fill_bg_and_icon_cmd(figure_msg_t *figure, uint8_t figure_num);
uint8_t lcd_fill_area_color_cmd(area_color_msg_t area_color);
uint8_t lcd_screen_display_ctrl(uint8_t on_off);
uint8_t lcd_get_screen_info_cmd(void);
void controller_msg_process(void);
void notice_flash(uint8_t idx);
void lcd_status_display( void );
uint16_t lcd_get_image_serial_number(uint8_t num, uint8_t num_type);
void lcd_time_display(uint32_t timestamp);
void lcd_total_volume_display(uint16_t volume);
void lcd_dilute_ratio_display(uint16_t ratio);
void lcd_running_status_display(uint16_t status_sn);
void lcd_temperature_display(uint16_t temp);
void lcd_cup_num_display(uint16_t cups);
void lcd_setting_display(uint16_t set_sn);
void lcd_main_bg_display(void);
void lcd_main_circle_bg_display(void);
void lcd_main_menu_bg_display(void);
void lcd_main_middle_bg_display(void);
void lcd_main_cup_bg_display(void);
void lcd_init_display(void);
void lcd_display_update(void);

#endif /* LCD_H */
