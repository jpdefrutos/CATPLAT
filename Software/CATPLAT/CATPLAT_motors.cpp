#include <CATPLAT_motors.h>

#ifdef ENABLE_DEBUG
#include <CATPLAT_log.h>
#endif /* ENABLE_DEBUG */

static int leftMotor[] = { 0, 0 };		/* D1 Forwards, D2 Backwards*/
static int rightMotor[] = { 0, 0 };	/* D3 Forwards, D4 Backwards*/

#ifdef ENABLE_DEBUG
	static char logTmp [LOG_BUFFER_SIZE];
#endif /* ENABLE_DEBUG */

void setupMotors (int rightMotorPins[2], int leftMotorPins[2])
{
	int idx = 0;

#ifdef ENABLE_DEBUG
	logTrace(LOG_INFORMATIONAL, SOURCE_MOTORS, "Setup motors");
#endif /* ENABLE_DEBUG*/

	rightMotor[PIN_FORWARDS] = rightMotorPins[PIN_FORWARDS];
	rightMotor[PIN_BACKWARDS] = rightMotorPins[PIN_BACKWARDS];
	
	leftMotor[PIN_FORWARDS] = leftMotorPins[PIN_FORWARDS];
	leftMotor[PIN_BACKWARDS] = leftMotorPins[PIN_BACKWARDS];

	for (idx = 0; idx < 2; idx++)
	{
		pinMode(leftMotor[idx], OUTPUT);
		pinMode(rightMotor[idx], OUTPUT);

		digitalWrite(leftMotor[idx], LOW);
		digitalWrite(rightMotor[idx], LOW);
	};

	analogWriteFreq(PWM_FREQUENCY_HZ); /* <= 5 kHz. See LM293D datasheet */
};

void setMotorSpeed(MOTOR_SELECTION selectMotor, MOTOR_DIR dir)
{
	int motorPins[] = { 0, 0 };
	
	switch (selectMotor)
	{
	case RIGHT_MOTOR:
		motorPins[PIN_FORWARDS] = rightMotor[PIN_FORWARDS];
		motorPins[PIN_BACKWARDS] = rightMotor[PIN_BACKWARDS];
		break;

	case LEFT_MOTOR:
		motorPins[PIN_FORWARDS] = leftMotor[PIN_FORWARDS];
		motorPins[PIN_BACKWARDS] = leftMotor[PIN_BACKWARDS];
		break;

	default:
#ifdef ENABLE_DEBUG
		logTrace(LOG_WARNING, SOURCE_MOTORS, "Motor not selected");
#endif /* ENABLE_DEBUG*/

		return;
		break;
	}

#ifdef ENABLE_DEBUG
	logTmp[0] = '\0';
	sprintf(logTmp, "Set motor %d to %d\n", selectMotor, dir);
	logTrace(LOG_INFORMATIONAL, SOURCE_MOTORS, logTmp);
#endif /* ENABLE_DEBUG*/

	switch (dir)
	{
		case STOP:
			digitalWrite(motorPins[PIN_FORWARDS], LOW);
			digitalWrite(motorPins[PIN_BACKWARDS], LOW);
			break;

		case FORWARDS:
			digitalWrite(motorPins[PIN_FORWARDS], HIGH);
			digitalWrite(motorPins[PIN_BACKWARDS], LOW);
			break;

		case BACKWARDS:
			digitalWrite(motorPins[PIN_FORWARDS], LOW);
			digitalWrite(motorPins[PIN_BACKWARDS], HIGH);
			break;

		default:
		break;
	};
	
};

void setMotorSpeed(MOTOR_SELECTION selectMotor, unsigned int speed)
{
	int motorPins[] = { 0, 0 };

	switch (selectMotor)
	{
	case RIGHT_MOTOR:
		motorPins[PIN_FORWARDS] = rightMotor[PIN_FORWARDS];
		motorPins[PIN_BACKWARDS] = rightMotor[PIN_BACKWARDS];
		break;

	case LEFT_MOTOR:
		motorPins[PIN_FORWARDS] = leftMotor[PIN_FORWARDS];
		motorPins[PIN_BACKWARDS] = leftMotor[PIN_BACKWARDS];
		break;

	default:
#ifdef ENABLE_DEBUG
		logTrace(LOG_WARNING, SOURCE_MOTORS, "Motor not selected");
#endif /* ENABLE_DEBUG*/

		return;
		break;
	}

#ifdef ENABLE_DEBUG
	logTmp[0] = '\0';
	sprintf(logTmp, "Set motor %d to %d\n", selectMotor, dir);
	logTrace(LOG_INFORMATIONAL, SOURCE_MOTORS, logTmp);
#endif /* ENABLE_DEBUG*/
	
	if ((speed >= 0) & (speed < 255))
	{
		analogWrite(motorPins[PIN_FORWARDS], LOW);
		analogWrite(motorPins[PIN_BACKWARDS], (255 - speed));
	}
	else if ((speed >= 255))
	{
		analogWrite(motorPins[PIN_FORWARDS], (speed - 255));
		analogWrite(motorPins[PIN_BACKWARDS], LOW);
	};
	
};