#include "thermistorCalcs.h"
#include "Constants.h"
#include <math.h>
#include <string.h>
#include <algorithm>

using namespace std; //need this to use the accumulate function

double calcTemp(double analog_value) {
        double thermistor_resistance, thermistor_temperature;
        thermistor_resistance = (analog_value * resistor) / (analogMax - analog_value);                       // calculates resistance of thermistor1
        thermistor_temperature = 1 / (a + b * log(thermistor_resistance) + c * pow(log(thermistor_resistance), 3));
        return thermistor_temperature;       
}

double selected_PID_input(double temperature_readings[]) 
  //takes in a static array of 9 temperature values and returns a double setpoint
 // remember to download the libraries #include <ArduinoSTL.h> and #include <numeric> 
{
  double average = 0;
  double sum = 0;
  double sdh = 0;
  double twosd = 0;
  double abserrori = 0;
  double newsetpoint = 0;
  double stackcounter = 0;
  double setpointsum = 0;

  for (int i = 0; i < 9; i++) //gets all 9 thermistor values
  {
    sum += temperature_readings[i];
  }
  average = sum / 9; //calculates the average

  for (int i = 0; i < 9; i++)
  {
    sdh += pow((temperature_readings[i] - average), 2);
  }
  twosd = 2 * sqrt(sdh / 9);

  for (int i = 0; i < 9; i++)
  {
    abserrori = abs(temperature_readings[i] - average);
    if (abserrori < twosd) //reject sensor data for values greater than 2 sd
    {
      setpointsum = setpointsum + temperature_readings[i]; //add values within 2sd to dynamic array
      stackcounter = stackcounter + 1;
    }
  }
  newsetpoint = setpointsum/stackcounter;
  return newsetpoint;
}
        
        
        
}
