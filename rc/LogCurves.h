
#ifndef LOG_CURVES_H
#define LOG_CURVES_H

#include <Arduino.h>
#include <math.h>

/*

default            descending         easeIn             Both

+                  ++++++++++++++     +                  ++++++++++++++
++                 ++++++++           ++++++             +++++++++++++
++++               ++++               ++++++++++         ++++++++++
++++++++           ++                 +++++++++++++      ++++++
++++++++++++++     +                  ++++++++++++++     +

*/

class LogCurves
{
	public:

		/**
		 * Creates a logarithmic curve made of bytes. Values are generated from 0 to 255.
		 * 
		 * @param values Points to the buffer that will receive the curve values.
		 * @param steps The number of steps to generate.
		 * @param smoothness Lower values create steeper curves. Useful values are between 0.1 and 50.
		 * @param easeIn Curve starts slow and speeds up.
		 * @param descending Starts with higher values and ends with zero.
		 */
		void createLogCurve(uint8_t values[], const int steps, double smoothness, bool easeIn, bool descending);

		void showGraph(uint8_t values[], const int steps);

		void outputValues(uint8_t values[], const int steps, boolean hex = false);
};

#endif	// LOG_CURVES_H
