#ifndef _CATPLAT_ENCODERS_H_
#define _CATPLAT_ENCODERS_H_


#include <Arduino.h>
extern "C" {
#include <user_interface.h>
}


#define DEBOUNCE_TIME_MS 10		/* timer for debouncing the signal in millisencods */

typedef enum NO_ENCODER
{
	RIGHT_ENCODER,	/* 0 */
	LEFT_ENCODER	/* 1 */
};

/* FUNCTIONS */
void debounceInterrupt(void *arg); /* 0 Left, 1 Right*/
void pulseIntRight();
void pulseIntLeft();
void setupEncoders(int encPins[2]);
void setLastUpdate (unsigned int time);
unsigned int getLastUpdate (NO_ENCODER noEncoder);
int getPulsesCount (NO_ENCODER noEncoder);
void resetPulsesCount ();

#endif /* _CATPLAT_ENCODERS_H_ */