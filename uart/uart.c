#include "uart.h"
#include <avr/interrupt.h>

#define UART_BUFFER_SIZE 64

#define UART_CLK_PER 3333333
#define UART_BAUD 9600

#define UART_TX_PORT PORTA
#define UART_TX_PIN 6
#define UART_RX_PORT PORTA
#define UART_RX_PIN 7

static volatile struct uart_buffer {
    uint8_t ridx;
    uint8_t widx;
    uint8_t cnt;
    uint8_t buff[UART_BUFFER_SIZE];
} rx_buff, tx_buff;

void uart_init(void)
{
    rx_buff = tx_buff = (struct uart_buffer){ 0 };

    cli();

    UART_TX_PORT.DIRSET = (1 << UART_TX_PIN);
    UART_TX_PORT.OUTCLR = (1 << UART_TX_PIN);

    UART_RX_PORT.DIRCLR = (1 << UART_RX_PIN);
    *((uint8_t *)&UART_RX_PORT.PIN0CTRL + UART_RX_PIN) &= ~PORT_PULLUPEN_bm;

    USART0.BAUD = ((float)(UART_CLK_PER * 64.0f / (16.0f * (float)UART_BAUD)) + 0.5f);

    USART0.CTRLA = USART_RXCIE_bm;
    USART0.CTRLB = (USART_RXEN_bm | USART_TXEN_bm);
    USART0.CTRLC = USART_CHSIZE_8BIT_gc;

    sei();
}

void uart_putc(uint8_t c)
{
    while (tx_buff.cnt == UART_BUFFER_SIZE)
        ;

    uint8_t widx = tx_buff.widx;
    tx_buff.buff[widx] = c;
    tx_buff.widx = (widx + 1) & (UART_BUFFER_SIZE - 1);
    cli();
    tx_buff.cnt++;
    sei();

    USART0.CTRLA |= USART_DREIE_bm;
}

uint8_t uart_getc(void)
{
    while (rx_buff.cnt == 0)
        ;

    uint8_t ridx = rx_buff.ridx;
    uint8_t c = rx_buff.buff[ridx];
    rx_buff.ridx = (ridx + 1) & (UART_BUFFER_SIZE - 1);
    cli();
    rx_buff.cnt--;
    sei();

    return c;
}

ISR(USART0_RXC_vect)
{
    uint8_t d;

    d = USART0.RXDATAL;

    if (rx_buff.cnt == UART_BUFFER_SIZE)
        return;

    uint8_t widx = rx_buff.widx;
    rx_buff.buff[widx] = d;
    rx_buff.widx = (widx + 1) & (UART_BUFFER_SIZE - 1);
    rx_buff.cnt++;
}

ISR(USART0_DRE_vect)
{
    if (tx_buff.cnt != 0) {
        uint8_t ridx = tx_buff.ridx;
        USART0.TXDATAL = tx_buff.buff[ridx];
        tx_buff.ridx = (ridx + 1) & (UART_BUFFER_SIZE - 1);
        tx_buff.cnt--;
    } else {
        USART0.CTRLA &= ~USART_DREIE_bm;
    }
}
