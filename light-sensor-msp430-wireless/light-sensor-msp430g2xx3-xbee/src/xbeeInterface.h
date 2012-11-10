#ifndef XBEEINTERFACE_H_
#define XBEEINTERFACE_H_

#include <inttypes.h>

void zbInit( int sleepEnabled );
void zbSleep( void );
void zbAwake( void );
void zbSendData( uint8_t *dataBuffer, int sleepEnabled );

#endif /*XBEEINTERFACE_H_*/
