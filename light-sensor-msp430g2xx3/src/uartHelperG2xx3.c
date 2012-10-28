/*
 * UART Helper
 */

#include "Board.h"
#include "uartHelperG2xx3.h"


void uartSendByte ( uint8_t ch)
{
    while( UCA0STAT & UCBUSY ) ;
    UCA0TXBUF = ch;
    _no_operation();
}


void uartSendBuffer ( uint8_t *ptr, int number)
{
    int idx = 0;
    while( idx < number ) {
        uartSendByte( ptr[idx++] );
    }
}


void uartInit()
{
    P1SEL |= BIT1 + BIT2;    // P1.1=RXD, P1.2=TXD
    P1SEL2 |= BIT1 + BIT2;   // P1.1=RXD, P1.2=TXD
    UCA0CTL1 |= UCSSEL_2;    // use SMCLK for USCI clock
    //
    UCA0BR0 = 41;            // 9600bps @ 8 MHz
    UCA0BR1 = 3;             // 9600bps @ 8 MHz
    UCA0MCTL = UCBRS1;       // 9600bps @ 8 MHz
    //
    UCA0CTL1 &= ~UCSWRST;    // Release from reset
    IFG2 &= ~( UCA0TXIFG + UCA0RXIFG );
    IE2 |= UCA0RXIE;         // Enable interrupts on RX
}


//#pragma vector = USCIAB0RX_VECTOR
//__interrupt void USCIAB0RX_ISR(void)
//{
//    if( IFG2 & UCA0RXIFG ) {
//        rxBuffer[rxBufPutIdx++] = UCA0RXBUF;
//        rxBufPutIdx &= RX_BUF_MASK;
//        if( rxBufPutIdx == rxBufGetIdx ) {
//            uartStatus |= UART_RX_OVERFLOW;
//        }
//    }
//}
