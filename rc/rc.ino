
#pragma region -------------------------------------------------------- Includes

#include <PS2X_lib.h>
#include "PwmMotorDriver.h"
#include <Servo.h>

#pragma endregion

#pragma region --------------------------------------------------------- Defines

#define BAUD_RATE 57600

// General constansts

#define LOOP_DELAY 50

// Speed constansts

#define SPEED_DEAD_ZONE 2
#define MAX_FORWARD_SPEED 255
#define MAX_BACKWARDS_SPEED 128

// Servo constansts

#define SERVO_MIN 55
#define SERVO_MAX 125

#pragma endregion

#pragma region ---------------------------------------------------- Arduino pins

// Motors

const uint8_t motor1A = 2;
const uint8_t motor1B = 4;
const uint8_t motor2A = 5;
const uint8_t motor2B = 6;
const uint8_t motor3A = 7;
const uint8_t motor3B = 8;
const uint8_t motor4A = A0;
const uint8_t motor4B = A1;

// Servos

const uint8_t servoPin1 = 3;
const uint8_t servoPin2 = 9;

// PS2 receiver pins

const uint8_t attention = 10;	// CS (orange)
const uint8_t command = 11;		// D0 (green)
const uint8_t data = 12;		// D1 (blue)
const uint8_t clock = 13;		// CLK (gray)

// DIP switches

const uint8_t dip1_2 = A2;
const uint8_t dip3_4 = A3;

// Inputs

const uint8_t analog1 = A6;
const uint8_t analog2 = A7;

#pragma endregion

#pragma region ------------------------------------------------ Global variables

// PS2 variables

PS2X ps2x;
uint8_t ps2xResult = 0;

// Servo variables

Servo servo1;
int steeringAngle = 90;
uint8_t speed = 0;
PwmMotorDriver motor1;

// Sinusoidal lookup table
// https://www.daycounter.com/Calculators/Sine-Generator-Calculator2.phtml
// Parameters: 512, 256, 16, decimal
// Edited a bit, then inverted by ChatGPT

const int lookupTable[] = {
	0,0,0,0,0,0,0,0,0,1,1,1,1,1,2,2,
	2,2,3,3,3,4,4,5,5,6,6,6,7,7,8,9,
	9,10,10,11,12,12,13,14,14,15,16,17,17,18,19,20,
	21,22,22,23,24,25,26,27,28,29,30,31,32,33,34,35,
	36,37,39,40,41,42,43,44,46,47,48,49,50,52,53,54,
	56,57,58,60,61,62,64,65,66,68,69,70,72,73,75,76,
	78,79,80,82,83,85,86,88,89,91,92,94,95,97,98,100,
	101,103,105,106,108,109,111,112,114,115,117,119,120,122,123,125,
	126,128,130,131,133,134,136,137,139,141,142,144,145,147,148,150,
	151,153,155,156,158,159,161,162,164,165,167,168,170,171,173,174,
	176,177,178,180,181,183,184,186,187,188,190,191,192,194,195,196,
	198,199,200,202,203,204,206,207,208,209,210,212,213,214,215,216,
	217,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,
	234,234,235,236,237,238,239,239,240,241,242,242,243,244,244,245,
	246,246,247,247,248,249,249,250,250,250,251,251,252,252,253,253,
	253,254,254,254,254,255,255,255,255,255,255,255,255,255,255,255
};

#pragma endregion

#pragma region ----------------------------------------------------------- Setup

void setup()
{
	Serial.begin(BAUD_RATE);

	// Motors, servos

	motor1.Init(motor1A, motor1B);
	servo1.attach(servoPin1);
	servo1.write(90);

	// PS2 controller

	ps2xResult = ps2x.config_gamepad(clock, command, attention, data);

	if(ps2xResult == 0) {
		Serial.println("PS2 receiver OK");
	} else if(ps2xResult == 1) {
		Serial.println("No PS2 receiver found.");
	} else if(ps2xResult == 2) {
		Serial.println("Found PS2 receiver, commands not accepted.");
	} else if(ps2xResult == 3) {
		Serial.println("Found PS2 receiver, pressures mode not accepted.");
	}

	if(ps2x.readType() == 1) {
		Serial.println("DualShock controller found.");
	}

	motor1.Coast();
}

#pragma endregion

#pragma region ------------------------------------------------------- Functions

/**
 * Motor speed and direction control.
 */
void setMotorSpeed()
{
	byte reading = ps2x.Analog(PSS_LY);

	if(reading < 128 - SPEED_DEAD_ZONE) {

		// Analog button above central position: values are between 0 (fully up)
		// and 128 (central position). Moves forward

		uint8_t pos = map(reading, 128, 0, 0, MAX_FORWARD_SPEED);
		speed = lookupTable[pos];
		motor1.RotateCCW(speed);
		// Serial.print("CCW: pos "); Serial.print(pos);
		// Serial.print(", speed "); Serial.println(speed);
		
	} else if(reading > 128 + SPEED_DEAD_ZONE) {

		// Analog button below central position: Values are between 128 (central
		// position) and 255 (fully down). Moves backwards

		uint8_t pos = map(reading, 128, 255, 0, MAX_BACKWARDS_SPEED);
		speed = lookupTable[pos];
		motor1.RotateCW(speed);
		// Serial.print("CW: pos "); Serial.print(pos);
		// Serial.print(", speed "); Serial.println(speed);

	} else {

		// Analog button near central position: Motors se to idle

		motor1.Coast();

	}
}

/**
 * Servo steering steeringAngle control.
 */
void setSteeringAngle()
{
	byte reading = ps2x.Analog(PSS_RX);
	steeringAngle = map(reading, 0, 255, SERVO_MIN, SERVO_MAX);
	servo1.write(steeringAngle);
	// Serial.print("Angle: "); Serial.println(steeringAngle);
}

#pragma endregion

#pragma region ------------------------------------------------------- Main loop

/**
 * Main loop
 */
void loop()
{
	if(ps2xResult != 1) {
		ps2x.read_gamepad();
		setMotorSpeed();
		setSteeringAngle();
		delay(LOOP_DELAY);
	}
}

#pragma endregion
