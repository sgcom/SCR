#ifndef BOARDSUPPORT_H_
#define BOARDSUPPORT_H_

#include <msp430g2553.h> 
#include <intrinsics.h>

// MSP430 LaunchPad specific
//
#define RED_LED         BIT0
#define GREEN_LED       BIT6
#define BUTTON_S2       BIT3

#define BUTTON_S2_ON()      !(P1IN & BUTTON_S2)

#define RED_LED_ON()        P1OUT |= RED_LED
#define RED_LED_OFF()       P1OUT &= ~RED_LED
#define RED_LED_TOGGLE()    P1OUT ^= RED_LED

#define GREEN_LED_ON()      P1OUT |= GREEN_LED
#define GREEN_LED_OFF()     P1OUT &= ~GREEN_LED
#define GREEN_LED_TOGGLE()  P1OUT ^= GREEN_LED


#endif /*BOARDSUPPORT_H_*/
