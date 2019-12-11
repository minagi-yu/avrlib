#include "uart.h"
#include <avr/interrupt.h>

#define UART_BUFFER_SIZE 64

#define UART_CLK_PER 3333333
#define UART_BAUD 9600

#define UART_TX_PORT PORTA
#define UART_TX_PIN PIN6_bp
#define UART_RX_PORT PORTA
#define UART_RX_PIN PIN7_bp

static volatile struct uart_buffer {
    uint16_t ridx;
    uint16_t widx;
    uint16_t cnt;
    uint8_t buff[UART_BUFFER_SIZE];
} rx_buff, tx_buff;

void uart_init()
{
    rx_buff = tx_buff = (struct uart_buffer){ 0 };

    cli();

    UART_TX_PORT.DIRSET = (1 << UART_TX_PIN);
    UART_TX_PORT.OUTCLR = (1 << UART_TX_PIN);

    UART_RX_PORT.DIRCLR = (1 << UART_RX_PIN);
    *((uint8_t *)&UART_RX_PORT.PIN0CTRL + UART_RX_PIN) &= ~PORT_PULLUPEN_bm;

    USART0.BAUD = ((float)(UART_CLK_PER * 64.0f / (16.0f * (float)UART_BAUD)) + 0.5f);

    sei();
}