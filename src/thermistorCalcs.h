#ifndef _THERMISTOR_CALCS_H
#define _THERMISTOR_CALCS_H
#include "Constants.h"
double calcTemp(double analog_value);
double selected_PID_input(double calculated_temperatures[thermistor_count]);
#endif