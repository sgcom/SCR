/*
 *
 * SCR (Slope Conversion of Resistance) Sensor Node
 * ( Sunlight Measuring Sensor )
 *
 * Measures the light intensity (illuminance on a LDR photoresistor).
 * Sends the measurements to a base station over a wireless mesh
 * network implemented with XBee modules.
 *
 * Target MCU Device: MSP430G2553
 * 
 * -- Developed with "sunlight measuring" application in mind,
 * using an LDR as a sensor element. The primary motivation has
 * been to collect data about how many hours of sunlight are there
 * at specific location spots.
 *
 * -- Can be easily adapted for other purposes, using any
 * resistance-based sensor. For this specific application
 * the sensor node collects data regularly (once per minute)
 * and sends a data frame (packet) of raw measurements to the base
 * station once in a while (for example once every 15 , 20 or 30
 * minutes). All further data processing and analysis happens outside
 * this simple sensor node.
 *
 * -- The actual measured value is the time the capacitor needs
 * to discharge through the sensor resistance from Vcc to the
 * comparator reference voltage. For this application, only
 * relative values matter - there is no calibration.
 *
 * -- In addition, the sensor node sends the internal MCU temperature
 * and the Vcc/battery voltage.
 *
 * -- Intended to be used in the context of a bigger wireless
 * sensor network.
 *
 * Author: Stefan Ganev
 * License: LGPL http://www.gnu.org/copyleft/lesser.html
 * Free to use with no warranty and no expectations for support.
*/

#include <string.h>

#include "Defines.h"
#include "Board.h"
#include "xbeeInterface.h"


void SystemInit( void );
void longDelay( void );
void shortDelay( void );
void run( void );
void measureAndSendData( void );
int16_t measureVcc_x100( void );
int8_t measureInternalTemperature( void );
void sendData( void );
void doTest( void );

#define EVT_TEST         0x01
#define EVT_MEASURE_R    0x02
#define TEST_MODE        ((evt_flg & EVT_TEST) == EVT_TEST)
#define TEST_MODE_ON     evt_flg = EVT_TEST


uint8_t  evt_flg = 0;
uint8_t  OverflowCnt = 0;
uint16_t DischargeTime=0;
int16_t  adc10mem;
uint32_t secondsCounter = 0L;
uint8_t  dataBuffer[ PAYLOAD_LENGTH ];
uint8_t  dataIdx = 0;


/*
 * main
 */
int main()
{
    evt_flg = 0;
    SystemInit();
    if( (P2IN & TEST_INPUT) == 0 /* 0 is active*/) {
        TEST_MODE_ON;
    }
    zbInit( XBEE_SLEEP_ENABLED && ! TEST_MODE );
    longDelay();
    __bis_SR_register(GIE);
    while(1) {
        run();
    }
}

/*
 * MCU Initialization.
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
    //--For VLO option instead of crystal: BCSCTL3 = LFXT1S_2;
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
}


/*
 * Main loop: forever do {
 *   Measure,
 *   Send,
 *   Sleep;
 * } unless in test mode.
 */
void run()
{
    while( TEST_MODE ) {
        doTest();
        __low_power_mode_3();
    }
    if( evt_flg & EVT_MEASURE_R ) {
        evt_flg &= ~EVT_MEASURE_R;
        measureAndSendData();
    }
    __low_power_mode_3();
}

/*
 * Measure the current sensor (LDR) resistance
 * and send it if the data buffer is full.
 */
void measureAndSendData()
{
    // Green LED - on when measurement is in progress;
    // Red LED   - on when data is transmitted
    // Blue LED  - connected to the XBee association pin
    // The LEDs are intended for test/debug only. A jumper on the board lets disconnect them otherwise.
    GREEN_LED_ON();
    CACTL1 |= CAON;
    longDelay();
    TA0CCR0 = TAR + 64000;
    TA0CCTL0 = OUTMOD_1;                         // SET output mode
    TA0CCTL1 = CAP | CCIS_1 | CCIE | SCS | CM_2; //  Capture
    __low_power_mode_0();
    longDelay();
    TAR = 64000;
    TA0CCR0 = 0;
    TA0CCTL0 = OUTMOD_5;    // RESET output mode
    TA0CCTL1 = CAP | CCIS_1 | CCIE | SCS | CM_1;    //  Capture
    TACTL &= ~TAIFG;
    OverflowCnt = 0;
    TACTL |= TAIE;
    __low_power_mode_0();
    DischargeTime = TA0CCR1 - TA0CCR0;
    CACTL1 &= ~CAON;
    //
    int16_t v = measureVcc_x100();
    int8_t tempC = measureInternalTemperature();
    //
    // save measured values
    dataBuffer[TXD_HEADER_SIZE+DATA_MODALITY*dataIdx+0] = OverflowCnt-1;                // overflow count, 0-based
    dataBuffer[TXD_HEADER_SIZE+DATA_MODALITY*dataIdx+1] = (DischargeTime >> 8) & 0xff;  // Discharge Time MSB
    dataBuffer[TXD_HEADER_SIZE+DATA_MODALITY*dataIdx+2] = DischargeTime & 0xff;         // Discharge Time LSB
    dataBuffer[TXD_HEADER_SIZE+DATA_MODALITY*dataIdx+3] = (v >> 8) & 0xff;              // battery voltage*100 MSB
    dataBuffer[TXD_HEADER_SIZE+DATA_MODALITY*dataIdx+4] = v & 0xff;                     // battery voltage*100 LSB
    dataBuffer[TXD_HEADER_SIZE+DATA_MODALITY*dataIdx+5] = tempC;                        // internal MCU temperature *C
    dataIdx++;
    if( dataIdx == BATCH_SIZE ) {
        // if the data buffer is full, then send the data frame to the base station
        if( TRUE /*OverflowCnt < 3*/ ) {
            // TODO consider measuring the raw battery voltage (before it's regulated)
            // TODO or send the regulated voltage only once per data frame
            dataBuffer[TXD_HEADER_SIZE+DATA_LENGTH+0] = 0xFF;             // reserved
            dataBuffer[TXD_HEADER_SIZE+DATA_LENGTH+1] = 0xFF;             // reserved
            sendData();
            dataIdx = 0;
        } else {
            // TO BE CONCIDERED optionally - depending on the application:
            //    * if it is too dark do not send data to save battery...
            //    * or send data only when things change ...
            //    * ..and other things like that. For now everything is sent regularly.
            __no_operation();
        }
    }
    GREEN_LED_OFF();
}


/*
 * Measure Vcc using ADC10
 */
int16_t measureVcc_x100()
{
    //  SREF_1     -- VR+ = VREF+ and VR- = VSS
    //  REF2_5V    -- 2.5 V Ref.
    //  REFON      -- Reference on
    //  ADC10SHT_3 -- sample-and-hold time 64 × ADC10CLKs
    //  ADC10SR    -- sampling rate up to ~50 ksps
    //  ADC10ON    -- ADC10 on
    //
    ADC10CTL0 = SREF_1 | REF2_5V | ADC10SHT_3 | ADC10SR | ADC10IE;
    //
    //  INCH_11    -- Input channel select: (VCC - VSS) / 2
    //  SHS_0      -- Sample-and-hold source select: ADC10SC bit
    //  ADC10DIV_0 -- ADC10 clock divider: /1
    //  ADC10SSEL_0-- ADC10 clock source select: ADC10OSC
    //  CONSEQ_0   -- Conversion sequence mode select: Single-channel-single-conversion
    //
    ADC10CTL1 = INCH_11 | SHS_0 | ADC10DIV_3 | ADC10SSEL_0 | CONSEQ_0;
    //  ENC        -- Enable conversion
    //  ADC10SC    -- Start conversion
    ADC10CTL0 |= (REFON | ADC10ON);
    ADC10CTL0 |= (ENC | ADC10SC);
    __low_power_mode_3();
    //
    // Using floating point on this kind of device is sometimes
    // considered not so good practice - however pragmatically,
    // some emulated floating point is OK in the context of this
    // application.
    //
    float f_vcc_x100 = (float)adc10mem * 500.0f / 1024.0f + 0.5f;
    int i_vcc_x100 = (int16_t) f_vcc_x100;
    __no_operation();
    return i_vcc_x100;
}


/*
 * Measure the internal device temperature using the internal
 * temperature sensor and ADC10.
 */
int8_t measureInternalTemperature()
{
    int32_t iTemp;
    ADC10CTL0 = SREF_1 | ADC10SHT_3 | ADC10SR | ADC10IE;
    ADC10CTL1 = INCH_10 | SHS_0 | ADC10DIV_3 | ADC10SSEL_0 | CONSEQ_0;
    ADC10CTL0 |= (REFON | ADC10ON);
    ADC10CTL0 |= (ENC | ADC10SC);
    __low_power_mode_3();
    iTemp = adc10mem;
    iTemp = (iTemp * 420 + 512) / 1024 - 278 + TEMP_CORRECTION;
    return (int8_t)iTemp;
}


/*
 * ADC10 Interrupt handler
 */
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_Interrupt ()
{
    ADC10CTL0 &= ~( ENC | REFON | ADC10ON | ADC10IE );
    adc10mem = ADC10MEM;
    __low_power_mode_off_on_exit();
}


/*
 * WDT Interrupt handler
 */
#pragma vector=WDT_VECTOR
__interrupt void WDT_Interrupt ()
{
    ++secondsCounter;
    if ( TEST_MODE && (secondsCounter % 10 == 0) ) {
        __low_power_mode_off_on_exit();
    } else if (secondsCounter % SECONDS_BTW_MEASURMENTS == 0) {
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
    TA0CCTL1 = CAP | CM_0;    // disable further capture
    __low_power_mode_off_on_exit();
}


/*
 * Test mode is used to debug possible issues with the XBee
 * communication. In this mode the sensor node does not sleep and
 * sends meaningless data frequently.
 */
void doTest()
{
    memset(&dataBuffer[2], secondsCounter&0xff, PAYLOAD_LENGTH-2);
    RED_LED_OFF(); GREEN_LED_ON();
    longDelay();
    zbSendData(dataBuffer, FALSE);
    RED_LED_ON(); GREEN_LED_OFF();
    longDelay();
    RED_LED_OFF(); GREEN_LED_OFF();
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
    zbSendData(dataBuffer, XBEE_SLEEP_ENABLED);
    shortDelay();
    RED_LED_OFF();
}
