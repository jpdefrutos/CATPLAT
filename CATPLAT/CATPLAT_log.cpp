#include <CATPLAT_log.h>

static char *debugBuffer;

void logSetup()
{
	Serial.begin(115200);
	debugBuffer = (char*)malloc(LOG_BUFFER_SIZE);
	memset(debugBuffer, '\0', LOG_BUFFER_SIZE);
};

void logTrace(LOG_SEV severity, LOG_SOURCE source, char* message)
{
	static char sevStr[4];
	static char srcStr[4];

	memset(debugBuffer, '\0', LOG_BUFFER_SIZE);

	sprintf(sevStr, "%s", logSeverity(severity));
	sprintf(srcStr, "%s", logSource(source));

	sprintf(debugBuffer, "%u %s [%s]: %s", millis(), sevStr, srcStr, message);

	/* Send log string */
	Serial.printf("%s\n", debugBuffer);
	memset(debugBuffer, '\0', LOG_BUFFER_SIZE);
};

char* logSeverity(LOG_SEV severity)
{
	char ret[3] = { 'U', 'N', 'K' };
	switch (severity)
	{
	case LOG_ERROR:
		sprintf(ret, "ERR");
		break;
	case LOG_WARNING:
		sprintf(ret, "WAR");
		break;
	case LOG_INFORMATIONAL:
		sprintf(ret, "INF");
		break;
	default:
		break;
	}

	return ret;
};

char* logSource(LOG_SOURCE source)
{
	char ret[3] = { 'U', 'N', 'K' };
	switch (source)
	{
	case SOURCE_COMMS:
		sprintf(ret, "COM");
		break;
	case SOURCE_GPIO:
		sprintf(ret, "GIO");
		break;
	case SOURCE_MOTORS:
		sprintf(ret, "MOT");
		break;
	case SOURCE_ENCODER:
		sprintf(ret, "ENC");
		break;
	case SOURCE_MAIN:
		sprintf(ret, "MAI");
		break;
	default:
		break;
	}

	return ret;
};
