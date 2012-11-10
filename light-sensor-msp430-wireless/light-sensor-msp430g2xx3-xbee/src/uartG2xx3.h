#ifndef UART_G2xx3_H_
#define UART_G2xx3_H_

#include <inttypes.h>

void uartInit( void );
void uartSendChar ( uint8_t ch);
void uartSendCharBuffer ( uint8_t *cp, uint8_t number);
void uartSendString ( uint8_t *str);

void uartReceivedCharacterCallback(uint8_t ch);

#endif /*UART_G2xx3_H_*/
