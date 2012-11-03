/*
 *
 * Light Sensor
 * Target MCU Device: MSP430G2553
 * 
*/

#include <string.h>

#include "Defines.h"
#include "Board.h"
#include "uartHelperG2xx3.h"


void SystemInit( void );
void longDelay( void );
void shortDelay( void );
void run( void );
void measureAndSendR( void );
void sendData( void );

#define EVT_TEST         0x01 // not used
#define EVT_MEASURE_R    0x02


uint8_t  evt_flg = 0;
uint8_t  OverflowCnt = 0;
uint16_t DischargeTime=0;
uint32_t secondsCounter = 0L;
uint8_t  dataBuffer[ PAYLOAD_LENGTH ];
uint8_t  dataIdx = 0;


int main()
{
    evt_flg = 0;
    SystemInit();
    uartInit();
    longDelay();
    __bis_SR_register(GIE);
    while(1) {
        run();
    }
}

/*
 * Initialize the MCU and used peripherals
 */
void SystemInit() 
{
    // Watchdog and clocks...
    WDTCTL = WDT_ADLY_1000; // WDT is clocked by fACLK -> 1s @32kHz
    IE1 |= WDTIE;
    // DCO @ 8 MHz
    BCSCTL1 = CALBC1_8MHZ;
    DCOCTL = CALDCO_8MHZ;
    // crystal oscillator
    BCSCTL3 |= LFXT1S_0; // LFXT1 = xtal
    IFG1 &= ~OFIFG;      // Clear OSCFault flag
    //--For VLO option instead of crystal: BCSCTL3 = LFXT1S_2; // Low-frequency clock select - VLOCLK
    //
    // IO...
    P2OUT = 0; // note: will be modified by the xbee initialization...
    P1DIR = 0;
    P1OUT = 0;
    P1DIR = GREEN_LED | RED_LED;
    P1DIR |= RDRIVE_OUT;
    P1SEL = RDRIVE_OUT;
    //
    // Comparator...
    CACTL1 = CAREF_2 | CAEX;  // VCAREF is applied to the - terminal | 0.50 × VCC
    CACTL2 = CAF | P2CA3;     // output filter | CA4 (pin6)
    CAPD = CAPD4;
    // Timer...
    TACTL = TASSEL_2 | ID_0 | MC_2;
    //
    dataBuffer[0] = 0xff;
    dataBuffer[1] = 0xff;
    dataBuffer[2] = 0xff;
    dataBuffer[3] = (uint8_t) SECONDS_BTW_MEASURMENTS;
    dataBuffer[4] = (uint8_t) DATA_LENGTH;
}


/*
 * Forever do: Measure, Send, Sleep...
 */
void run()
{
    // Measure, Send, Sleep...
    if( evt_flg & EVT_MEASURE_R ) {
        measureAndSendR();
        evt_flg &= ~EVT_MEASURE_R;
    }
    __low_power_mode_3();
}


/*
 * Measure the current sensor (LDR) resistance and send it if the
 * data buffer is full.
 *
 * Actually it measures and sends the number of sys clock periods
 * captured while the capacitor discharges from Vcc to Vcc/2, which
 * can be used to calculate R and the illuminance if the response
 * characteristic of the LDR is known. Similarly, if different sensor
 * element is used, like a thermistor, R and T (or whatever else) can
 * be calculated if needed. It may be not necessarily needed to make
 * precise calculations, if the goal is for example, to capture
 * relative changes of significant value.
 */
void measureAndSendR()
{
    // Green LED - on when measurement is in progress;
    // Red LED   - on when data is transmitted
    // The LEDs are intended to be used for test/debug only.
    GREEN_LED_ON();
    CACTL1 |= CAON;
    longDelay();
    TA0CCR0 = TAR + 64000;
    ///// CHARGE /////////////////////////////////////
    TA0CCTL0 = OUTMOD_1;                            // SET output mode
    TA0CCTL1 = CAP | CCIS_1 | CCIE | SCS | CM_2;
    __low_power_mode_0();
    // we do not use the charge cycle for measurement in this implementation;
    // just give it enough time to fully charge the capacitor
    longDelay();
    TAR = 64000;
    TA0CCR0 = 0;
    ///// DISCHARGE //////////////////////////////////
    TA0CCTL0 = OUTMOD_5;                            // RESET output mode
    TA0CCTL1 = CAP | CCIS_1 | CCIE | SCS | CM_1;    //  Capture
    TACTL &= ~TAIFG;
    OverflowCnt = 0;
    TACTL |= TAIE;
    __low_power_mode_0();
    DischargeTime = TA0CCR1 - TA0CCR0;
    ///// DONE DISCHARGE; DischargeTime has the timer captured value; OverflowCnt has the overflow count + 1
    CACTL1 &= ~CAON;
    //
    // save measured values
    dataBuffer[TXD_HEADER_SIZE+DATA_MODALITY*dataIdx+0] = OverflowCnt-1;                // overflow count, 0-based
    dataBuffer[TXD_HEADER_SIZE+DATA_MODALITY*dataIdx+1] = (DischargeTime >> 8) & 0xff;  // Discharge Time MSB
    dataBuffer[TXD_HEADER_SIZE+DATA_MODALITY*dataIdx+2] = DischargeTime & 0xff;         // Discharge Time LSB
    dataIdx++;
    if( dataIdx == BATCH_SIZE ) {
        // if the data buffer is full, then send the data frame to the base station
        sendData();
        dataIdx = 0;
    }
    GREEN_LED_OFF();
}


/*
 * WDT Interrupt handler
 */
#pragma vector=WDT_VECTOR
__interrupt void WDT_Interrupt ()
{
    ++secondsCounter;
    if (secondsCounter % SECONDS_BTW_MEASURMENTS == 0) {
        __low_power_mode_off_on_exit();
        evt_flg |= EVT_MEASURE_R;
    }
}


/*
 * Timer A1 Interrupt Handler
 */
#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_Interrupt ()
{
    unsigned int flag = TA0IV;
    if( flag == 0xA ) {
        OverflowCnt++;
        return;
    }
    TACTL &= ~TAIE;
    TA0CCTL1 = CAP | CM_0;
    __low_power_mode_off_on_exit();
}


/*
 * "Long delay" utility function
 */
void longDelay() {
    __delay_cycles( 1000000 );
}

/*
 * "short delay" utility function
 */
void shortDelay() {
    __delay_cycles( 10000 );
}

/*
 * Send the measured data to the base station
 */
void sendData() 
{
    RED_LED_ON(); GREEN_LED_OFF();
    uartSendBuffer ( dataBuffer, PAYLOAD_LENGTH);
    shortDelay();
    RED_LED_OFF();
}
