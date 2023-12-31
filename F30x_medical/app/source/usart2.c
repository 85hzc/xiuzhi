/*!
    \file    usart.c
    \brief   FOC demo
*/
#include "global.h"

/* public variables */
uint16_t sg_write_index = 0, sg_read_index = 0;
uint8_t rx_buffer[COM_BUFFER_SIZE] = {0};

/*!
    \brief      data transmission with USART
    \param[in]  usart_data: the data to be sent
    \param[out] none
    \retval     none
*/
void usart2_data_transfer(uint8_t *usart_data, uint8_t len)
{
    uint8_t index = 0;

    for (index = 0; index < len; index++)
    {
        usart_data_transmit(USART2, (uint8_t)usart_data[index]);
        while (RESET == usart_flag_get(USART2, USART_FLAG_TBE))
            ;
    }
}

/*!
    \brief      configure USART
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usart2_init(void)
{
    /* clock enable */
    rcu_periph_clock_enable(RCU_GPIOC);

    /* USART2_RX: PC11 */
    gpio_init(COM_USART_RX_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, COM_USART_RX_PIN);
    /* USART2_TX: PC10 */
    gpio_init(COM_USART_TX_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, COM_USART_TX_PIN);
    // gpio_pin_remap_config(GPIO_USART2_FULL_REMAP,ENABLE);
    gpio_pin_remap_config(GPIO_USART2_PARTIAL_REMAP, ENABLE);

    /* clock enable */
    rcu_periph_clock_enable(RCU_USART2);

    /* USART2 configure */
    usart_deinit(USART2);
    usart_baudrate_set(USART2, 115200);
    usart_stop_bit_set(USART2, USART_STB_1BIT);
    usart_word_length_set(USART2, USART_WL_8BIT);
    usart_parity_config(USART2, USART_PM_NONE);
    usart_hardware_flow_rts_config(USART2, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART2, USART_CTS_DISABLE);
    usart_receive_config(USART2, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART2, USART_TRANSMIT_ENABLE);
    usart_flag_clear(USART2, USART_FLAG_TC);

    usart_enable(USART2);

    /* enable USART2 receive interrupt */
    usart_interrupt_enable(USART2, USART_INT_RBNE);
    /* enable USART2 transmit interrupt */
    // usart_interrupt_enable(USART2, USART_INT_TBE);
}

uint8_t len_ok()
{
    if (sg_write_index > sg_read_index) {
        if (sg_write_index - sg_read_index >= 5) {
            return 1;
        }
    } else if (sg_write_index < sg_read_index) {
        if (sg_write_index + COM_BUFFER_SIZE - sg_read_index >= 5) {
            return 1;
        }
    }

    return 0;
}

uint8_t buffer_writes(uint8_t *chs, uint8_t len)
{

    for (uint8_t i=0; i<len; i++) {
        buffer_write(*(chs+i));
    }
    return 1;
}

uint8_t buffer_write(uint8_t ch)
{
    if (sg_read_index == (sg_write_index + 1) % COM_BUFFER_SIZE)
    {
        /* buffer is full */
        printf("[error]:buffer is full!\r\n");
        ringbuff_init();
        return 0;
    }

    rx_buffer[sg_write_index] = ch;
    sg_write_index = (sg_write_index + 1) % COM_BUFFER_SIZE;
    return 1;
}

uint8_t buffer_read(uint8_t *ch)
{
    if (sg_read_index == sg_write_index)
    {
        /* buffer is empty */
        return 0;
    }

    *ch = rx_buffer[sg_read_index];
    sg_read_index = (sg_read_index + 1) % COM_BUFFER_SIZE;
    return 1;
}

uint8_t buffer_reads(uint8_t *data, uint8_t len)
{
    uint8_t cnt = 0;
    uint8_t *pdata = data;

    while ((cnt < len) && buffer_read(pdata))
    {
        cnt++;
        pdata++;
    }

    // printf("fun:%s line:%d cnt:%d\r\n",__FUNCTION__,__LINE__,cnt);
    return cnt;
}

void ringbuff_init()
{
    sg_read_index = 0;
    sg_write_index = 0;
    memset(rx_buffer, 0, COM_BUFFER_SIZE);
}
