#include "thermistorCalcs.h"
#include "Constants.h"
#include <math.h>
#include <string.h>
#include <algorithm>

double calcTemp(double analog_value) {
        double thermistor_resistance, thermistor_temperature;
        thermistor_resistance = (analog_value * resistor) / (analogMax - analog_value);                       // calculates resistance of thermistor1
        thermistor_temperature = 1 / (a + b * log(thermistor_resistance) + c * pow(log(thermistor_resistance), 3));
        return thermistor_temperature;       
}

double selected_PID_input(double calculated_temperatures[thermistor_count]){
//     double sorted_temperatures[thermistor_count];
//     memcpy(sorted_temperatures,calculated_temperatures,sizeof(double)* thermistor_count);
//     std::sort(sorted_temperatures,sorted_temperatures + thermistor_count);
//     return sorted_temperatures[middle_index];
        
        //Schiano's Standard Deviation Method
        //1. First find the mean of the sensors
        //2. Find the error between each sensor and the mean
        //
        //3. use the definition of standard deviation
        //std = (
        //4.
        //if abs(k) > 3*std; reject error of (k)
}
