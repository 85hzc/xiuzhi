/*!
    \file    lcd.c
    \brief   
*/

#include "global.h"

void notice_flash(uint8_t idx)
{
    if (idx == POSITION_ERROR) {
        //显示“走位错误”
    } else if (idx == WUBEI_ERROR) {
        //显示“无纸杯”
    } else if (idx == QIBEI_ERROR) {
        //显示“弃杯错误”
    } else if (idx == ZHUSHUI_ERROR) {
        //显示“加注失败”
    } else if (idx == WENDU_ERROR) {
        //显示“温度失控”
    } else if (idx == SHUIWEI_ERROR) {
        //显示“水位不足”
    } else if (idx == RESET_) {
        //显示“复位”
    } else if (idx == READY_) {
        //显示“待机”
    } else if (idx == CHUBEI_) {
        //显示“出杯”
    } else if (idx == HUISHOU_) {
        //显示“回收”
    }
}

void lcd_status_display( void )
{
    uint8_t loop = 0;
    static uint8_t next_warnning = 0;

    loop = next_warnning;
    while (!(error_bits_flag & 1<<loop)) {
        loop++;
        if (loop == ERROR_max) {
            next_warnning = 0;
            return;
        }
    }

    if (error_bits_flag & 1<<loop) {
        notice_flash(loop);
        next_warnning = loop+1;
    }
}
