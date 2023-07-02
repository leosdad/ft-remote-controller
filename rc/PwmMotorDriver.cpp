/**
 * SoftPWM motor driver for H-bridge modules like the DRV8833.
 * Rubem Pechansky
 */

#include "PwmMotorDriver.h"
#include <SoftPWM.h>

// ------------------------------------------------------------- Private methods

void PwmMotorDriver::initMotor(const uint8_t motorPorts[2])
{
	SoftPWMBegin(reversePolarity ? SOFTPWM_INVERTED : SOFTPWM_NORMAL);

	motorOutA = motorPorts[0];
	motorOutB = motorPorts[1];
	SoftPWMSet(motorOutA, 0);
	SoftPWMSet(motorOutB, 0);
}

// ----------------------------------------------------------------- Constructor

PwmMotorDriver::PwmMotorDriver(bool reversePolarity = false)
{
	reversePolarity = reversePolarity ? SOFTPWM_INVERTED : SOFTPWM_NORMAL;
	SoftPWMBegin(reversePolarity);
}

// -------------------------------------------------------------- Public methods

void PwmMotorDriver::Init(const uint8_t motorPorts[2])
{
	initMotor(motorPorts);
}

void PwmMotorDriver::Init(const uint8_t motorPortA, const uint8_t motorPortB)
{
	const uint8_t ports[2] = {motorPortA, motorPortB};
	initMotor(ports);
}

// Rotate at full speed

void PwmMotorDriver::FullRotateCCW()
{
	SoftPWMSet(motorOutB, 255);
	SoftPWMSet(motorOutA, 0);
}

void PwmMotorDriver::FullRotateCW()
{
	SoftPWMSet(motorOutA, 255);
	SoftPWMSet(motorOutB, 0);
}

// Rotate with PWM

void PwmMotorDriver::RotateCCW(const uint8_t speed)
{
	SoftPWMSet(motorOutB, speed);
	SoftPWMSet(motorOutA, 0);
}

void PwmMotorDriver::RotateCW(const uint8_t speed)
{
	SoftPWMSet(motorOutA, speed);
	SoftPWMSet(motorOutB, 0);
}

void PwmMotorDriver::Coast()
{
	SoftPWMSet(motorOutA, 0);
	SoftPWMSet(motorOutB, 0);
}

void PwmMotorDriver::Brake()
{
	SoftPWMSet(motorOutA, 255);
	SoftPWMSet(motorOutB, 255);
}
