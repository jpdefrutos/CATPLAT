/* Comment to erase log info */
//#define ENABLE_DEBUG 0

#include <CATPLAT.h>

#define DEEP_SLEEP_HOURS 24		/* Deep Sleep time. It's the last instruction of the program */

#define READ_TIMER 0			/* Every READ_TIMER iterations of the main loop, check the UDP socket */
#define WRITE_TIMER 20			/* Every WRITE_TIMER iteration os the main loop, send the pulse count */
#define HEART_BEAT_TIMER 20		/* Every HEART_BEAT_TIMER iterations of the main loop, toogle LED_BUILTIN state*/

#define MAIN_EXEC_TIMER 50		/* Delay between executions of the main process of the application */
/* ======================  VARIABLES ====================== */

#ifdef ENABLE_DEBUG
	static char logTmp[LOG_BUFFER_SIZE];
#endif /* ENABLE_DEBUG */


/* ======================  PROTOTYPES ====================== */

void heartBeat();



/* ======================    SETUP    ====================== */
void setup()
{
	int idx = 0;
	int encPins[2], leftMotor[2], rightMotor[2];

	clearSystemError();

	pinMode(LED_BUILTIN, OUTPUT);

	/*
	 * DEBUG SETUP
	 */
#ifdef ENABLE_DEBUG
	heartBeat(); /* ON */
	logSetup();
	logTrace(LOG_INFORMATIONAL, SOURCE_MAIN,"Debug enabled\n");
	heartBeat(); /* OFF */
#else
	Serial.begin(115200);
	Serial.printf("*** DEBUG DISABLED ***");
	Serial.end();
#endif /* ENABLE DEBUG */
	//Serial.begin(115200);
	/* 
	 * WiFi SETUP 
	 */
	heartBeat(); /* ON */
	setupComms();
	heartBeat(); /* OFF */
	/*
	 * PINS SETUP
	 */
	heartBeat();

	leftMotor[PIN_FORWARDS] = D2;
	leftMotor[PIN_BACKWARDS] = D1;

	rightMotor[PIN_FORWARDS] = D7;
	rightMotor[PIN_BACKWARDS] = D8;


	
	encPins[RIGHT_ENCODER] = D5;
	encPins[LEFT_ENCODER] = D6;

	if (checkSystemErrors())
	{
		/* Setup Motors*/
		setupMotors(rightMotor, leftMotor);
				
		setMotorSpeed(RIGHT_MOTOR, STOP);
		setMotorSpeed(LEFT_MOTOR, STOP);

		/* Set encoders */
		setupEncoders(encPins);

	}
	else
	{
		/* No communications with GUI. Set platform to low power mode and wait for user to reboot */
	};
	heartBeat();
};

/* ======================  MAIN LOOP  ====================== */
void loop()
{
	static int noError = 0;
	static int endProcess = 0;
	static int readSocketTimer = 0;
	static int writeSocketTimer = 0;
	static int heartBeatTimer = 0;
	static unsigned int currentTime = 0;

	

	CATPLAT_MSG rcvMsg, sendMsg;
	/* Reintialize to an empty structure */
	delay(0);
	
	emptyStr(&sendMsg);

	noError = checkSystemErrors();
	
	if (noError && !endProcess)
	{
		if (heartBeatTimer >= HEART_BEAT_TIMER)
		{
			heartBeat();
			heartBeatTimer = 0;
		}
		else
			heartBeatTimer++;

		/*Check UDP Socket*/

		if (readUDPMessage(&rcvMsg) && readSocketTimer >= READ_TIMER)
		{

#ifdef ENABLE_DEBUG
			logTmp[0] = '\0';
			sprintf(logTmp, "Rx %c", rcvMsg.msgType);
			logTrace(LOG_INFORMATIONAL, SOURCE_COMMS, logTmp);
#endif /* ENABLE_DEBUG*/

			readSocketTimer = 0;

			switch (rcvMsg.msgType)
			{
			case 'S':
				//Serial.printf("RCV: %d %d %d\n", rcvMsg.msgContent[0], rcvMsg.msgContent[1], rcvMsg.msgContent[2]);
				if (rcvMsg.msgContent[2])
				{
					/* PWM */
					//Serial.printf("PWM\n");
					setMotorSpeed(RIGHT_MOTOR, rcvMsg.msgContent[0]);
					setMotorSpeed(LEFT_MOTOR, rcvMsg.msgContent[1]);
				}
				else
				{
					//Serial.printf("DISCRETE\n");
					/* Discrete */
					setMotorSpeed(RIGHT_MOTOR, (MOTOR_DIR)(rcvMsg.msgContent[0]));
					setMotorSpeed(LEFT_MOTOR, (MOTOR_DIR)(rcvMsg.msgContent[1]));
				}
#ifdef ENABLE_DEBUG
				logTmp[0] = '\0';
				sprintf(logTmp, "Set speed: L: %d R: %d", rcvMsg.msgContent[1], rcvMsg.msgContent[0]);
				logTrace(LOG_INFORMATIONAL, SOURCE_COMMS, logTmp);
#endif /* ENABLE_DEBUG*/

				break;
			case 'X':
				endProcess = 1;

#ifdef ENABLE_DEBUG
				logTrace(LOG_INFORMATIONAL, SOURCE_COMMS, "Rx exit cmd ACK");
#endif /* ENABLE_DEBUG*/

				break;
			default:
				break;

				/* Acknowledge */
				emptyStr(&sendMsg);
				sendMsg.msgType = 'A';
				sendMsg.msgLength = 1;
				sendMsg.msgContent[0] = 0;
				sendUDPMessage(&sendMsg);

			delay(0);
			}

		}
		else
		{
			readSocketTimer++;
			/* No message received */
		};

		if (writeSocketTimer >= WRITE_TIMER)
		{
			delay(0);

			emptyStr(&sendMsg);

			writeSocketTimer = 0;
			
			sendMsg.msgType = 'S';
			sendMsg.msgLength = 10;

			os_intr_lock(); /* Do not modify these values */
			sendMsg.msgContent[0] = getPulsesCount(RIGHT_ENCODER);
			sendMsg.msgContent[1] = getPulsesCount(LEFT_ENCODER);
			os_intr_unlock();
			currentTime = millis();
			//Serial.printf("READ: %d %d\n", sendMsg.msgContent[0], sendMsg.msgContent[1]);
			resetPulsesCount();
			delay(0);

			sendMsg.msgContent[2] = currentTime - getLastUpdate(RIGHT_ENCODER);
			sendMsg.msgContent[3] = currentTime - getLastUpdate(LEFT_ENCODER);

			sendUDPMessage(&sendMsg);

			setLastUpdate(currentTime);

#ifdef ENABLE_DEBUG
			logTmp[0] = '\0';
			sprintf(logTmp, "Sent pulse count: R:%d L:%d", sendMsg.msgContent[0], sendMsg.msgContent[1]);
			logTrace(LOG_INFORMATIONAL, SOURCE_COMMS, logTmp);
#endif /* ENABLE_DEBUG*/
		}
		else
		{
			writeSocketTimer++;
		};

		delay(0); /* Prevent watchdog from firing */
		delay(MAIN_EXEC_TIMER);
	}
	else
	{
#ifdef ENABLE_DEBUG
		logTrace(LOG_INFORMATIONAL, SOURCE_COMMS, "Closing app");
#endif /* ENABLE_DEBUG*/

		setMotorSpeed(RIGHT_MOTOR, STOP);
		setMotorSpeed(LEFT_MOTOR, STOP);
		digitalWrite(LED_BUILTIN, LOW);

		/* END OF PLATFORM PROGRAM */
		if (!noError)
		{
			#ifdef ENABLE_DEBUG
				logTrace(LOG_ERROR, SOURCE_MAIN, "SYSTEM ERROR. Low Power mode");
			#endif /* ENABLE_DEBUG */

			emptyStr(&sendMsg);
			sendMsg.msgType = 'R';
			sendMsg.msgContent[0] = getErrorCode();
			sendMsg.msgLength = 1;

			sendUDPMessage(&sendMsg);
		}
		else
		{
			#ifdef ENABLE_DEBUG
				logTrace(LOG_INFORMATIONAL, SOURCE_MAIN, "Final log. Low Power mode");
			#endif /* ENABLE_DEBUG*/
		}

		emptyStr(&sendMsg);
		sendMsg.msgType = 'X';
		sendMsg.msgLength = 1;
		sendMsg.msgContent[0] = getErrorCode(); /* 0 no error, 1 error see log */
		sendUDPMessage(&sendMsg);
		delay(500);

#ifdef ENABLE_DEBUG
		logTrace(LOG_INFORMATIONAL, SOURCE_COMMS, "Sent exit application message 'X'");
#endif /* ENABLE_DEBUG*/
		
		ESP.deepSleep(DEEP_SLEEP_HOURS * 3600 * 1000000, RF_DISABLED);
		
	};
	/*
	 * Check UDP socket, process message, reply ACK
	 */
};

void heartBeat()
{
	static int ledOn = 0;

	if (ledOn)
		digitalWrite(LED_BUILTIN, LOW);
	else
		digitalWrite(LED_BUILTIN, HIGH);

	ledOn = !ledOn;
};