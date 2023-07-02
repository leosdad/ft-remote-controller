
#pragma region -------------------------------------------------------- Includes

#include <PS2X_lib.h>
#include <Servo.h>
#include "PwmMotorDriver.h"
#include "LogCurves.h"

#pragma endregion

#pragma region --------------------------------------------------------- Defines

#define BAUD_RATE 57600

// General constansts

#define LOOP_DELAY 50

// Speed constansts

#define SPEED_DEAD_ZONE 2
#define MAX_FORWARD_SPEED 255
#define MAX_BACKWARDS_SPEED 200

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

// Motor and servo variables

PwmMotorDriver motor1;
Servo servo1;
int steeringAngle = 90;
uint8_t speed = 0;

// createLogCurve() with 256, 30, false, false
uint8_t speedLookupTable[256] = {
	0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 
	5, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 
	10, 10, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 15, 15, 15, 16, 
	16, 17, 17, 17, 18, 18, 19, 19, 20, 20, 20, 21, 21, 22, 22, 23, 
	23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 31, 
	31, 32, 32, 33, 33, 34, 34, 35, 36, 36, 37, 37, 38, 39, 39, 40, 
	40, 41, 42, 42, 43, 44, 44, 45, 46, 46, 47, 48, 48, 49, 50, 50, 
	51, 52, 52, 53, 54, 55, 55, 56, 57, 58, 58, 59, 60, 61, 62, 62, 
	63, 64, 65, 66, 67, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 76, 
	77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 93, 
	94, 95, 96, 97, 98, 99, 100, 101, 103, 104, 105, 106, 107, 109, 110, 111, 
	112, 114, 115, 116, 117, 119, 120, 121, 123, 124, 125, 127, 128, 130, 131, 132, 
	134, 135, 137, 138, 140, 141, 143, 144, 146, 147, 149, 150, 152, 154, 155, 157, 
	159, 160, 162, 164, 165, 167, 169, 170, 172, 174, 176, 178, 179, 181, 183, 185, 
	187, 189, 191, 193, 195, 197, 199, 201, 203, 205, 207, 209, 211, 213, 215, 218, 
	220, 222, 224, 226, 229, 231, 233, 236, 238, 240, 243, 245, 248, 250, 253, 255, 
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

	// Generate logarithmic lookup table

	// LogCurves lc;
	// lc.createLogCurve(speedLookupTable, 256, 30, false, false);
	// lc.outputValues(speedLookupTable, 256);
	// // lc.showGraph(speedLookupTable, 256);
	// Serial.println("Log curve generated.");

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
		speed = speedLookupTable[pos];
		motor1.RotateCCW(speed);
		// Serial.print("CCW: pos "); Serial.print(pos);
		// Serial.print(", speed "); Serial.println(speed);
		
	} else if(reading > 128 + SPEED_DEAD_ZONE) {

		// Analog button below central position: Values are between 128 (central
		// position) and 255 (fully down). Moves backwards

		uint8_t pos = map(reading, 128, 255, 0, MAX_BACKWARDS_SPEED);
		speed = speedLookupTable[pos];
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
