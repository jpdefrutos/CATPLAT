#ifndef _CATPLAT_LOG_H_
#define _CATPLAT_LOG_H_

#include <Arduino.h>

#define LOG_BUFFER_SIZE 1024

typedef enum LOG_SEV
{
	LOG_INFORMATIONAL,
	LOG_WARNING,
	LOG_ERROR,
};

typedef enum LOG_SOURCE
{
	SOURCE_COMMS,
	SOURCE_GPIO,
	SOURCE_MAIN,
	SOURCE_MOTORS,
	SOURCE_ENCODER,
};

/* FUNCTIONS */
void logSetup();
void logTrace(LOG_SEV severity, LOG_SOURCE source, char* message);
char* logSeverity(LOG_SEV severity);
char* logSource(LOG_SOURCE source);

#endif /* _CATPLAT_LOG_H_ */