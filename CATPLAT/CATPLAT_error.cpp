#include <CATPLAT_error.h>

/* VARIABLES */
const char commErrorString[6][30] =
{
	"No error",
	"Failed sending message",
	"Failed receiving message",
	"WiFi not available",
	"UDP Socket not open",
	"No ack from GUI"
};

const char gpioErrorString[4][30] =
{
	"No error",
	"GPIO not available",
	"GPIO not found",
	"Unkown error"
};

const char mainErrorString[2][30] =
{
	"No error",
	"Unkown error"
};

const char motorErrorString[2][30] =
{
	"No error",
	"Unkown error"
};

const char encoderErrorString[2][30] =
{
	"No error",
	"Unkown error"
};

SYSTEM_ERROR systemError;

/* FUNCTIONS */
/* 1 if no errors, 0 otherwise */
int checkSystemErrors()
{
	return ((systemError.commsError == COMMS_NO_ERROR) &
			(systemError.gpioError == GPIO_NO_ERROR) &
			(systemError.mainError == MAIN_NO_ERROR) &
			(systemError.motorError == MOTOR_NO_ERROR)&
			(systemError.encoderError == ENCODER_NO_ERROR));
};

char *getErrorString()
{
	char *ret = "";
	
	if (systemError.commsError != COMMS_NO_ERROR)
		sprintf(ret, "COMMS:%s", commErrorString[systemError.commsError]);

	else if (systemError.encoderError != ENCODER_NO_ERROR)
		sprintf(ret, "ENC:%s", encoderErrorString[systemError.encoderError]);

	else if (systemError.gpioError != GPIO_NO_ERROR)
		sprintf(ret, "GPIO:%s", gpioErrorString[systemError.gpioError]);

	else if (systemError.mainError != MAIN_NO_ERROR)
		sprintf(ret, "MAIN:%s", mainErrorString[systemError.mainError]);

	else sprintf(ret, "MOTOR:%s", motorErrorString[systemError.motorError]);
		
	return ret;
};

int getErrorCode()
{
	int ret = 0;
	/*
	 *  0 (no error) to  9 -> COMMS
	 * 10 (no error) to 19 -> ENCODER
	 * 20 (no error) to 29 -> GPIO
	 * 30 (no error) to 39 -> MAIN
	 * 40 (no error) to 49 -> MOTORS
	 */
	if (systemError.commsError != COMMS_NO_ERROR)
		ret = systemError.commsError;

	else if (systemError.encoderError != ENCODER_NO_ERROR)
		ret = 10 + systemError.encoderError;

	else if (systemError.gpioError != GPIO_NO_ERROR)
		ret = 20 + systemError.gpioError;

	else if (systemError.mainError != MAIN_NO_ERROR)
		ret = 30 + systemError.mainError;

	else if (systemError.motorError != MOTOR_NO_ERROR)
		ret = 40 + systemError.motorError;

	return ret;
};

void setError(ERROR_SOURCE source, int errCode)
{
	switch (source)
	{
	case COMMS:
		systemError.commsError = (COMMS_ERROR) errCode;
		break;

	case GPIO:
		systemError.gpioError = (GPIO_ERROR) errCode;
		break;

	case MAIN:
		systemError.mainError = (MAIN_ERROR) errCode;
		break;

	case MOTOR:
		systemError.motorError = (MOTOR_ERROR) errCode;
		break;

	case ENCODER:
		systemError.encoderError = (ENCODER_ERROR) errCode;
		break;

	default:
		break;
	};
};

void clearSystemError ()
{
	systemError.commsError = COMMS_NO_ERROR;
	systemError.encoderError = ENCODER_NO_ERROR;
	systemError.gpioError = GPIO_NO_ERROR;
	systemError.mainError = MAIN_NO_ERROR;
	systemError.motorError = MOTOR_NO_ERROR;
};