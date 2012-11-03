#ifndef DEFINES_H_
#define DEFINES_H_

#include <inttypes.h>

#define TRUE    1
#define FALSE   0

#define SECONDS_BTW_MEASURMENTS      5
#define BATCH_SIZE                   3
#define DATA_MODALITY                3
#define TXD_HEADER_SIZE              5
#define PAYLOAD_XTRA                 5
#define DATA_LENGTH      DATA_MODALITY * BATCH_SIZE
#define PAYLOAD_LENGTH   DATA_LENGTH + PAYLOAD_XTRA

// Port 1
//
#define RDRIVE_OUT        BIT5
//... + Uart and sensor... + LEDs...
//
// Port 2
//
//...

// delays @ 8MHz system clock

#define DELA8_1_MS      __delay_cycles( 8000L )
#define DELA8_10_MS     __delay_cycles( 10L*8000L )
#define DELA8_100_MS    __delay_cycles( 100L*8000L )
#define DELA8_1_S       __delay_cycles( 1000L*8000L )
#define DELA8_5_S       __delay_cycles( 5000L*8000L )



#endif /*DEFINES_H_*/
