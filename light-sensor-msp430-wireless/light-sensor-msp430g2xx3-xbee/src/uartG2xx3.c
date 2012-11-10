/*
 * USCI_A0 UART, 9600, 8-N-1 format
 * Tested on MSP430G2553.
 */

#include "Board.h"
#include "uartG2xx3.h"
#include "xbeeInterface.h"


void uartSendChar ( uint8_t ch)
{
    while( UCA0STAT & UCBUSY ) ;
    UCA0TXBUF = ch;
    _no_operation();
}


void uartSendCharBuffer ( uint8_t *cp, uint8_t number)
{
    uint8_t idx = 0;
    while( idx < number ) {
        uartSendChar( cp[idx++] );
    }
}


void uartSendString ( uint8_t *str)
{
    while( *str ) {
        uartSendChar( *str++ );
    }
}


void uartInit()
{
    P1SEL |= BIT1 + BIT2;     // P1.1=RXD, P1.2=TXD
    P1SEL2 |= BIT1 + BIT2;    // P1.1=RXD, P1.2=TXD
    UCA0CTL1 |= UCSSEL_2;     // use SMCLK for USCI clock
    //
    // Baud rate Setting - slau144h.pdf, page 429 -
    // Table 15-4. Commonly Used Baud Rates, Settings...
    //
//  UCA0BR0 = 104;         // 1 MHz 9600
//  UCA0MCTL = UCBRS0;     // 1 MHz 9600
//  UCA0BR1 = 0;           // 1 MHz 9600
    UCA0BR0 = 41;          // 8 MHz 9600
    UCA0BR1 = 3;           // 8 MHz 9600
    UCA0MCTL = UCBRS1;     // 8 MHz 9600
    //
    UCA0CTL1 &= ~UCSWRST;     // Release from reset
    IFG2 &= ~( UCA0TXIFG + UCA0RXIFG );
    IE2 |= UCA0RXIE;         // Enable interrupts on RX
}


#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCIAB0RX_ISR(void)
{
    if( IFG2 & UCA0RXIFG ) {
        uint8_t ch = UCA0RXBUF;
        uartReceivedCharacterCallback(ch);
    }
}
