#ifndef _CATPLAT_COMMS_H_
#define _CATPLAT_COMMS_H_

#include <Arduino.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <string.h>


#define BUFFER_SIZE 20
#define MAX_RETRY 200
#define MSG_CONTENT_LENGTH 10	/* Lenght of the content field of the messages (uint) */

typedef struct CATPLAT_MSG
{
	char msgType; /* S=speed, P=power, E=encoder */
	unsigned int msgContent[MSG_CONTENT_LENGTH];
	unsigned int msgLength;
} CATPLAT_MSG;

/* FUNCTIONS */
int readUDPMessage(CATPLAT_MSG *rcvMsg);
int sendUDPMessage(CATPLAT_MSG *sendMsg);

CATPLAT_MSG parseUDPMsg(char *rawMsg);
void parseUDPMsg(CATPLAT_MSG *parsedMsg);

void emptyStr(CATPLAT_MSG *str);

void setupComms();
void closeComms();
#endif /* _CATPLAT_COMMS_H_ */