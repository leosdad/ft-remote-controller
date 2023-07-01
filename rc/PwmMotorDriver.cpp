/**
 * SoftPWM motor driver for H-bridge modules like the DRV8833.
 * Rubem Pechansky
 */

#include "PwmMotorDriver.h"
#include <SoftPWM.h>

// --------------------------------------------------------------------- Defines

#define WITHIN100(v) max(0, min(100, v))

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
	SoftPWMSetPercent(motorOutB, 100);
	SoftPWMSetPercent(motorOutA, 0);
}

void PwmMotorDriver::FullRotateCW()
{
	SoftPWMSetPercent(motorOutA, 100);
	SoftPWMSetPercent(motorOutB, 0);
}

// Rotate with PWM

void PwmMotorDriver::RotateCCW(const uint8_t speed)
{
	SoftPWMSetPercent(motorOutB, WITHIN100(speed));
	SoftPWMSetPercent(motorOutA, 0);
}

void PwmMotorDriver::RotateCW(const uint8_t speed)
{
	SoftPWMSetPercent(motorOutA, WITHIN100(speed));
	SoftPWMSetPercent(motorOutB, 0);
}

void PwmMotorDriver::Coast()
{
	SoftPWMSetPercent(motorOutA, 0);
	SoftPWMSetPercent(motorOutB, 0);
}

void PwmMotorDriver::Brake()
{
	SoftPWMSetPercent(motorOutA, 100);
	SoftPWMSetPercent(motorOutB, 100);
}
