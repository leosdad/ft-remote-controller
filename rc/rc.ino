
#pragma region -------------------------------------------------------- Includes

#include <PS2X_lib.h>
#include "PwmMotorDriver.h"
#include <Servo.h>

#pragma endregion

#pragma region --------------------------------------------------------- Defines

#define BAUD_RATE 57600
#define LOOP_DELAY 50

// Servo constansts

#define SPEED_DEAD_ZONE 2
#define ANGLE_DEAD_ZONE 2
#define SERVO_MIN 55
#define SERVO_MAX 125

#pragma endregion

#pragma region ------------------------------------------------------- Constants

// Arduino pins

const uint8_t motor1A = 2;
const uint8_t motor1B = 4;
const uint8_t motor2A = 5;
const uint8_t motor2B = 6;
const uint8_t motor3A = 7;
const uint8_t motor3B = 8;

const uint8_t servoPin1 = 3;
const uint8_t servoPin2 = 9;

const uint8_t attention = 10;	// orange
const uint8_t command = 11;		// green
const uint8_t data = 12;		// blue
const uint8_t clock = 13;		// gray

const uint8_t motor4A = A0;
const uint8_t motor4B = A1;
const uint8_t dip1_2 = A2;
const uint8_t dip3_4 = A3;

const uint8_t analog1 = A6;
const uint8_t analog2 = A7;

#pragma endregion

#pragma region ------------------------------------------------------- Variables

// PS/2 variables

PS2X ps2x;
uint8_t ps2xResult = 0;
bool vibrate = false;

// Servo variables

uint8_t reading1 = 0;
uint8_t speed1 = 0;
int angle1 = 90;
int servoStep = 3;
Servo servo1;

// Motor variables

PwmMotorDriver motor1;// = PwmMotorDriver();

#pragma endregion

#pragma region ----------------------------------------------------------- Setup

void setup()
{
	Serial.begin(BAUD_RATE);

	// Motors

	motor1.Init(motor1A, motor1B);

	// Servos

	servo1.attach(servoPin1);
	servo1.write(90);

	// PS/2 controller

	ps2xResult = ps2x.config_gamepad(clock, command, attention, data, vibrate, vibrate);

	if(ps2xResult == 0) {
		Serial.println("PS/2 receiver OK");
	} else if(ps2xResult == 1) {
		Serial.println("No PS/2 receiver found.");
	} else if(ps2xResult == 2) {
		Serial.println("Found PS/2 receiver, commands not accepted.");
	} else if(ps2xResult == 3) {
		Serial.println("Found PS/2 receiver, pressures mode not accepted.");
	}

	if(ps2x.readType() == 1) {
		Serial.println("DualShock controller found.");
	}

	motor1.Coast();
}

#pragma endregion

#pragma region ------------------------------------------------------- Main loop

// TODO: só precisa passar valores quando eles mudarem

void loop()
{
	// Skip loop if no controller found

	if(ps2xResult == 1) {
		return;
	}

	ps2x.read_gamepad();

	// Motor speed and direction control

	reading1 = ps2x.Analog(PSS_LY);

	if(reading1 > 128 + SPEED_DEAD_ZONE) {
		speed1 = map(reading1, 255, 128, 100, 0);
		Serial.print("CW ");
		Serial.println(speed1);
		motor1.RotateCW(speed1);
	} else if(reading1 < 128 - SPEED_DEAD_ZONE) {
		speed1 = map(reading1, 0, 128, 100, 0);
		Serial.print("CCW ");
		Serial.println(speed1);
		motor1.RotateCCW(speed1);
	} else {
		motor1.Coast();
	}

	// Servo steering angle control

	// steerAngle = map(ps2x.Analog(PSS_RX), 0, 255, SERVO_MIN, SERVO_MAX);
	if(ps2x.Button(PSB_TRIANGLE)) {
		angle1 = 90;
	} else if(ps2x.Button(PSB_CIRCLE)) {
		angle1 = min(SERVO_MAX, max(SERVO_MIN, angle1 + servoStep));
	} else if(ps2x.Button(PSB_SQUARE)) {
		angle1 = min(SERVO_MAX, max(SERVO_MIN, angle1 - servoStep));
	}

	servo1.write(angle1);

	// if(ps2x.Button(PSB_R1)) {
	// 	// brake
	// }

	delay(LOOP_DELAY);
}

#pragma endregion
