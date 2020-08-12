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

double selected_PID_input(double calculated_temperatures[thermistor_count]){ //inputting in an array 
        //selected_PID_input(double calculated_temperatures[])
        
//     double sorted_temperatures[thermistor_count];
//     memcpy(sorted_temperatures,calculated_temperatures,sizeof(double)* thermistor_count);
//     std::sort(sorted_temperatures,sorted_temperatures + thermistor_count);
//     return sorted_temperatures[middle_index];
        //Schiano's Standard Deviation Method
        double sorted_temperatures[thermistor_count];
        double tcerror, sum, avg, threesd;
        int count; 
        memcpy(sorted_temperatures,calculated_temperatures,sizeof(double)* thermistor_count); //copying into a new array
        count = sizeof(sorted_temperatures)/sizeof(sorted_temperatures[0]); //since all elements have same byte size
        
        for (int i = 0; i < count; i++)
        {
        sum = sorted_temperatures[i];
        }
        
        avg = sum/count;
        for (int i = 0; i < count; i++)
        {
        threesd = 3*sqrt((1/count) * pow(sorted_temperatures[i] - avg, 2));
        tcerror = abs(sum - sorted_temperatures[i]);
                if (tcerror > thresd)
                {
                 ///remove ith term from sorted temperatures       
                }
        }        
        return avg;
        //1. First find the mean of the sensors
        //2. Find the error between each read sensor and the mean
        //
        //3. use the definition of standard deviation
        //4. if abs(k) > 3*std; reject error of (k)
        //5. return the average
}


// double getAverage(int arr[], int size);

// int main () {
//    // an int array with 5 elements.
//    int balance[5] = {1000, 2, 3, 17, 50};
//    double avg;

//    // pass pointer to the array as an argument.
//    avg = getAverage( balance, 5 ) ;
 
//    // output the returned value 
//    cout << "Average value is: " << avg << endl; 
    
//    return 0;
}
