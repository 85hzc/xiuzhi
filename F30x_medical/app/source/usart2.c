/*!
    \file    usart.c
    \brief   FOC demo
*/


#include "global.h"

/* public variables */
uint8_t rx_buffer[COM_BUFFER_SIZE];
uint8_t rx_buffer_app[COM_BUFFER_SIZE];
uint16_t tx_counter = 0, rx_counter = 0, rx_counter_app = 0, rx_ok = 0;

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
        usart_data_transmit(USART2, (uint8_t) usart_data[index]);
        while(RESET == usart_flag_get(USART2, USART_FLAG_TBE));
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
    //gpio_pin_remap_config(GPIO_USART2_FULL_REMAP,ENABLE);
    gpio_pin_remap_config(GPIO_USART2_PARTIAL_REMAP,ENABLE);

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
    //usart_interrupt_enable(USART2, USART_INT_TBE);
}
