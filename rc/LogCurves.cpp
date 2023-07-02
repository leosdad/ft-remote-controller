
#include "LogCurves.h"


/**
 * Creates a logarithmic curve made of bytes.
 */
void LogCurves::createLogCurve(uint8_t values[], const int steps, double smoothness, bool easeIn, bool descending)
{
	double gap = (log(smoothness + 255) - log(smoothness)) / steps;
	double val0 = smoothness;
	double val1;
	uint8_t result;

	for(int i = 0; i < steps; i++) {
		val1 = val0 * exp(gap);
		val0 = val1;
		result = (uint8_t)((round(val1)) - smoothness);

		if(easeIn) {
			if(descending) {
				values[i] = 255 - result;
			} else {
				values[steps - i - 1] = 255 - result;
			}
		} else {
			if(descending) {
				values[steps - i - 1] = result;
			} else {
				values[i] = result;
			}
		}
	}
}

void LogCurves::showGraph(uint8_t values[], const int steps)
{
	for(int i = 0; i < steps; i++) {
		Serial.print(i);
		Serial.print(" ");
		Serial.print(values[i]);
		Serial.print(" ");
		for(int x = 0; x < values[i] / 3; x++) { Serial.print('*'); }
		Serial.println();
	}
}

void LogCurves::outputValues(uint8_t buffer[], const int steps, bool hex)
{
	for(int i = 0; i < steps; i++) {
		if(hex) {
			Serial.print("0x");
			if(buffer[i] < 0x10) {
				Serial.print('0');
			}
			Serial.print(buffer[i], HEX);
		} else {
			Serial.print(buffer[i], DEC);
		}
		Serial.print(", ");

		if((i + 1) % 16 == 0) {
			Serial.println();
		}
	}
}
