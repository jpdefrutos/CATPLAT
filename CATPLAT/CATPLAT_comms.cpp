#include <CATPLAT_comms.h>
#include <CATPLAT_error.h>

#ifdef ENABLE_DEBUG
#include <CATPLAT_log.h>
#endif /* ENABLE_DEBUG */

static char sendBuffer[BUFFER_SIZE] = "";
static char rcvBuffer[BUFFER_SIZE] = "";

const char* WiFiSSID = "caterpillarWiFi";
const char* WiFiPass = "caterpillarPlatform";

static IPAddress UDPIP;
const unsigned int UDPPort = 3500;

static IPAddress GUIIP;
static unsigned int GUIPort = 3000;

static IPAddress BroadcastIP;

static WiFiUDP UDPSocket;

#ifdef ENABLE_DEBUG
	static char logTmp[LOG_BUFFER_SIZE];
#endif /* ENABLE_DEBUG */

void setupComms()
{
	static int rcvPacketSize = 0;
	static int noRetries = 0;

	WiFi.disconnect(true);
	delay(10);
	WiFi.disconnect(false);
	WiFi.persistent(false);
	WiFi.mode(WIFI_OFF);
	WiFi.mode(WIFI_STA);
	WiFi.begin(WiFiSSID, WiFiPass);

/*#ifdef ENABLE_DEBUG
	logTmp[0] = '\0';
	sprintf(logTmp, "Connecting to WiFi SSID: %s PASS: %s", WiFiSSID, WiFiPass);
	logTrace(LOG_INFORMATIONAL, SOURCE_COMMS, logTmp);
#endif /* ENABLE_DEBUG*/

	do
	{
		delay(0);
		delay(500);
		noRetries = noRetries + 1;
		Serial.printf(".");
	} while (WiFi.status() != WL_CONNECTED);// && noRetries < MAX_RETRY);
	
	Serial.printf("\n");
	if (0)//noRetries >= MAX_RETRY)
	{
		/* No Wifi */
		setError (COMMS, WIFI_NOT_CONNECTED);

/*#ifdef ENABLE_DEBUG
		logTrace(LOG_ERROR, SOURCE_COMMS, "WiFi not connected");
#endif /* ENABLE_DEBUG*/

	}
	else
	{
		/* Platform IP */
		UDPIP = WiFi.localIP();

/*#ifdef ENABLE_DEBUG
		logTmp[0] = '\0';
		sprintf(logTmp, "WiFi Connected IP:%s Port:%d", UDPIP.toString().c_str(), UDPPort);
		logTrace(LOG_INFORMATIONAL, SOURCE_COMMS, logTmp);
#endif /* ENABLE_DEBUG*/

		/* Broadcast IP */
		BroadcastIP = ~WiFi.subnetMask() | WiFi.gatewayIP();

/*#ifdef ENABLE_DEBUG
		logTmp[0] = '\0';
		sprintf(logTmp, "BroadcastIP %s", BroadcastIP.toString().c_str());
		logTrace(LOG_INFORMATIONAL, SOURCE_COMMS, logTmp);
#endif /* ENABLE_DEBUG*/
		
		if (UDPSocket.begin(UDPPort))
		{

/*#ifdef ENABLE_DEBUG
			logTrace(LOG_INFORMATIONAL, SOURCE_COMMS, "UDP Socket open");
#endif /* ENABLE_DEBUG*/

			/* Send broadcast message */
			sendBuffer[0] = '\0';
			UDPSocket.beginPacket(BroadcastIP, GUIPort);
			sprintf(sendBuffer, "I,%d,%s,%u\0", UDPIP.toString().length() , UDPIP.toString().c_str(), UDPPort);
			UDPSocket.write(sendBuffer, strlen(sendBuffer));

			if (UDPSocket.endPacket())
			{

/*#ifdef ENABLE_DEBUG
				logTmp[0] = '\0';
				sprintf(logTmp, "Tx to %d@%s: %s", GUIPort, BroadcastIP.toString().c_str(), sendBuffer);
				logTrace(LOG_INFORMATIONAL, SOURCE_COMMS, logTmp);
#endif /* ENABLE_DEBUG*/

				/* Wait for the ACK of the GUI*/
				noRetries = 0;
				while (!UDPSocket.parsePacket() && noRetries < MAX_RETRY)
				{
					delay(0); /* Prevent watchdog from firing */
					delay(1000);
					
					UDPSocket.beginPacket(BroadcastIP, GUIPort);
					sprintf(sendBuffer, "I,%d,%s,%u\0", UDPIP.toString().length(), UDPIP.toString().c_str(), UDPPort);
					UDPSocket.write(sendBuffer, strlen(sendBuffer));
					UDPSocket.endPacket();

/*#ifdef ENABLE_DEBUG
					logTmp[0] = '\0';
					sprintf(logTmp, "Tx to %d@%s: %s", GUIPort, BroadcastIP.toString().c_str(), sendBuffer);
					logTrace(LOG_INFORMATIONAL, SOURCE_COMMS, logTmp);
#endif /* ENABLE_DEBUG*/

					noRetries = noRetries + 1;

				};

				if (noRetries < MAX_RETRY)
				{

					rcvPacketSize = UDPSocket.read(rcvBuffer, BUFFER_SIZE);
				
					if (rcvPacketSize)
					{
						rcvBuffer[rcvPacketSize] = '\0';
					}
					else
					{
						/* Empty packet */
						setError (COMMS, NO_ACK);
					};

/*#ifdef ENABLE_DEBUG
					logTmp[0] = '\0';
					sprintf(logTmp, "Rx Broadcast ACK: %s", rcvBuffer);
					logTrace(LOG_INFORMATIONAL, SOURCE_COMMS, logTmp);
#endif /* ENABLE_DEBUG*/

				}
				else
				{
					/* No answer from GUI*/
					setError (COMMS, NO_ACK);

/*#ifdef ENABLE_DEBUG
					logTrace(LOG_ERROR, SOURCE_COMMS, "No broadcast ACK");
#endif /* ENABLE_DEBUG*/

				};
			}
			else
			{
/*#ifdef ENABLE_DEBUG
				logTrace(LOG_ERROR, SOURCE_COMMS, "Tx broadcast msg");
#endif /* ENABLE_DEBUG*/

				/* Error sending broadcast message */
				setError (COMMS, SEND_ERROR);
			};
		}
		else
		{
/*#ifdef ENABLE_DEBUG
			logTrace(LOG_ERROR, SOURCE_COMMS, "UDP Socket not open");
#endif /* ENABLE_DEBUG*/

			/* Error opening UDP socket */
			setError (COMMS, SOCKET_NOT_OPEN);
		};
	};
};

int readUDPMessage(CATPLAT_MSG *rcvMsg)
{
	int ret = 0;

	emptyStr(rcvMsg);
	ret = UDPSocket.parsePacket();

	if (ret)
	{
		ret = UDPSocket.read(rcvBuffer, BUFFER_SIZE);
		if (ret)
		{
			rcvBuffer[ret] = '\0';
		}

#ifdef ENABLE_DEBUG
		logTmp[0] = '\0';
		sprintf(logTmp, "Rx new msg: %s L:%d", rcvBuffer, ret);
		logTrace(LOG_INFORMATIONAL, SOURCE_COMMS, logTmp);
#endif /* ENABLE_DEBUG*/

		GUIIP = UDPSocket.remoteIP();
		GUIPort = UDPSocket.remotePort();
		*rcvMsg = parseUDPMsg(rcvBuffer);
		
#ifdef ENABLE_DEBUG
		logTmp[0] = '\0';
		sprintf(logTmp, "Rx->(%d@%s): %c", GUIPort, GUIIP.toString().c_str(), rcvMsg->msgType);
		logTrace(LOG_INFORMATIONAL, SOURCE_COMMS, logTmp);
#endif /* ENABLE_DEBUG*/
	}
	else
	{
		/* No packet available */
		/* THIS IS NOT AN ERROR!!*/
#ifdef ENABLE_DEBUG
		logTrace(LOG_WARNING, SOURCE_COMMS, "No msg");
#endif /* ENABLE_DEBUG*/
	}
	return ret;
}

int sendUDPMessage(CATPLAT_MSG *sendMsg)
{
	parseUDPMsg(sendMsg);

	UDPSocket.beginPacket(GUIIP, GUIPort);
	UDPSocket.clearWriteError();
	UDPSocket.write(sendBuffer, strlen(sendBuffer));


#ifdef ENABLE_DEBUG
	logTmp[0] = '\0';
	sprintf(logTmp, "Tx to %d@%s:%s", GUIPort, GUIIP.toString().c_str(), sendBuffer);
	logTrace(LOG_INFORMATIONAL, SOURCE_COMMS, logTmp);
#endif /* ENABLE_DEBUG*/

	return UDPSocket.endPacket();
};

CATPLAT_MSG parseUDPMsg(char *rawMsg)
{
	char tmpBuff[10];
	char *strtokRet;
	CATPLAT_MSG parsedMsg;
	emptyStr(&parsedMsg);
	
	/*
	* rawMsg = | type (1 byte) | No.Bytes (1 byte) | content ([0,n] bytes) | CRC (tbd)
	*/
#ifdef ENABLE_DEBUG
	logTrace(LOG_INFORMATIONAL, SOURCE_COMMS, "Parsing raw msg");
#endif /* ENABLE_DEBUG*/

	parsedMsg.msgType = *strtok_r(rawMsg, ",", &strtokRet);
	
	switch (parsedMsg.msgType)
	{
	case 'S':
			parsedMsg.msgLength = atoi(strtok_r(NULL, ",",&strtokRet));

			parsedMsg.msgContent[0] = atoi(strtok_r(NULL, ",", &strtokRet)); /* Right Motor */

			parsedMsg.msgContent[1] = atoi(strtok_r(NULL, ",", &strtokRet)); /* Left Motor */

			parsedMsg.msgContent[2] = atoi(strtok_r(NULL, "\0", &strtokRet)); /* PWM? */

#ifdef ENABLE_DEBUG
			logTmp[0] = '\0';
			sprintf(logTmp, "Parsed: %c msg: L:%d RM:%d LM:%d", parsedMsg.msgType, parsedMsg.msgLength, parsedMsg.msgContent[0], parsedMsg.msgContent[1]);
			logTrace(LOG_INFORMATIONAL, SOURCE_COMMS, logTmp);
#endif /* ENABLE_DEBUG*/

		break;
	default: /* P, E, X are requests. A is informative. All have empty content */
#ifdef ENABLE_DEBUG
		logTmp[0] = '\0';
		sprintf(logTmp, "Parsed: %c msg", parsedMsg.msgType);
		logTrace(LOG_INFORMATIONAL, SOURCE_COMMS, logTmp);
#endif /* ENABLE_DEBUG*/
		break;
	}
	
	return parsedMsg;
};

void parseUDPMsg(CATPLAT_MSG *parsedMsg)
{
	memset(sendBuffer, '\0', BUFFER_SIZE);
	static char* tmp;
	tmp = "";

	/*
	* rawMsg = | type (1 byte) | content ([0,n] bytes) | CRC (TBD)
	*/

	switch (parsedMsg->msgType)
	{
	case 'S':
		sprintf(sendBuffer, "S,%d,%u,%u,%u,%u", parsedMsg->msgLength,
												parsedMsg->msgContent[0],
												parsedMsg->msgContent[1],
												parsedMsg->msgContent[2],
												parsedMsg->msgContent[3]);
		break;
	case 'P':
		//sendBuffer[1] = battery voltage cell 1
		//sendBuffer[2] = battery voltage cell 2
		break;

	case 'X': /* Finish */
	case 'R':
		sprintf(sendBuffer, "%c,%d,%u", parsedMsg->msgType, parsedMsg->msgLength, parsedMsg->msgContent[0]);
		break;

	case 'A': /* ACK */
	default: /* P, E are requests. Empty content */
		sendBuffer[0] = parsedMsg->msgType;
		break;
	};

#ifdef ENABLE_DEBUG
	logTmp[0] = '\0';
	sprintf(logTmp, "Built msg: %s", sendBuffer);
	logTrace(LOG_INFORMATIONAL, SOURCE_COMMS, logTmp);
#endif /* ENABLE_DEBUG*/
};

void closeComms()
{
	UDPSocket.stop();
	WiFi.disconnect(true);

#ifdef ENABLE_DEBUG
	logTrace(LOG_INFORMATIONAL, SOURCE_COMMS, "Closed WiFi and UDP Socket");
#endif /* ENABLE_DEBUG*/
};

void emptyStr(CATPLAT_MSG *str)
{
#ifdef ENABLE_DEBUG
	logTrace(LOG_INFORMATIONAL, SOURCE_COMMS, "Emptied CATPLAT_MSG structure");
#endif /* ENABLE_DEBUG*/

	str->msgType = '\0';
	str->msgLength = 0;
	memset(str->msgContent, 0, MSG_CONTENT_LENGTH);
};