#ifndef UART_G2xx3_H_
#define UART_G2xx3_H_

#include <inttypes.h>

void uartInit( void );
void uartSendBuffer ( uint8_t *ptr, int number );

#endif /*UART_G2xx3_H_*/
