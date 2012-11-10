#include <string.h>
#include "xbeeInterface.h"
#include "uartG2xx3.h"
#include "Board.h"
#include "Defines.h"

/*
 * Functions to support the XBee communication.
 *
 * XBee configuration check list:
 *   o Set your PAN ID
 *   o Enable API MODE = 2 (AP2)
 *   o Enable hardware flow control - CTS/RTS
 *   o Enable sleep mode (SM1) - PIN HIBERNATE (Pin 9)
 *   o Enable associated indicator (DIO5/AssociatedInd) (Pin 15)
 * On all XBee routers and the coordinator - set long enough SN and
 * SP, depending on how often the data packets are sent. For 15min
 * period of sleep between data sessions, the recommended values are:
 *   o SN = 0x2D
 *   o SP = 0x7D0
 */
 

#define DEFAULT_ADDR16      0xFFFE
#define ZB_TX_REQUEST       0x10
#define START_BYTE          0x7e
#define ESCAPE              0x7d
#define XON                 0x11
#define XOFF                0x13

#define _RTS_ACTIVATE        P2OUT &= ~XBEE_RTS
#define _RTS_DEACTIVATE      P2OUT |= XBEE_RTS
#define SLEEP_RQ_ACTIVATE    P2OUT |= XBEE_SLEEP_RQ
#define SLEEP_RQ_DEACTIVATE  P2OUT &= ~XBEE_SLEEP_RQ

typedef struct {
    uint32_t address_msb;
    uint32_t address_lsb;
} ZbAddress64;


typedef struct {
    uint8_t frameId;
    ZbAddress64 addr64;
    uint16_t addr16;
    uint8_t broadcastRadius;
    uint8_t options;
    uint8_t *payloadPtr;
    uint8_t payloadLength;
} ZbTxFrame;

ZbTxFrame txFrame;

void sendByte( uint8_t byte, int escape );
void zbTransmitReq();

#define RXBUF_SIZE    0x20
uint8_t rxDataBuf[RXBUF_SIZE];
uint8_t *rxp;


void uartReceivedCharacterCallback(uint8_t ch)
{
    if(ch == START_BYTE) {
        rxp = rxDataBuf;
    }
    if(rxp < rxDataBuf + RXBUF_SIZE) {
        *rxp++ = ch;
    } else {
        // TODO Overflow...
    }
}

void zbInit(int sleepEnabled)
{
    P2DIR = XBEE_RTS | XBEE_SLEEP_RQ;
    rxp = rxDataBuf;
    txFrame.frameId = 0;
    txFrame.addr64.address_msb = DEST_ADDR_MSB;
    txFrame.addr64.address_lsb = DEST_ADDR_LSB;
    txFrame.addr16 = DEFAULT_ADDR16;
    txFrame.broadcastRadius = 0;
    txFrame.options = 0;

    uartInit();

    _RTS_ACTIVATE;          // Activate /RTS
    SLEEP_RQ_DEACTIVATE;    // Awake XBee
    if(sleepEnabled) {
        // a few seconds chance to associate, and put XBee to sleep
        int i;
        for(i=0; i< 10000; i++) {
            DELA8_1_MS;
        }
        zbSleep();
    }

}


void zbSleep()
{
    // Wait fot Tx Status frame,
    // but if it does not come, proceed anyway.
    memset(rxDataBuf, 0, RXBUF_SIZE);
    DELA8_5_S;
    SLEEP_RQ_ACTIVATE; // let the radio sleeep until next time
}


void zbAwake()
{
    memset(rxDataBuf, 0, RXBUF_SIZE);
    SLEEP_RQ_DEACTIVATE;     // Awake XBee
    int i;
    // Wait fot ModemStatus:JoinedNetwork, but if it does not come, proceed anyway; next time will try again...
    for(i=0; i< 10000; i++) {   // wait up to 10 sec for modem status 'joined network'
        DELA8_1_MS;
        if( rxDataBuf[3] == 0x8a && rxDataBuf[4] == 2 ) {
            // success
            DELA8_100_MS;
            _no_operation();
            return;
        }
    }
    // TODO possibly indicate that XBee could not re-join the network after awaken from sleep;
    _no_operation();
}


void zbSendData(uint8_t *dataBuffer, int sleepEnabled)
{
    _RTS_ACTIVATE;
    if(sleepEnabled) {
        zbAwake();
    }
    dataBuffer[0] = PACKET_DATA;
    dataBuffer[1] = (uint8_t) SECONDS_BTW_MEASURMENTS;
    dataBuffer[2] = (uint8_t) DATA_LENGTH;
    txFrame.payloadPtr = dataBuffer;
    txFrame.payloadLength = PAYLOAD_LENGTH;
    zbTransmitReq();
    if(sleepEnabled) {
        zbSleep();
    }
    DELA8_100_MS;
    _RTS_DEACTIVATE;
}



void zbTransmitReq()
{
    uint16_t frameDataLen = 14 + txFrame.payloadLength;
    uint8_t msbLen = (frameDataLen >> 8) & 0xff;
    uint8_t lsbLen = frameDataLen & 0xff;
    uint8_t byte = 0;
    uint8_t checksum = 0;
    if( ++txFrame.frameId == 0 ) {
        txFrame.frameId = 1;
    }
    sendByte(START_BYTE, FALSE);
    sendByte(msbLen, TRUE);
    sendByte(lsbLen, TRUE);
    sendByte(ZB_TX_REQUEST, TRUE); checksum += ZB_TX_REQUEST;
    sendByte(txFrame.frameId, TRUE);  checksum += txFrame.frameId;

    byte= ((txFrame.addr64.address_msb >> 24) & 0xff); sendByte( byte, TRUE);  checksum += byte;
    byte= ((txFrame.addr64.address_msb >> 16) & 0xff); sendByte( byte, TRUE);  checksum += byte;
    byte= ((txFrame.addr64.address_msb >> 8) & 0xff);  sendByte( byte, TRUE);  checksum += byte;
    byte= (txFrame.addr64.address_msb & 0xff);         sendByte( byte, TRUE);  checksum += byte;

    byte= ((txFrame.addr64.address_lsb >> 24)& 0xff);  sendByte( byte, TRUE);  checksum += byte;
    byte= ((txFrame.addr64.address_lsb >> 16) & 0xff); sendByte( byte, TRUE);  checksum += byte;
    byte= ((txFrame.addr64.address_lsb >> 8) & 0xff);  sendByte( byte, TRUE);  checksum += byte;
    byte= (txFrame.addr64.address_lsb & 0xff);         sendByte( byte, TRUE);  checksum += byte;

    byte= ((txFrame.addr16 >> 8) & 0xff);              sendByte( byte, TRUE); checksum += byte;
    byte= (txFrame.addr16 & 0xff);                     sendByte( byte, TRUE); checksum += byte;

    sendByte( txFrame.broadcastRadius, TRUE); checksum += txFrame.broadcastRadius;
    sendByte( txFrame.options, TRUE); checksum += txFrame.options;

    uint8_t *bp = txFrame.payloadPtr;
    uint8_t len = txFrame.payloadLength;
    while(len > 0) {
        len--;
        byte = *bp++;
        sendByte(byte, TRUE); checksum += byte;
    }
    checksum = 0xff - checksum;
    sendByte(checksum, TRUE);
}


void sendByte( uint8_t b, int escape )
{
    while( P2IN & XBEE_CTS ) {
        // wait until the radio is awake and/or clear to send
        _no_operation();
    }
    if (escape && (b == START_BYTE || b == ESCAPE || b == XON || b == XOFF)) {
        uartSendChar(ESCAPE);
        uartSendChar(b ^ 0x20);
    } else {
        uartSendChar(b);
    }
}
