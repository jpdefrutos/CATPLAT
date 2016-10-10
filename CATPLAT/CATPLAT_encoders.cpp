#include <CATPLAT_encoders.h>

#ifdef ENABLE_DEBUG
#include <CATPLAT_log.h>
#endif /* ENABLE_DEBUG */

static int prevVal[2]; /* Left, Right */
os_timer_t debTimerRight, debTimerLeft;
static int encoder[2];				/* GPIO12 Right, GPIO14 Left*/
static unsigned int lastUpdate[2];
static int pulsesCount[2];

#ifdef ENABLE_DEBUG
	static char logTmp [LOG_BUFFER_SIZE];
#endif /* ENABLE_DEBUG */


/* FUCNTIONS */
void setupEncoders(int encPins[2])
{

#ifdef ENABLE_DEBUG
	logTrace(LOG_INFORMATIONAL, SOURCE_ENCODER, "Setup encoder");
#endif /* ENABLE_DEBUG*/

	int idx = 0;
	/* Set pins */
	encoder[RIGHT_ENCODER] = encPins[RIGHT_ENCODER];
	encoder[LEFT_ENCODER] = encPins[LEFT_ENCODER];
	
	for (idx = 0; idx < 2; idx++)
	{
		pinMode(encoder[idx], INPUT); /* Set interrupt pins to OUTPUT */
		digitalWrite(encoder[idx], LOW); /* Set initially to LOW to detect RISE, and viceversa */
	};
	
	/* Set timers */
	os_timer_setfn(&debTimerLeft, debounceInterrupt, (void *)LEFT_ENCODER);
	os_timer_setfn(&debTimerRight, debounceInterrupt, (void *)RIGHT_ENCODER);
#ifdef ENABLE_DEBUG
	logTrace(LOG_INFORMATIONAL, SOURCE_ENCODER, "Configured encoder timers");
#endif /* ENABLE_DEBUG*/

	/* Attach interrupts*/
	attachInterrupt(encoder[LEFT_ENCODER], pulseIntLeft, RISING);
	attachInterrupt(encoder[RIGHT_ENCODER], pulseIntRight, RISING);
#ifdef ENABLE_DEBUG
	logTrace(LOG_INFORMATIONAL, SOURCE_ENCODER, "Attached interrupts");
#endif /* ENABLE_DEBUG*/

	/* Initialize lastUpdate*/
	unsigned int currentTime = millis();
	lastUpdate[LEFT_ENCODER] = currentTime;
	lastUpdate[RIGHT_ENCODER] = currentTime;

	//Serial.print("INIT\n");
	pulsesCount[LEFT_ENCODER] = 0;
	pulsesCount[RIGHT_ENCODER] = 0;

	prevVal[LEFT_ENCODER] = 0;
	prevVal[RIGHT_ENCODER] = 0;
#ifdef ENABLE_DEBUG
	logTrace(LOG_INFORMATIONAL, SOURCE_ENCODER, "Initialized lastUpdate");
#endif /* ENABLE_DEBUG*/

};

void debounceInterrupt(void *arg) /* LEFT_ENCODER, RIGHT_ENCODER*/
{
	/* Add 1 (true) to the pulse counter, if equal */
	pulsesCount[(int)arg] += (prevVal[(int)arg] == digitalRead(encoder[(int)arg]));
};

void pulseIntRight()
{
	prevVal[RIGHT_ENCODER] = digitalRead(encoder[RIGHT_ENCODER]);
	os_timer_arm(&debTimerRight, DEBOUNCE_TIME_MS, false);
};

void pulseIntLeft()
{
	prevVal[LEFT_ENCODER] = digitalRead(encoder[LEFT_ENCODER]);
	os_timer_arm(&debTimerLeft, DEBOUNCE_TIME_MS, false);
};

unsigned int getLastUpdate (NO_ENCODER noEncoder)
{
	return lastUpdate[noEncoder];
};

void setLastUpdate (unsigned int time)
{

#ifdef ENABLE_DEBUG
	logTmp[0] = '\0';
	sprintf(logTmp, "Set lastUpdate %u", time);
	logTrace(LOG_INFORMATIONAL, SOURCE_ENCODER, logTmp);
#endif /* ENABLE_DEBUG*/

	lastUpdate[RIGHT_ENCODER] = time;
	lastUpdate[LEFT_ENCODER] = time;
};

int getPulsesCount (NO_ENCODER noEncoder)
{
#ifdef ENABLE_DEBUG
	logTmp[0] = '\0';
	sprintf(logTmp, "Get pulseCount %d for encoder %d", pulsesCount[noEncoder], noEncoder);
	logTrace(LOG_INFORMATIONAL, SOURCE_ENCODER, logTmp);
#endif /* ENABLE_DEBUG*/
	//Serial.printf("GET: %d %d\n", noEncoder, pulsesCount[noEncoder]);
	return pulsesCount[noEncoder];
};

void resetPulsesCount ()
{
	//Serial.print("RESET\n");
	os_intr_lock();
		pulsesCount[RIGHT_ENCODER] = 0;
		pulsesCount[LEFT_ENCODER] = 0;
	os_intr_unlock();
	
#ifdef ENABLE_DEBUG
	logTrace(LOG_INFORMATIONAL, SOURCE_ENCODER, "Reset pulsesCount");
#endif /* ENABLE_DEBUG*/
};