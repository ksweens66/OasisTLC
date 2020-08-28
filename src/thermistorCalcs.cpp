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

double selected_PID_input(double temperature_readings[])
{
  double average = 0;
  double sum = 0;
  double sdh = 0; //standard deviation 
  double threesd = 0;
  double abserrori = 0;
  double newsetpoint = 0;
  std::vector<double> vertices;
  
  for (int i = 0; i < 9; i++) //gets all 9 thermistor values
  {
    sum = sum + temperature_readings[i];
  }
  average = sum / 9; //calculates the average

  ///calculating to 3 standard deviations ///
  for (int i = 0; i < 9; i++)
  {
    sdh += pow(temperature_readings[i] - average, 2);
  }
  threesd = sqrt(sdh / 9);
  //////
        
  for (int i = 0; i < 9; i++)
  {
    abserrori = abs(temperature_readings[i] - average);
    if (abserrori > threesd) //reject sensor data for values greater than 3 sd
    { 
        continue;
    }
    else
    {
    vertices.push_back({temperature_readings[i]}); //add values within 3sd to the new, dynamic array 
    }
  }

  newsetpoint = (accumulate(vertices.begin(),vertices.end(),0))/vertices.size();
  vertices.clear();

  return newsetpoint;
}
