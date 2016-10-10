#ifndef _CATPLAT_ERROR_H_
#define _CATPLAT_ERROR_H_

#include <Arduino.h>

typedef enum COMMS_ERROR
{
	COMMS_NO_ERROR,
	SEND_ERROR,
	RCV_ERROR,
	WIFI_NOT_CONNECTED,
	SOCKET_NOT_OPEN,
	NO_ACK
};

typedef enum GPIO_ERROR
{
	GPIO_NO_ERROR,
	GPIO_NOT_AVAILABLE,
	GPIO_NOT_FOUND,
	GPIO_FAULTED
};

typedef enum MAIN_ERROR
{
	MAIN_NO_ERROR,
	MAIN_FAULTED
};

typedef enum MOTOR_ERROR
{
	MOTOR_NO_ERROR,
	MOTOR_FAULTED
};

typedef enum ENCODER_ERROR
{
	ENCODER_NO_ERROR,
	ENCODER_FAULTED
};

typedef enum ERROR_SOURCE
{
	COMMS,
	GPIO,
	MAIN,
	MOTOR,
	ENCODER
};

typedef struct SYSTEM_ERROR
{
	COMMS_ERROR commsError;
	GPIO_ERROR gpioError;
	MAIN_ERROR mainError;
	MOTOR_ERROR motorError;
	ENCODER_ERROR encoderError;

} SYSTEM_ERROR;

/* PROTOYPES */
int checkSystemErrors();
char *getErrorString();
int getErrorCode();
void setError(ERROR_SOURCE source, int errCode);
void clearSystemError ();

#endif /* _CATPLAT_ERROR_H_ */