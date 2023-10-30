/*!
    \file    lcd.c
    \brief   
*/

#include "global.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

static uint8_t sg_lcd_stage = COMM_UNCONN_STAGE;
static uint8_t sg_lcd_status = COMM_ACK_NORMAL;

extern void usart2_data_transfer(uint8_t *usart_data, uint8_t len);
extern void report_operation_handle(uint8_t opt);


static void _u16_2_byte2_big_endian(unsigned short value, unsigned char *buf)
{
    buf[0] = (unsigned char)((value >> 8) & 0xFF);
    buf[1] = (unsigned char)(value & 0xFF);
}

#if 0
static unsigned short _byte2_big_endian_2_u16(unsigned char *buf)
{
    unsigned short value = (buf[0] << 8) | buf[1];
    return value;
}

static uint8_t _chk_CRC(unsigned char crc, unsigned char *data) {
    if (data[0] == crc)
    {
        return 1;
    }

    return 0;
}
#endif // 0

static uint8_t _calc_CRC(const uint8_t* data, size_t length) {
    uint32_t sum = 0;
    uint8_t crc = 0;

    for (size_t i = 0; i < length; i++) {
        sum += data[i];
    }

    crc = ((uint8_t)sum) ^ 0xFF;
    // printf("_calc_CRC length:%02x sum:%04x crc:%02x\n", length, sum, crc);
    return crc;
}

static uint8_t lcd_check_conn_status()
{
    if (sg_lcd_stage == COMM_UNCONN_STAGE
        || sg_lcd_stage == COMM_DIS_CONN_STAGE)
    {
        return COMM_DISCONN;
    }

    return COMM_CONN;
}

uint8_t lcd_conn_opt(uint8_t conn)
{
    printf("fun:%s line:%d conn:%d\n", __FUNCTION__, __LINE__,conn);
    // comm_msg_t's len + conn/disconn data's len + 1(crc)
    int msg_len = sizeof(comm_msg_t) + sizeof(uint8_t) + 1;
    comm_msg_t *pMsg = (comm_msg_t *)calloc(1, msg_len);
    if (!pMsg)
    {
        printf("fun:%s line:%d calloc comm_msg_t fail\n", __FUNCTION__, __LINE__);
        return 0;
    }

    pMsg->head = COMM_HEADER;
    // conn/disconn data's len
    pMsg->payload_len = sizeof(uint8_t);
    pMsg->cmd = COMM_CONN_DISCONNECT_CMD;
    memcpy(pMsg->payload, &conn, sizeof(uint8_t));
    uint8_t *pbuf = (uint8_t *)(&pMsg->cmd);
    // crc not include head and crc len
    uint8_t crc = _calc_CRC(pbuf, msg_len-1-1);
    pbuf = pbuf + pMsg->payload_len+2;
    *pbuf = crc;

    // pbuf = (uint8_t *)pMsg;
    // printf("send data:\n");
    // for (size_t i = 0; i < msg_len; i++)
    // {
    //     printf("%02x ", pbuf[i]);
    // }
    // printf("\n");

    usart2_data_transfer((uint8_t *)pMsg, msg_len);

#if 0

    int32_t time_send = 0;
    int32_t delay_t = COMM_MAX_DELAY_TIME;
    while (delay_t > 0)
    {
        if (time_send >= COMM_SEND_TIME_INTERVAL)
        {
            time_send = 0;
            usart2_data_transfer((uint8_t *)pMsg, msg_len);
        }

        delay_1ms(COMM_DELAY_TIME_INTERVAL);
        if (sg_lcd_status == COMM_ACK_NORMAL)
        {
            break;
        }
        delay_t -= COMM_DELAY_TIME_INTERVAL;
        time_send += COMM_DELAY_TIME_INTERVAL;
    }

    free(pMsg);

    if (delay_t <= 0)
    {
        printf("fun:%s line:%d conn:%d knob screen operation fail\n", __FUNCTION__, __LINE__, conn);
        return 0;
    }
#endif // 0

    free(pMsg);

    return 1;
}

/**
 * 为简单建议一次只发送一个figure信息 这样可以避免信息过长导致错误
 * 如需显示多图片可多次调用
*/
uint8_t lcd_fill_bg_and_icon_cmd(figure_msg_t *figure, uint8_t figure_num)
{
    printf("fun:%s line:%d figure_num:%d\n", __FUNCTION__, __LINE__,figure_num);
    // comm_msg_t's len + figure_msg_t's len + 1(crc)
    int msg_len = sizeof(comm_msg_t) + sizeof(figure_msg_t) * figure_num + 1;
    comm_msg_t *pMsg = (comm_msg_t *)calloc(1, msg_len);
    if (!pMsg)
    {
        printf("fun:%s line:%d calloc comm_msg_t fail", __FUNCTION__, __LINE__);
        return 0;
    }

    pMsg->head = COMM_HEADER;
    // figure_msg_t's len
    pMsg->payload_len = sizeof(figure_msg_t) * figure_num;
    pMsg->cmd = COMM_FILL_BG_AND_ICON_CMD;
    memcpy(pMsg->payload, figure, pMsg->payload_len);
    uint8_t *pbuf = (uint8_t *)(&pMsg->cmd);
    // crc not include head and crc len
    uint8_t crc = _calc_CRC(pbuf, msg_len-1-1);
    pbuf = pbuf + pMsg->payload_len+2;
    *pbuf = crc;

    // pbuf = (uint8_t *)pMsg;
    // printf("send data:\n");
    // for (size_t i = 0; i < msg_len; i++)
    // {
    //     printf("%02x ", pbuf[i]);
    // }
    // printf("\n");

    usart2_data_transfer((uint8_t *)pMsg, msg_len);

#if 0
    int32_t time_send = 0;
    int32_t delay_t = COMM_MAX_DELAY_TIME;
    while (delay_t > 0)
    {
        if (time_send >= COMM_SEND_TIME_INTERVAL)
        {
            time_send = 0;
            usart2_data_transfer((uint8_t *)pMsg, msg_len);
        }

        delay_1ms(COMM_DELAY_TIME_INTERVAL);
        if (sg_lcd_status == COMM_ACK_NORMAL)
        {
            break;
        }
        delay_t -= COMM_DELAY_TIME_INTERVAL;
        time_send += COMM_DELAY_TIME_INTERVAL;
    }

    free(pMsg);

    if (delay_t <= 0)
    {
        printf("fun:%s line:%d\n", __FUNCTION__, __LINE__);
        return 0;
    }
#endif // 0

    free(pMsg);

    return 1;
}

uint8_t lcd_fill_area_color_cmd(area_color_msg_t area_color)
{
    printf("fun:%s line:%d\n", __FUNCTION__, __LINE__);
    // comm_msg_t's len + area_color_msg_t's len + 1(crc)
    int msg_len = sizeof(comm_msg_t) + sizeof(area_color_msg_t) + 1;
    comm_msg_t *pMsg = (comm_msg_t *)calloc(1, msg_len);
    if (!pMsg)
    {
        printf("fun:%s line:%d calloc comm_msg_t fail", __FUNCTION__, __LINE__);
        return 0;
    }

    pMsg->head = COMM_HEADER;
    // area_color_msg_t's len
    pMsg->payload_len = sizeof(area_color_msg_t);
    pMsg->cmd = COMM_FILL_AREA_COLOR_CMD;
    memcpy(pMsg->payload, &area_color, sizeof(area_color_msg_t));
    uint8_t *pbuf = (uint8_t *)(&pMsg->cmd);
    // crc not include head and crc len
    uint8_t crc = _calc_CRC(pbuf, msg_len-1-1);
    pbuf = pbuf + pMsg->payload_len+2;
    *pbuf = crc;

    // pbuf = (uint8_t *)pMsg;
    // printf("send data:\n");
    // for (size_t i = 0; i < msg_len; i++)
    // {
    //     printf("%02x ", pbuf[i]);
    // }
    // printf("\n");

    usart2_data_transfer((uint8_t *)pMsg, msg_len);

#if 0
    int32_t time_send = 0;
    int32_t delay_t = COMM_MAX_DELAY_TIME;
    while (delay_t > 0)
    {
        if (time_send >= COMM_SEND_TIME_INTERVAL)
        {
            time_send = 0;
            usart2_data_transfer((uint8_t *)pMsg, msg_len);
        }

        delay_1ms(COMM_DELAY_TIME_INTERVAL);
        if (sg_lcd_status == COMM_ACK_NORMAL)
        {
            break;
        }
        delay_t -= COMM_DELAY_TIME_INTERVAL;
        time_send += COMM_DELAY_TIME_INTERVAL;
    }

    free(pMsg);

    if (delay_t <= 0)
    {
        printf("fun:%s line:%d\n", __FUNCTION__, __LINE__);
        return 0;
    }
#endif // 0

    free(pMsg);

    return 1;
}

uint8_t lcd_screen_display_ctrl(uint8_t on_off)
{
    printf("fun:%s line:%d on_off:%d\n", __FUNCTION__, __LINE__,on_off);
    // comm_msg_t's len + on_off data's len + 1(crc)
    int msg_len = sizeof(comm_msg_t) + sizeof(uint8_t) + 1;
    comm_msg_t *pMsg = (comm_msg_t *)calloc(1, msg_len);
    if (!pMsg)
    {
        printf("fun:%s line:%d calloc comm_msg_t fail", __FUNCTION__, __LINE__);
        return 0;
    }

    pMsg->head = COMM_HEADER;
    // on_off data's len
    pMsg->payload_len = sizeof(uint8_t);
    pMsg->cmd = COMM_CRTL_SCREEN_DISPLAY_CMD;
    memcpy(pMsg->payload, &on_off, sizeof(uint8_t));
    uint8_t *pbuf = (uint8_t *)(&pMsg->cmd);
    // crc not include head and crc len
    uint8_t crc = _calc_CRC(pbuf, msg_len-1-1);
    pbuf = pbuf + pMsg->payload_len+2;
    *pbuf = crc;

    // pbuf = (uint8_t *)pMsg;
    // printf("send data:\n");
    // for (size_t i = 0; i < msg_len; i++)
    // {
    //     printf("%02x ", pbuf[i]);
    // }
    // printf("\n");

    usart2_data_transfer((uint8_t *)pMsg, msg_len);

#if 0
    int32_t time_send = 0;
    int32_t delay_t = COMM_MAX_DELAY_TIME;
    while (delay_t > 0)
    {
        if (time_send >= COMM_SEND_TIME_INTERVAL)
        {
            time_send = 0;
            usart2_data_transfer((uint8_t *)pMsg, msg_len);
        }

        delay_1ms(COMM_DELAY_TIME_INTERVAL);
        if (sg_lcd_status == COMM_ACK_NORMAL)
        {
            break;
        }
        delay_t -= COMM_DELAY_TIME_INTERVAL;
        time_send += COMM_DELAY_TIME_INTERVAL;
    }

    free(pMsg);

    if (delay_t <= 0)
    {
        printf("fun:%s line:%d\n", __FUNCTION__, __LINE__);
        return 0;
    }
#endif // 0

    free(pMsg);

    return 1;
}

uint8_t lcd_get_screen_info_cmd()
{
    printf("fun:%s line:%d\n", __FUNCTION__, __LINE__);
    uint8_t def_payload = 0;
    // comm_msg_t's len + default data's len + 1(crc)
    int msg_len = sizeof(comm_msg_t) + sizeof(uint8_t) + 1;
    comm_msg_t *pMsg = (comm_msg_t *)calloc(1, msg_len);
    if (!pMsg)
    {
        printf("fun:%s line:%d calloc comm_msg_t fail", __FUNCTION__, __LINE__);
        return 0;
    }

    pMsg->head = COMM_HEADER;
    // default data's len
    pMsg->payload_len = sizeof(uint8_t);
    pMsg->cmd = COMM_GET_SLAVE_SOFTWARE_VER_CMD;
    memcpy(pMsg->payload, &def_payload, sizeof(uint8_t));
    uint8_t *pbuf = (uint8_t *)(&pMsg->cmd);
    // crc not include head and crc len
    uint8_t crc = _calc_CRC(pbuf, msg_len-1-1);
    pbuf = pbuf + pMsg->payload_len+2;
    *pbuf = crc;

    // pbuf = (uint8_t *)pMsg;
    // printf("send data:\n");
    // for (size_t i = 0; i < msg_len; i++)
    // {
    //     printf("%02x ", pbuf[i]);
    // }
    // printf("\n");

    usart2_data_transfer((uint8_t *)pMsg, msg_len);

#if 0
    int32_t time_send = 0;
    int32_t delay_t = COMM_MAX_DELAY_TIME;
    while (delay_t > 0)
    {
        if (time_send >= COMM_SEND_TIME_INTERVAL)
        {
            time_send = 0;
            usart2_data_transfer((uint8_t *)pMsg, msg_len);
        }

        delay_1ms(COMM_DELAY_TIME_INTERVAL);
        if (sg_lcd_status == COMM_ACK_NORMAL)
        {
            break;
        }
        delay_t -= COMM_DELAY_TIME_INTERVAL;
        time_send += COMM_DELAY_TIME_INTERVAL;
    }

    free(pMsg);

    if (delay_t <= 0)
    {
        printf("fun:%s line:%d\n", __FUNCTION__, __LINE__);
        return 0;
    }
#endif // 0

    free(pMsg);

    return 1;
}

void controller_msg_process(void)
{
    uint8_t head;
    uint8_t cmd;
    uint8_t payload_len;
    uint8_t msg[COM_BUFFER_SIZE] = {0};
    uint8_t *pbuf = msg;
    uint8_t cnt = 0;
    uint8_t opt = 0;

    if (!buffer_read(&head) || head != COMM_HEADER)
    {
        // printf("fun:%s line:%d invalid header\r\n",__FUNCTION__,__LINE__);
        return;
    }

    *pbuf = head;
    pbuf++;
    if (!buffer_read(&cmd) || cmd < COMM_SOFTWARE_VER_REPORT_CMD)
    {
        printf("fun:%s line:%d invalid cmd[%x]\r\n",__FUNCTION__,__LINE__,cmd);
        return;
    }
    *pbuf = cmd;
    pbuf++;
    // 经多次验证cmd为COMM_FILL_BG_AND_ICON_COMPLETE_CMD时,数据不按照协议实现 故需做特殊处理
    if (COMM_FILL_BG_AND_ICON_COMPLETE_CMD == cmd)
    {
        cnt = buffer_reads(pbuf, 3);
        if (cnt == 3 && pbuf[0] == 0xF2 && pbuf[1] == 0x00 && pbuf[2] == 0x1A)
        {
            lcd_screen_display_ctrl(COMM_CTRL_DISPLAY_ON);
        }
        return;
    }

    if (!buffer_read(&payload_len))
    {
        printf("fun:%s line:%d invalid payload len\r\n",__FUNCTION__,__LINE__);
        return;
    }
    *pbuf = payload_len;
    pbuf++;
    // get payload info and one byte crc
    cnt = buffer_reads(pbuf, payload_len+1);

    /*printf("\n recv [%d] data:\n", cnt+3);
    for (size_t i = 0; i < cnt+3; i++)
    {
        printf("%02x ", msg[i]);
    }
    printf("\n");*/

    if (cnt < (payload_len+1))
    {
        printf("fun:%s line:%d recv invalid payload_len:%d data\r\n",__FUNCTION__,__LINE__,payload_len);
        return;
    }

    comm_msg_t *pdata = (comm_msg_t *)msg;
    // crc not include head and crc len
    uint8_t crc = _calc_CRC((uint8_t *)&pdata->cmd, payload_len+1+1);
    pbuf = (uint8_t *)(&pdata->cmd);
    pbuf = pbuf + payload_len+1+1;
    uint8_t buf_crc = *pbuf;
    if (crc != buf_crc)
    {
        printf("fun:%s line:%d calc_crc:%x buf_crc:%x\r\n",__FUNCTION__,__LINE__,crc,buf_crc);
        return;
    }

    if (COMM_HEADER == head)
    {
        switch (cmd)
        {
        case COMM_RESPONSE_CMD:
            sg_lcd_status = pdata->payload[0];
            //printf("fun:%s line:%d sg_lcd_status:%x\n",__FUNCTION__,__LINE__,sg_lcd_status);
            if (sg_lcd_status == COMM_ACK_NORMAL)
            {
                // if (sg_lcd_stage == COMM_UNCONN_STAGE)
                // {
                //     // sg_lcd_stage = COMM_INIT_CONN_STAGE;
                //     sg_lcd_stage = COMM_FILL_MAIN_BG_STAGE;
                // }
                // else if (sg_lcd_stage == COMM_DIS_CONN_STAGE)
                // {
                //     sg_lcd_stage = COMM_CONN_STAGE;
                // }
                switch (sg_lcd_stage)
                {
                case COMM_UNCONN_STAGE:
                    sg_lcd_stage = COMM_FILL_MAIN_BG_STAGE;
                    break;
                case COMM_FILL_MAIN_BG_STAGE:
                    sg_lcd_stage = COMM_FILL_MAIN_CIRCLE_BG_STAGE;
                    break;
                case COMM_FILL_MAIN_CIRCLE_BG_STAGE:
                    sg_lcd_stage = COMM_FILL_MAIN_MENU_BG_STAGE;
                    break;
                case COMM_FILL_MAIN_MENU_BG_STAGE:
                    sg_lcd_stage = COMM_FILL_MAIN_MIDDLE_BG_STAGE;
                    break;
                case COMM_FILL_MAIN_MIDDLE_BG_STAGE:
                    sg_lcd_stage = COMM_FILL_MAIN_CUP_BG_STAGE;
                    break;
                case COMM_FILL_MAIN_CUP_BG_STAGE:
                    sg_lcd_stage = COMM_CONN_STAGE;
                    break;
                case COMM_DIS_CONN_STAGE:
                    sg_lcd_stage = COMM_CONN_STAGE;
                    break;

                default:
                    break;
                }
            }

            // response_status_handle(status);
            break;
        case COMM_SOFTWARE_VER_REPORT_CMD:
            printf("Screen version info:%s\n",pdata->payload);
            break;
        case COMM_OPT_BTN_OR_KNOBS_CMD:
            opt = pdata->payload[0];
            printf("fun:%s line:%d operation:%x\n",__FUNCTION__,__LINE__,pdata->payload[0]);
            report_operation_handle(opt);
            break;
        case COMM_FILL_BG_AND_ICON_COMPLETE_CMD:
            printf("fill the graph ok\n");
            // refresh screen content when recv complete cmd
            lcd_screen_display_ctrl(COMM_CTRL_DISPLAY_ON);
            break;

        default:
            break;
        }
    }
}

void notice_flash(uint8_t idx)
{
    // printf("fun:%s line:%d idx:%d\n", __FUNCTION__, __LINE__,idx);
    if (idx == POSITION_ERROR) {
        //显示“走位错误”
        lcd_running_status_display(IMAGES_STATUS_POSITION_ERR_SERIAL_NUMBER);
    } else if (idx == WUBEI_ERROR) {
        //显示“无纸杯”
        lcd_running_status_display(IMAGES_STATUS_NO_CUP_SERIAL_NUMBER);
    } else if (idx == QIBEI_ERROR) {
        //显示“弃杯错误”
        lcd_running_status_display(IMAGES_STATUS_THROW_CUP_ERR_SERIAL_NUMBER);
    } else if (idx == ZHUSHUI_ERROR) {
        //显示“加注失败”
        lcd_running_status_display(IMAGES_STATUS_JIAZHU_FAIL_SERIAL_NUMBER);
    } else if (idx == WENDU_ERROR) {
        //显示“温度失控”
        lcd_running_status_display(IMAGES_STATUS_HEAT_FAIL_SERIAL_NUMBER);
    } else if (idx == SHUIWEI_ERROR) {
        //显示“水位不足”
        lcd_running_status_display(IMAGES_STATUS_LOW_WATER_SERIAL_NUMBER);
    } else if (idx == RESET_) {
        //显示“复位”
        lcd_running_status_display(IMAGES_STATUS_RESET_SERIAL_NUMBER);
    } else if (idx == READY_) {
        //显示“待机”
        lcd_running_status_display(IMAGES_STATUS_POSITION_ERR_SERIAL_NUMBER);
    } else if (idx == CHUBEI_) {
        //显示“出杯”
        lcd_running_status_display(IMAGES_STATUS_STANDBY_SERIAL_NUMBER);
    } else if (idx == HUISHOU_) {
        //显示“回收”
        lcd_running_status_display(IMAGES_STATUS_RECYCLE_SERIAL_NUMBER);
    }
}

void lcd_status_display( void )
{
    uint8_t loop = 0;
    static uint8_t next_warnning = 0;

    loop = next_warnning;
    while (!(error_bits_flag & 1<<loop)) {
        loop++;
        if (loop >= ERROR_max) {
            next_warnning = 0;
            return;
        }
    }

    if (error_bits_flag & 1<<loop) {
        notice_flash(loop);
        next_warnning = loop+1;
    }
}

#if 1
static uint16_t lcd_get_image_dilute_sn(uint8_t num)
{
    // uint16_t sn = -1;
    uint16_t sn = 0;
    switch (num)
    {
    case 0:
        sn = IMAGES_DILUTE_SET_0_SERIAL_NUMBER;
        break;
    case 1:
        sn = IMAGES_DILUTE_SET_1_SERIAL_NUMBER;
        break;
    case 2:
        sn = IMAGES_DILUTE_SET_2_SERIAL_NUMBER;
        break;
    case 3:
        sn = IMAGES_DILUTE_SET_3_SERIAL_NUMBER;
        break;
    case 4:
        sn = IMAGES_DILUTE_SET_4_SERIAL_NUMBER;
        break;
    case 5:
        sn = IMAGES_DILUTE_SET_5_SERIAL_NUMBER;
        break;
    case 6:
        sn = IMAGES_DILUTE_SET_6_SERIAL_NUMBER;
        break;
    case 7:
        sn = IMAGES_DILUTE_SET_7_SERIAL_NUMBER;
        break;
    case 8:
        sn = IMAGES_DILUTE_SET_8_SERIAL_NUMBER;
        break;
    case 9:
        sn = IMAGES_DILUTE_SET_9_SERIAL_NUMBER;
        break;

    default:
        break;
    }

    return sn;
}

static uint16_t lcd_get_image_measure_sn(uint8_t num)
{
    // uint16_t sn = -1;
    uint16_t sn = 0;
    switch (num)
    {
    case 0:
        sn = IMAGES_MEASURE_DATA_0_SERIAL_NUMBER;
        break;
    case 1:
        sn = IMAGES_MEASURE_DATA_1_SERIAL_NUMBER;
        break;
    case 2:
        sn = IMAGES_MEASURE_DATA_2_SERIAL_NUMBER;
        break;
    case 3:
        sn = IMAGES_MEASURE_DATA_3_SERIAL_NUMBER;
        break;
    case 4:
        sn = IMAGES_MEASURE_DATA_4_SERIAL_NUMBER;
        break;
    case 5:
        sn = IMAGES_MEASURE_DATA_5_SERIAL_NUMBER;
        break;
    case 6:
        sn = IMAGES_MEASURE_DATA_6_SERIAL_NUMBER;
        break;
    case 7:
        sn = IMAGES_MEASURE_DATA_7_SERIAL_NUMBER;
        break;
    case 8:
        sn = IMAGES_MEASURE_DATA_8_SERIAL_NUMBER;
        break;
    case 9:
        sn = IMAGES_MEASURE_DATA_9_SERIAL_NUMBER;
        break;

    default:
        break;
    }

    return sn;
}

static uint16_t lcd_get_image_temp_black_sn(uint8_t num)
{
    // uint16_t sn = -1;
    uint16_t sn = 0;
    switch (num)
    {
    case 0:
        sn = IMAGES_TEMPERATURE_SET_BLACK_0_SERIAL_NUMBER;
        break;
    case 1:
        sn = IMAGES_TEMPERATURE_SET_BLACK_1_SERIAL_NUMBER;
        break;
    case 2:
        sn = IMAGES_TEMPERATURE_SET_BLACK_2_SERIAL_NUMBER;
        break;
    case 3:
        sn = IMAGES_TEMPERATURE_SET_BLACK_3_SERIAL_NUMBER;
        break;
    case 4:
        sn = IMAGES_TEMPERATURE_SET_BLACK_4_SERIAL_NUMBER;
        break;
    case 5:
        sn = IMAGES_TEMPERATURE_SET_BLACK_5_SERIAL_NUMBER;
        break;
    case 6:
        sn = IMAGES_TEMPERATURE_SET_BLACK_6_SERIAL_NUMBER;
        break;
    case 7:
        sn = IMAGES_TEMPERATURE_SET_BLACK_7_SERIAL_NUMBER;
        break;
    case 8:
        sn = IMAGES_TEMPERATURE_SET_BLACK_8_SERIAL_NUMBER;
        break;
    case 9:
        sn = IMAGES_TEMPERATURE_SET_BLACK_9_SERIAL_NUMBER;
        break;

    default:
        break;
    }

    return sn;
}

static uint16_t lcd_get_image_temp_blue_sn(uint8_t num)
{
    // uint16_t sn = -1;
    uint16_t sn = 0;
    switch (num)
    {
    case 0:
        sn = IMAGES_TEMPERATURE_SET_BLUE_0_SERIAL_NUMBER;
        break;
    case 1:
        sn = IMAGES_TEMPERATURE_SET_BLUE_1_SERIAL_NUMBER;
        break;
    case 2:
        sn = IMAGES_TEMPERATURE_SET_BLUE_2_SERIAL_NUMBER;
        break;
    case 3:
        sn = IMAGES_TEMPERATURE_SET_BLUE_3_SERIAL_NUMBER;
        break;
    case 4:
        sn = IMAGES_TEMPERATURE_SET_BLUE_4_SERIAL_NUMBER;
        break;
    case 5:
        sn = IMAGES_TEMPERATURE_SET_BLUE_5_SERIAL_NUMBER;
        break;
    case 6:
        sn = IMAGES_TEMPERATURE_SET_BLUE_6_SERIAL_NUMBER;
        break;
    case 7:
        sn = IMAGES_TEMPERATURE_SET_BLUE_7_SERIAL_NUMBER;
        break;
    case 8:
        sn = IMAGES_TEMPERATURE_SET_BLUE_8_SERIAL_NUMBER;
        break;
    case 9:
        sn = IMAGES_TEMPERATURE_SET_BLUE_9_SERIAL_NUMBER;
        break;

    default:
        break;
    }

    return sn;
}

static uint16_t lcd_get_image_temp_show_sn(uint8_t num)
{
    // uint16_t sn = -1;
    uint16_t sn = 0;
    switch (num)
    {
    case 0:
        sn = IMAGES_TEMPERATURE_SHOW_0_SERIAL_NUMBER;
        break;
    case 1:
        sn = IMAGES_TEMPERATURE_SHOW_1_SERIAL_NUMBER;
        break;
    case 2:
        sn = IMAGES_TEMPERATURE_SHOW_2_SERIAL_NUMBER;
        break;
    case 3:
        sn = IMAGES_TEMPERATURE_SHOW_3_SERIAL_NUMBER;
        break;
    case 4:
        sn = IMAGES_TEMPERATURE_SHOW_4_SERIAL_NUMBER;
        break;
    case 5:
        sn = IMAGES_TEMPERATURE_SHOW_5_SERIAL_NUMBER;
        break;
    case 6:
        sn = IMAGES_TEMPERATURE_SHOW_6_SERIAL_NUMBER;
        break;
    case 7:
        sn = IMAGES_TEMPERATURE_SHOW_7_SERIAL_NUMBER;
        break;
    case 8:
        sn = IMAGES_TEMPERATURE_SHOW_8_SERIAL_NUMBER;
        break;
    case 9:
        sn = IMAGES_TEMPERATURE_SHOW_9_SERIAL_NUMBER;
        break;

    default:
        break;
    }

    return sn;
}

static uint16_t lcd_get_image_time_sn(uint8_t num)
{
    // uint16_t sn = -1;
    uint16_t sn = 0;
    switch (num)
    {
    case 0:
        sn = IMAGES_TIME_0_SERIAL_NUMBER;
        break;
    case 1:
        sn = IMAGES_TIME_1_SERIAL_NUMBER;
        break;
    case 2:
        sn = IMAGES_TIME_2_SERIAL_NUMBER;
        break;
    case 3:
        sn = IMAGES_TIME_3_SERIAL_NUMBER;
        break;
    case 4:
        sn = IMAGES_TIME_4_SERIAL_NUMBER;
        break;
    case 5:
        sn = IMAGES_TIME_5_SERIAL_NUMBER;
        break;
    case 6:
        sn = IMAGES_TIME_6_SERIAL_NUMBER;
        break;
    case 7:
        sn = IMAGES_TIME_7_SERIAL_NUMBER;
        break;
    case 8:
        sn = IMAGES_TIME_8_SERIAL_NUMBER;
        break;
    case 9:
        sn = IMAGES_TIME_9_SERIAL_NUMBER;
        break;

    default:
        break;
    }

    return sn;
}

static uint16_t lcd_get_image_total_sn(uint8_t num)
{
    // uint16_t sn = -1;
    uint16_t sn = 0;
    switch (num)
    {
    case 0:
        sn = IMAGES_TOTAL_SET_0_SERIAL_NUMBER;
        break;
    case 1:
        sn = IMAGES_TOTAL_SET_1_SERIAL_NUMBER;
        break;
    case 2:
        sn = IMAGES_TOTAL_SET_2_SERIAL_NUMBER;
        break;
    case 3:
        sn = IMAGES_TOTAL_SET_3_SERIAL_NUMBER;
        break;
    case 4:
        sn = IMAGES_TOTAL_SET_4_SERIAL_NUMBER;
        break;
    case 5:
        sn = IMAGES_TOTAL_SET_5_SERIAL_NUMBER;
        break;
    case 6:
        sn = IMAGES_TOTAL_SET_6_SERIAL_NUMBER;
        break;
    case 7:
        sn = IMAGES_TOTAL_SET_7_SERIAL_NUMBER;
        break;
    case 8:
        sn = IMAGES_TOTAL_SET_8_SERIAL_NUMBER;
        break;
    case 9:
        sn = IMAGES_TOTAL_SET_9_SERIAL_NUMBER;
        break;

    default:
        break;
    }

    return sn;
}
#endif // 0

uint16_t lcd_get_image_serial_number(uint8_t num, uint8_t num_type)
{
    // uint16_t sn = -1;
    uint16_t sn = 0;

    if (num > 9)
    {
        return sn;
    }

    switch (num_type)
    {
    case IMAGE_TYPE_DILUTE_SET:
        // sn = IMAGES_DILUTE_SERIAL_NUMBER(num);
        sn = lcd_get_image_dilute_sn(num);
        break;
    case IMAGE_TYPE_MEASURE_DATA:
        // sn = IMAGES_MEASURE_SERIAL_NUMBER(num);
        sn = lcd_get_image_measure_sn(num);
        break;
    case IMAGE_TYPE_TEMPERATURE_SET_BLACK:
        // sn = IMAGES_TEMPERATURE_BLACK_SERIAL_NUMBER(num);
        sn = lcd_get_image_temp_black_sn(num);
        break;
    case IMAGE_TYPE_TEMPERATURE_SET_BLUE:
        // sn = IMAGES_TEMPERATURE_BLUE_SERIAL_NUMBER(num);
        sn = lcd_get_image_temp_blue_sn(num);
        break;
    case IMAGE_TYPE_TEMPERATURE_SHOW:
        // sn = IMAGES_TEMPERATURE_SHOW_SERIAL_NUMBER(num);
        sn = lcd_get_image_temp_show_sn(num);
        break;
    case IMAGE_TYPE_TIME_SHOW:
        // sn = IMAGES_TIME_SHOW_SERIAL_NUMBER(num);
        sn = lcd_get_image_time_sn(num);
        break;
    case IMAGE_TYPE_TOTAL_SET:
        // sn = IMAGES_TOTAL_SET_SERIAL_NUMBER(num);
        sn = lcd_get_image_total_sn(num);
        break;

    default:
        break;
    }

    return sn;
}

void lcd_time_display(uint32_t timestamp)
{
    printf("fun:%s line:%d timestamp:%ld\n", __FUNCTION__, __LINE__,timestamp);
    if (lcd_check_conn_status() == COMM_DISCONN)
    {
        lcd_conn_opt(COMM_CONN);
        return;
    }

    figure_msg_t imgs[5];
    // uint16_t seconds = timestamp / 1000;
    // uint16_t minutes = seconds / 60;
    // uint16_t hours = minutes / 60;
    uint16_t seconds = (timestamp % 3600) % 60;
    uint16_t minutes = (timestamp % 3600) / 60;
    uint16_t hours = timestamp / 3600;
    uint16_t sn = 0;

    printf("fun:%s line:%d hours:%02x minutes:%02x seconds:%02x\n", __FUNCTION__, __LINE__,hours,minutes,seconds);

    _u16_2_byte2_big_endian(237, imgs[0].x_coordinate);
    _u16_2_byte2_big_endian(59, imgs[0].y_coordinate);

    _u16_2_byte2_big_endian(206, imgs[1].x_coordinate);
    _u16_2_byte2_big_endian(59, imgs[1].y_coordinate);

    _u16_2_byte2_big_endian(IMAGES_TIME_DELIMITER_SERIAL_NUMBER, imgs[2].figure_no);
    _u16_2_byte2_big_endian(194, imgs[2].x_coordinate);
    _u16_2_byte2_big_endian(71, imgs[2].y_coordinate);

    _u16_2_byte2_big_endian(164, imgs[3].x_coordinate);
    _u16_2_byte2_big_endian(59, imgs[3].y_coordinate);

    _u16_2_byte2_big_endian(135, imgs[4].x_coordinate);
    _u16_2_byte2_big_endian(59, imgs[4].y_coordinate);

    if (minutes < 10)
    {
        sn = lcd_get_image_serial_number(minutes, IMAGE_TYPE_TIME_SHOW);
        _u16_2_byte2_big_endian(sn, imgs[0].figure_no);

        sn = lcd_get_image_serial_number(0, IMAGE_TYPE_TIME_SHOW);
        _u16_2_byte2_big_endian(sn, imgs[1].figure_no);
    }
    else
    {
        uint16_t m0 = minutes % 10;
        uint16_t m1 = minutes / 10;

        sn = lcd_get_image_serial_number(m0, IMAGE_TYPE_TIME_SHOW);
        _u16_2_byte2_big_endian(sn, imgs[0].figure_no);

        sn = lcd_get_image_serial_number(m1, IMAGE_TYPE_TIME_SHOW);
        _u16_2_byte2_big_endian(sn, imgs[1].figure_no);
    }

    if (hours < 10)
    {
        sn = lcd_get_image_serial_number(hours, IMAGE_TYPE_TIME_SHOW);
        _u16_2_byte2_big_endian(sn, imgs[3].figure_no);

        sn = lcd_get_image_serial_number(0, IMAGE_TYPE_TIME_SHOW);
        _u16_2_byte2_big_endian(sn, imgs[4].figure_no);
    }
    else
    {
        uint16_t h0 = hours % 10;
        uint16_t h1 = hours / 10;

        sn = lcd_get_image_serial_number(h0, IMAGE_TYPE_TIME_SHOW);
        _u16_2_byte2_big_endian(sn, imgs[3].figure_no);

        sn = lcd_get_image_serial_number(h1, IMAGE_TYPE_TIME_SHOW);
        _u16_2_byte2_big_endian(sn, imgs[4].figure_no);
    }

    lcd_fill_bg_and_icon_cmd(imgs, 5);
}

void lcd_total_volume_display(uint16_t volume)
{
    printf("fun:%s line:%d volume:%d\n", __FUNCTION__, __LINE__,volume);
    if (lcd_check_conn_status() == COMM_DISCONN)
    {
        lcd_conn_opt(COMM_CONN);
        return;
    }

    figure_msg_t imgs[5];
    uint16_t v1 = volume % 10;
    uint16_t v10 = volume / 10 % 10;
    uint16_t v100 = volume / 100 % 10;
    uint16_t v1000 = volume / 1000 % 10;
    uint16_t sn = 0;

    _u16_2_byte2_big_endian(IMAGES_TOTAL_SET_ML_SERIAL_NUMBER, imgs[0].figure_no);
    _u16_2_byte2_big_endian(160, imgs[0].x_coordinate);
    _u16_2_byte2_big_endian(146, imgs[0].y_coordinate);

    sn = lcd_get_image_serial_number(v1, IMAGE_TYPE_TOTAL_SET);
    _u16_2_byte2_big_endian(sn, imgs[1].figure_no);
    _u16_2_byte2_big_endian(139, imgs[1].x_coordinate);
    _u16_2_byte2_big_endian(148, imgs[1].y_coordinate);

    sn = lcd_get_image_serial_number(v10, IMAGE_TYPE_TOTAL_SET);
    _u16_2_byte2_big_endian(sn, imgs[2].figure_no);
    _u16_2_byte2_big_endian(120, imgs[2].x_coordinate);
    _u16_2_byte2_big_endian(148, imgs[2].y_coordinate);

    sn = lcd_get_image_serial_number(v100, IMAGE_TYPE_TOTAL_SET);
    _u16_2_byte2_big_endian(sn, imgs[3].figure_no);
    _u16_2_byte2_big_endian(101, imgs[3].x_coordinate);
    _u16_2_byte2_big_endian(148, imgs[3].y_coordinate);

    sn = lcd_get_image_serial_number(v1000, IMAGE_TYPE_TOTAL_SET);
    _u16_2_byte2_big_endian(sn, imgs[4].figure_no);
    _u16_2_byte2_big_endian(81, imgs[4].x_coordinate);
    _u16_2_byte2_big_endian(148, imgs[4].y_coordinate);

    lcd_fill_bg_and_icon_cmd(imgs, 5);
}

void lcd_dilute_ratio_display(uint16_t ratio)
{
    printf("fun:%s line:%d ratio:%d\n", __FUNCTION__, __LINE__,ratio);
    if (lcd_check_conn_status() == COMM_DISCONN)
    {
        lcd_conn_opt(COMM_CONN);
        return;
    }

    figure_msg_t imgs[3];
    uint16_t r1 = ratio % 10;
    uint16_t r10 = ratio / 10 % 10;
    uint16_t sn = 0;

    _u16_2_byte2_big_endian(IMAGES_DILUTE_SET_PERCENT_SERIAL_NUMBER, imgs[0].figure_no);
    _u16_2_byte2_big_endian(317, imgs[0].x_coordinate);
    _u16_2_byte2_big_endian(148, imgs[0].y_coordinate);

    sn = lcd_get_image_serial_number(r1, IMAGE_TYPE_DILUTE_SET);
    _u16_2_byte2_big_endian(sn, imgs[1].figure_no);
    _u16_2_byte2_big_endian(296, imgs[1].x_coordinate);
    _u16_2_byte2_big_endian(148, imgs[1].y_coordinate);

    sn = lcd_get_image_serial_number(r10, IMAGE_TYPE_DILUTE_SET);
    _u16_2_byte2_big_endian(sn, imgs[2].figure_no);
    _u16_2_byte2_big_endian(275, imgs[2].x_coordinate);
    _u16_2_byte2_big_endian(148, imgs[2].y_coordinate);

    lcd_fill_bg_and_icon_cmd(imgs, 3);
}

void lcd_running_status_display(uint16_t status_sn)
{
    if (sg_lcd_stage != COMM_CONN_STAGE)
    {
        return;
    }

    // printf("fun:%s line:%d status_sn:%x\n", __FUNCTION__, __LINE__,status_sn);
    if (lcd_check_conn_status() == COMM_DISCONN)
    {
        lcd_conn_opt(COMM_CONN);
        return;
    }

    printf("fun:%s line:%d status_sn:%x\n", __FUNCTION__, __LINE__,status_sn);
    figure_msg_t imgs[2];
    uint16_t x = 0;
    uint16_t y = 203;

    switch (status_sn)
    {
    case IMAGES_STATUS_OUT_CUP_SERIAL_NUMBER:
    case IMAGES_STATUS_RECYCLE_SERIAL_NUMBER:
    case IMAGES_STATUS_RESET_SERIAL_NUMBER:
    case IMAGES_STATUS_STANDBY_SERIAL_NUMBER:
        x = 104;
        break;
    case IMAGES_STATUS_LOW_WATER_SERIAL_NUMBER:
    case IMAGES_STATUS_NO_CUP_SERIAL_NUMBER:
        x = 95;
        break;
    case IMAGES_STATUS_HEAT_FAIL_SERIAL_NUMBER:
    case IMAGES_STATUS_JIAZHU_FAIL_SERIAL_NUMBER:
    case IMAGES_STATUS_POSITION_ERR_SERIAL_NUMBER:
    case IMAGES_STATUS_THROW_CUP_ERR_SERIAL_NUMBER:
        x = 81;
        break;

    default:
        printf("fun:%s line:%d unknown status sn:%x\n",__FUNCTION__,__LINE__,status_sn);
        return;
        // break;
    }

    _u16_2_byte2_big_endian(status_sn, imgs[0].figure_no);
    _u16_2_byte2_big_endian(x, imgs[0].x_coordinate);
    _u16_2_byte2_big_endian(y, imgs[0].y_coordinate);

    lcd_fill_bg_and_icon_cmd(imgs, 1);
}

void lcd_temperature_display(uint16_t temp)
{
    printf("fun:%s line:%d temp:%d\n", __FUNCTION__, __LINE__,temp);
    if (lcd_check_conn_status() == COMM_DISCONN)
    {
        lcd_conn_opt(COMM_CONN);
        return;
    }

    figure_msg_t imgs[3];
    uint8_t figure_num = 3;

    _u16_2_byte2_big_endian(IMAGES_TEMPERATURE_SHOW_CELSIUS_SERIAL_NUMBER, imgs[0].figure_no);
    _u16_2_byte2_big_endian(324, imgs[0].x_coordinate);
    _u16_2_byte2_big_endian(203, imgs[0].y_coordinate);

    if (TEMPERATURE_NORMAL_VALUE == temp)
    {
        figure_num = 2;
        _u16_2_byte2_big_endian(IMAGES_TEMPERATURE_SHOW_NORMAL_TEMP_SERIAL_NUMBER, imgs[1].figure_no);
        _u16_2_byte2_big_endian(275, imgs[1].x_coordinate);
        _u16_2_byte2_big_endian(203, imgs[1].y_coordinate);
    }
    else
    {
        uint16_t sn = 0;
        uint16_t t1 = temp % 10;
        uint16_t t10 = temp / 10 % 10;
        //uint16_t t100 = temp / 100 % 10;

        sn = lcd_get_image_serial_number(t1, IMAGE_TYPE_TEMPERATURE_SHOW);
        _u16_2_byte2_big_endian(sn, imgs[1].figure_no);
        _u16_2_byte2_big_endian(305, imgs[1].x_coordinate);
        _u16_2_byte2_big_endian(204, imgs[1].y_coordinate);

        sn = lcd_get_image_serial_number(t10, IMAGE_TYPE_TEMPERATURE_SHOW);
        _u16_2_byte2_big_endian(sn, imgs[2].figure_no);
        _u16_2_byte2_big_endian(286, imgs[2].x_coordinate);
        _u16_2_byte2_big_endian(204, imgs[2].y_coordinate);
/*
        sn = lcd_get_image_serial_number(t100, IMAGE_TYPE_TEMPERATURE_SHOW);
        _u16_2_byte2_big_endian(sn, imgs[3].figure_no);
        _u16_2_byte2_big_endian(266, imgs[3].x_coordinate);
        _u16_2_byte2_big_endian(204, imgs[3].y_coordinate);*/
    }

    lcd_fill_bg_and_icon_cmd(imgs, figure_num);
}

void lcd_cup_num_display(uint16_t cups)
{
    printf("fun:%s line:%d cups:%d\n", __FUNCTION__, __LINE__,cups);
    if (lcd_check_conn_status() == COMM_DISCONN)
    {
        lcd_conn_opt(COMM_CONN);
        return;
    }

    figure_msg_t imgs[5];
    uint16_t sn = 0;
    uint16_t c1 = cups % 10;
    uint16_t c10 = cups / 10 % 10;
    uint16_t c100 = cups / 100 % 10;
    uint16_t c1000 = cups / 1000 % 10;

    // _u16_2_byte2_big_endian(IMAGES_MEASURE_DATA_CUP_SERIAL_NUMBER, imgs[0].figure_no);
    // _u16_2_byte2_big_endian(240, imgs[0].x_coordinate);
    // _u16_2_byte2_big_endian(282, imgs[0].y_coordinate);

    sn = lcd_get_image_serial_number(c1, IMAGE_TYPE_MEASURE_DATA);
    _u16_2_byte2_big_endian(sn, imgs[1].figure_no);
    _u16_2_byte2_big_endian(209, imgs[1].x_coordinate);
    _u16_2_byte2_big_endian(271, imgs[1].y_coordinate);

    sn = lcd_get_image_serial_number(c10, IMAGE_TYPE_MEASURE_DATA);
    _u16_2_byte2_big_endian(sn, imgs[2].figure_no);
    _u16_2_byte2_big_endian(189, imgs[2].x_coordinate);
    _u16_2_byte2_big_endian(271, imgs[2].y_coordinate);

    sn = lcd_get_image_serial_number(c100, IMAGE_TYPE_MEASURE_DATA);
    _u16_2_byte2_big_endian(sn, imgs[3].figure_no);
    _u16_2_byte2_big_endian(169, imgs[3].x_coordinate);
    _u16_2_byte2_big_endian(271, imgs[3].y_coordinate);

    sn = lcd_get_image_serial_number(c1000, IMAGE_TYPE_MEASURE_DATA);
    _u16_2_byte2_big_endian(sn, imgs[4].figure_no);
    _u16_2_byte2_big_endian(149, imgs[4].x_coordinate);
    _u16_2_byte2_big_endian(271, imgs[4].y_coordinate);

    lcd_fill_bg_and_icon_cmd(&imgs[1], 5);
}

void lcd_setting_display(uint8_t set_opt)
{
    uint16_t sn = IMAGES_SETTING_TIME_SET_SERIAL_NUMBER;
    printf("fun:%s line:%d set option:%x\n", __FUNCTION__, __LINE__,set_opt);
    if (lcd_check_conn_status() == COMM_DISCONN)
    {
        lcd_conn_opt(COMM_CONN);
        return;
    }

    figure_msg_t imgs[2];
    uint16_t x = 0;
    uint16_t y = 0;

    switch (set_opt)
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
    case SETTING_OPTIONS_RESET:
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

    default:
        printf("fun:%s line:%d unknown setting option:%d\n",__FUNCTION__,__LINE__,set_opt);
        return;
        // break;
    }

    _u16_2_byte2_big_endian(sn, imgs[0].figure_no);
    _u16_2_byte2_big_endian(x, imgs[0].x_coordinate);
    _u16_2_byte2_big_endian(y, imgs[0].y_coordinate);

    lcd_fill_bg_and_icon_cmd(imgs, 1);
}

void lcd_main_bg_display()
{
    if (lcd_check_conn_status() == COMM_DISCONN)
    {
        lcd_conn_opt(COMM_CONN);
        return;
    }

    figure_msg_t imgs[2];

    // 显示黑屏大背景
    _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_BACKGROUND_BLACK_SERIAL_NUMBER, imgs[0].figure_no);
    _u16_2_byte2_big_endian(0, imgs[0].x_coordinate);
    _u16_2_byte2_big_endian(0, imgs[0].y_coordinate);

    lcd_fill_bg_and_icon_cmd(imgs, 1);
}

void lcd_main_circle_bg_display()
{
    if (lcd_check_conn_status() == COMM_DISCONN)
    {
        lcd_conn_opt(COMM_CONN);
        return;
    }

    figure_msg_t imgs[2];

    // 显示背景大圆圈 可选自己喜欢的颜色
    _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_CIRCLE_GREEN_SERIAL_NUMBER, imgs[0].figure_no);
    _u16_2_byte2_big_endian(8, imgs[0].x_coordinate);
    _u16_2_byte2_big_endian(8, imgs[0].y_coordinate);


    lcd_fill_bg_and_icon_cmd(imgs, 1);
}

void lcd_main_menu_bg_display()
{
    if (lcd_check_conn_status() == COMM_DISCONN)
    {
        lcd_conn_opt(COMM_CONN);
        return;
    }

    figure_msg_t imgs[2];

    // 显示底部菜单背景图标
    _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_BOTTOM_MENU_SERIAL_NUMBER, imgs[0].figure_no);
    _u16_2_byte2_big_endian(25, imgs[0].x_coordinate);
    _u16_2_byte2_big_endian(197, imgs[0].y_coordinate);

    lcd_fill_bg_and_icon_cmd(imgs, 1);
}

void lcd_main_middle_bg_display()
{
    if (lcd_check_conn_status() == COMM_DISCONN)
    {
        lcd_conn_opt(COMM_CONN);
        return;
    }

    figure_msg_t imgs[8];

    // 显示中间总容量信息图标
    _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_TOTAL_SET_BG_SERIAL_NUMBER, imgs[0].figure_no);
    _u16_2_byte2_big_endian(38, imgs[0].x_coordinate);
    _u16_2_byte2_big_endian(141, imgs[0].y_coordinate);
    _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_TOTAL_CAPACITY_SERIAL_NUMBER, imgs[1].figure_no);
    _u16_2_byte2_big_endian(48, imgs[1].x_coordinate);
    _u16_2_byte2_big_endian(147, imgs[1].y_coordinate);

    // 显示中间稀释比例信息图标
    _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_DILUTE_SET_BG_SERIAL_NUMBER, imgs[2].figure_no);
    _u16_2_byte2_big_endian(209, imgs[2].x_coordinate);
    _u16_2_byte2_big_endian(141, imgs[2].y_coordinate);
    _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_DILUTE_RATIO_SERIAL_NUMBER, imgs[3].figure_no);
    _u16_2_byte2_big_endian(216, imgs[3].x_coordinate);
    _u16_2_byte2_big_endian(147, imgs[3].y_coordinate);

    // 显示中间运行状态信息图标
    _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_STATUS_SET_BG_SERIAL_NUMBER, imgs[4].figure_no);
    _u16_2_byte2_big_endian(38, imgs[4].x_coordinate);
    _u16_2_byte2_big_endian(197, imgs[4].y_coordinate);
    _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_STATUS_SERIAL_NUMBER, imgs[5].figure_no);
    _u16_2_byte2_big_endian(46, imgs[5].x_coordinate);
    _u16_2_byte2_big_endian(202, imgs[5].y_coordinate);


    // 显示中间温度信息图标
    _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_TEMPERATURE_SET_BG_SERIAL_NUMBER, imgs[6].figure_no);
    _u16_2_byte2_big_endian(210, imgs[6].x_coordinate);
    _u16_2_byte2_big_endian(197, imgs[6].y_coordinate);
    _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_TEMPERATURE_SERIAL_NUMBER, imgs[7].figure_no);
    _u16_2_byte2_big_endian(220, imgs[7].x_coordinate);
    _u16_2_byte2_big_endian(201, imgs[7].y_coordinate);

    lcd_fill_bg_and_icon_cmd(imgs, 8);
}

void lcd_main_cup_bg_display()
{
    if (lcd_check_conn_status() == COMM_DISCONN)
    {
        lcd_conn_opt(COMM_CONN);
        return;
    }

    figure_msg_t imgs[2];

    // 显示中间杯图标
    _u16_2_byte2_big_endian(IMAGES_MEASURE_DATA_CUP_SERIAL_NUMBER, imgs[0].figure_no);
    _u16_2_byte2_big_endian(240, imgs[0].x_coordinate);
    _u16_2_byte2_big_endian(282, imgs[0].y_coordinate);
    // _u16_2_byte2_big_endian(IMAGES_MEASURE_DATA_0_SERIAL_NUMBER, imgs[1].figure_no);
    // _u16_2_byte2_big_endian(209, imgs[1].x_coordinate);
    // _u16_2_byte2_big_endian(271, imgs[1].y_coordinate);

    lcd_fill_bg_and_icon_cmd(imgs, 1);
}

void lcd_init_display(void)
{
    // 建立连接
    if (lcd_check_conn_status() == COMM_DISCONN)
    {
        lcd_conn_opt(COMM_CONN);
        return;
    }

#if 0
    // 显示主屏背景
    lcd_main_bg_display();
    // 显示背景大圆圈 可选自己喜欢的颜色
    lcd_main_circle_bg_display();
    // 显示底部菜单背景图标
    lcd_main_menu_bg_display();
    // 显示中间信息背景图标
    lcd_main_middle_bg_display();
    // 显示中间杯背景图标
    lcd_main_cup_bg_display();

    // 显示时间
    lcd_time_display(rtc_counter_get());
    // 显示总容量
    lcd_total_volume_display(water_set);
    // 显示稀释比例
    lcd_dilute_ratio_display(enzyme_rate);
    // 显示运行状态
    lcd_running_status_display(IMAGES_STATUS_STANDBY_SERIAL_NUMBER);
    // 显示温度
    lcd_temperature_display(temperature_set);
    // 显示出杯数量
    lcd_cup_num_display(cup_count);
    // 显示默认设置
    lcd_setting_display(SETTING_OPTIONS_TIME);
#endif // 0
}

#if 0
void lcd_display_update(void)
{
    // 建立连接
    if (lcd_check_conn_status() == COMM_DISCONN)
    {
        lcd_conn_opt(COMM_CONN);
        return;
    }

    switch (sg_lcd_stage)
    {
    case COMM_FILL_MAIN_BG_STAGE:
        // 显示主屏背景
        lcd_main_bg_display();
        break;
    case COMM_FILL_MAIN_CIRCLE_BG_STAGE:
        // 显示背景大圆圈 可选自己喜欢的颜色
        lcd_main_circle_bg_display();
        break;
    case COMM_FILL_MAIN_MENU_BG_STAGE:
        // 显示底部菜单背景图标
        lcd_main_menu_bg_display();
        break;
    case COMM_FILL_MAIN_MIDDLE_BG_STAGE:
        // 显示中间信息背景图标
        lcd_main_middle_bg_display();
        break;
    case COMM_FILL_MAIN_CUP_BG_STAGE:
        // 显示中间杯背景图标
        lcd_main_cup_bg_display();
        break;
    case COMM_CONN_STAGE:
        // 显示时间
        lcd_time_display(rtc_counter_get());
        // 显示总容量
        lcd_total_volume_display(water_set);
        // 显示稀释比例
        lcd_dilute_ratio_display(enzyme_rate);
        // 显示运行状态
        lcd_running_status_display(IMAGES_STATUS_STANDBY_SERIAL_NUMBER);
        // 显示温度
        lcd_temperature_display(temperature_set);
        // 显示出杯数量
        lcd_cup_num_display(cup_count);
        // 显示默认设置
        lcd_setting_display(SETTING_OPTIONS_TIME);
        break;

    default:
        break;
    }
}
#endif // 0

void lcd_display_update(void)
{
    uint16_t sn = 0;
    // 建立连接
    if (lcd_check_conn_status() == COMM_DISCONN)
    {
        lcd_conn_opt(COMM_CONN);
        return;
    }

    figure_msg_t imgs[24];

    // 显示黑屏大背景
    _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_BACKGROUND_BLACK_SERIAL_NUMBER, imgs[0].figure_no);
    _u16_2_byte2_big_endian(0, imgs[0].x_coordinate);
    _u16_2_byte2_big_endian(0, imgs[0].y_coordinate);

    // 显示背景大圆圈 可选自己喜欢的颜色
    _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_CIRCLE_GREEN_SERIAL_NUMBER, imgs[1].figure_no);
    _u16_2_byte2_big_endian(8, imgs[1].x_coordinate);
    _u16_2_byte2_big_endian(8, imgs[1].y_coordinate);

    // 显示底部菜单背景图标
    _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_BOTTOM_MENU_SERIAL_NUMBER, imgs[2].figure_no);
    _u16_2_byte2_big_endian(25, imgs[2].x_coordinate);
    _u16_2_byte2_big_endian(197, imgs[2].y_coordinate);

    // 显示中间总容量信息图标
    _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_TOTAL_SET_BG_SERIAL_NUMBER, imgs[3].figure_no);
    _u16_2_byte2_big_endian(38, imgs[3].x_coordinate);
    _u16_2_byte2_big_endian(141, imgs[3].y_coordinate);
    _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_TOTAL_CAPACITY_SERIAL_NUMBER, imgs[4].figure_no);
    _u16_2_byte2_big_endian(48, imgs[4].x_coordinate);
    _u16_2_byte2_big_endian(147, imgs[4].y_coordinate);

    // 显示中间稀释比例信息图标
    _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_DILUTE_SET_BG_SERIAL_NUMBER, imgs[5].figure_no);
    _u16_2_byte2_big_endian(209, imgs[5].x_coordinate);
    _u16_2_byte2_big_endian(141, imgs[5].y_coordinate);
    _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_DILUTE_RATIO_SERIAL_NUMBER, imgs[6].figure_no);
    _u16_2_byte2_big_endian(216, imgs[6].x_coordinate);
    _u16_2_byte2_big_endian(147, imgs[6].y_coordinate);

    // 显示中间运行状态信息图标
    _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_STATUS_SET_BG_SERIAL_NUMBER, imgs[7].figure_no);
    _u16_2_byte2_big_endian(38, imgs[7].x_coordinate);
    _u16_2_byte2_big_endian(197, imgs[7].y_coordinate);
    _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_STATUS_SERIAL_NUMBER, imgs[8].figure_no);
    _u16_2_byte2_big_endian(46, imgs[8].x_coordinate);
    _u16_2_byte2_big_endian(202, imgs[8].y_coordinate);

    // 显示中间温度信息图标
    _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_TEMPERATURE_SET_BG_SERIAL_NUMBER, imgs[9].figure_no);
    _u16_2_byte2_big_endian(210, imgs[9].x_coordinate);
    _u16_2_byte2_big_endian(197, imgs[9].y_coordinate);
    _u16_2_byte2_big_endian(IMAGES_MAIN_PICTURE_TEMPERATURE_SERIAL_NUMBER, imgs[10].figure_no);
    _u16_2_byte2_big_endian(220, imgs[10].x_coordinate);
    _u16_2_byte2_big_endian(201, imgs[10].y_coordinate);

    // 显示中间杯图标
    _u16_2_byte2_big_endian(IMAGES_MEASURE_DATA_CUP_SERIAL_NUMBER, imgs[11].figure_no);
    _u16_2_byte2_big_endian(240, imgs[11].x_coordinate);
    _u16_2_byte2_big_endian(282, imgs[11].y_coordinate);

    // 显示总容量
    uint16_t v1 = water_set % 10;
    uint16_t v10 = water_set / 10 % 10;
    uint16_t v100 = water_set / 100 % 10;
    //uint16_t v1000 = water_set / 1000 % 10;
    _u16_2_byte2_big_endian(IMAGES_TOTAL_SET_ML_SERIAL_NUMBER, imgs[12].figure_no);
    _u16_2_byte2_big_endian(160, imgs[12].x_coordinate);
    _u16_2_byte2_big_endian(146, imgs[12].y_coordinate);

    sn = lcd_get_image_serial_number(v1, IMAGE_TYPE_TOTAL_SET);
    _u16_2_byte2_big_endian(sn, imgs[13].figure_no);
    _u16_2_byte2_big_endian(139, imgs[13].x_coordinate);
    _u16_2_byte2_big_endian(148, imgs[13].y_coordinate);

    sn = lcd_get_image_serial_number(v10, IMAGE_TYPE_TOTAL_SET);
    _u16_2_byte2_big_endian(sn, imgs[14].figure_no);
    _u16_2_byte2_big_endian(120, imgs[14].x_coordinate);
    _u16_2_byte2_big_endian(148, imgs[14].y_coordinate);

    sn = lcd_get_image_serial_number(v100, IMAGE_TYPE_TOTAL_SET);
    _u16_2_byte2_big_endian(sn, imgs[15].figure_no);
    _u16_2_byte2_big_endian(101, imgs[15].x_coordinate);
    _u16_2_byte2_big_endian(148, imgs[15].y_coordinate);
/*
    sn = lcd_get_image_serial_number(v1000, IMAGE_TYPE_TOTAL_SET);
    _u16_2_byte2_big_endian(sn, imgs[16].figure_no);
    _u16_2_byte2_big_endian(81, imgs[16].x_coordinate);
    _u16_2_byte2_big_endian(148, imgs[16].y_coordinate);
*/
    // 显示稀释比例
    uint16_t r1 = enzyme_rate % 10;
    uint16_t r10 = enzyme_rate / 10 % 10;
    _u16_2_byte2_big_endian(IMAGES_DILUTE_SET_PERCENT_SERIAL_NUMBER, imgs[16].figure_no);
    _u16_2_byte2_big_endian(317, imgs[16].x_coordinate);
    _u16_2_byte2_big_endian(148, imgs[16].y_coordinate);

    sn = lcd_get_image_serial_number(r1, IMAGE_TYPE_DILUTE_SET);
    _u16_2_byte2_big_endian(sn, imgs[17].figure_no);
    _u16_2_byte2_big_endian(296, imgs[17].x_coordinate);
    _u16_2_byte2_big_endian(148, imgs[17].y_coordinate);

    sn = lcd_get_image_serial_number(r10, IMAGE_TYPE_DILUTE_SET);
    _u16_2_byte2_big_endian(sn, imgs[18].figure_no);
    _u16_2_byte2_big_endian(275, imgs[18].x_coordinate);
    _u16_2_byte2_big_endian(148, imgs[18].y_coordinate);

    // 显示出杯数量
    uint16_t c1 = cup_count % 10;
    uint16_t c10 = cup_count / 10 % 10;
    uint16_t c100 = cup_count / 100 % 10;
    uint16_t c1000 = cup_count / 1000 % 10;
    sn = lcd_get_image_serial_number(c1, IMAGE_TYPE_MEASURE_DATA);
    _u16_2_byte2_big_endian(sn, imgs[19].figure_no);
    _u16_2_byte2_big_endian(209, imgs[19].x_coordinate);
    _u16_2_byte2_big_endian(271, imgs[19].y_coordinate);

    sn = lcd_get_image_serial_number(c10, IMAGE_TYPE_MEASURE_DATA);
    _u16_2_byte2_big_endian(sn, imgs[20].figure_no);
    _u16_2_byte2_big_endian(189, imgs[20].x_coordinate);
    _u16_2_byte2_big_endian(271, imgs[20].y_coordinate);

    sn = lcd_get_image_serial_number(c100, IMAGE_TYPE_MEASURE_DATA);
    _u16_2_byte2_big_endian(sn, imgs[21].figure_no);
    _u16_2_byte2_big_endian(169, imgs[21].x_coordinate);
    _u16_2_byte2_big_endian(271, imgs[21].y_coordinate);

    sn = lcd_get_image_serial_number(c1000, IMAGE_TYPE_MEASURE_DATA);
    _u16_2_byte2_big_endian(sn, imgs[22].figure_no);
    _u16_2_byte2_big_endian(149, imgs[22].x_coordinate);
    _u16_2_byte2_big_endian(271, imgs[22].y_coordinate);

    lcd_fill_bg_and_icon_cmd(imgs, 23);
    // // 显示温度
    // lcd_temperature_display(temperature_set);
    // // 显示默认设置
    // lcd_setting_display(IMAGES_SETTING_TIME_SET_SERIAL_NUMBER);
}
