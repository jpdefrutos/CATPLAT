#ifndef _CATPLAT_MOTORS_H_
#define _CATPLAT_MOTORS_H_

#include <Arduino.h>

#define PIN_FORWARDS 0			/* position in the motor pin vector to move it forwards */
#define PIN_BACKWARDS 1			/* position in the motor pin vector to move it backwardsm*/
#define PWM_FREQUENCY_HZ 100	/* PWM Frequency (Hz) */

typedef enum MOTOR_DIR
{
	STOP,
	FORWARDS,
	BACKWARDS
};

typedef enum MOTOR_SELECTION
{
	RIGHT_MOTOR,
	LEFT_MOTOR
};
/* FUNCTIONS */
void setupMotors (int rightMotorPins[2], int leftMotorPins[2]);
void setMotorSpeed(MOTOR_SELECTION selectMotor, MOTOR_DIR dir);
void setMotorSpeed(MOTOR_SELECTION selectMotor, unsigned int speed);

#endif /* _CATPLAT_MOTORS_H_ */