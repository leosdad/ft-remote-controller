/**
 * SoftPWM motor driver for H-bridge modules like the DRV8833.
 * Rubem Pechansky
 */

#ifndef PWM_MOTOR_DRIVER_H
#define PWM_MOTOR_DRIVER_H

#include <Arduino.h>

/**
 * SoftPWM driver for one motor using DRV8833 module.
 */
class PwmMotorDriver
{
	private:

		uint8_t motorOutA, motorOutB;
		bool reversePolarity = false;
		void initMotor(const uint8_t motorPorts[2]);

	public:
		
		/**
		 * Constructor.
		*/
		PwmMotorDriver(bool reversePolarity = 0);

		void Init(const uint8_t motorPortA, const uint8_t motorPortB);
		void Init(const uint8_t motorPorts[2]);

		/**
		 * Rotate motor counterclockwise.
		*/
		void FullRotateCCW();
		void RotateCCW(const uint8_t speed);

		/**
		 * Rotate motor clockwise.
		*/
		void FullRotateCW();
		void RotateCW(const uint8_t speed);

		/**
		 * Let motor deacelerate naturally.
		*/
		void Coast();

		/**
		 * Forces motor to stop quickly.
		*/
		void Brake();
};

#endif	// PWM_MOTOR_DRIVER_H

// -----------------------------------------------------------------------------
