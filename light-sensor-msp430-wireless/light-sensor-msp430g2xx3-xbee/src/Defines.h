#ifndef DEFINES_H_
#define DEFINES_H_

#include <inttypes.h>

#define TRUE    1
#define FALSE   0

#define DEVICE_SUNSENSOR_1


////////////////////////////////////////////////
// the internal temperature sensor may have
// different offset for each device:
// calibrate and adjust this value per device
#ifdef DEVICE_SUNSENSOR_1
#define TEMP_CORRECTION -2
#endif
///


// Destination 64-bit address
//
#define DEST_ADDR_MSB     0x00000000
#define DEST_ADDR_LSB     0x00000000

/////////////////////////////////////
#define XBEE_SLEEP_ENABLED      TRUE
/////////////////////////////////////

#define SECONDS_BTW_MEASURMENTS     60
#define BATCH_SIZE                  15
#define DATA_MODALITY                6
#define TXD_HEADER_SIZE              3
#define PAYLOAD_XTRA                 5
#define DATA_LENGTH      DATA_MODALITY * BATCH_SIZE
#define PAYLOAD_LENGTH   DATA_LENGTH + PAYLOAD_XTRA

#define PACKET_DATA      0x10
#define PACKET_PING      0x01

// Port 1
//
#define RDRIVE_OUT        BIT5
#define TEST_INPUT        BIT5
// + Uart and sensor... + LEDs...
//
// Port 2
//
#define XBEE_CTS        BIT3
#define XBEE_RTS        BIT4
#define XBEE_SLEEP_RQ   BIT2

// delays @ 8MHz system clock

#define DELA8_1_MS      __delay_cycles( 8000L )
#define DELA8_10_MS     __delay_cycles( 10L*8000L )
#define DELA8_100_MS    __delay_cycles( 100L*8000L )
#define DELA8_1_S       __delay_cycles( 1000L*8000L )
#define DELA8_5_S       __delay_cycles( 5000L*8000L )



#endif /*DEFINES_H_*/
