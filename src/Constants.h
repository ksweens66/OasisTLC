#ifndef TLC_CONSTANTS_H
#define TLC_CONSTANTS_H
//Filename: Constants.h
//Purpose : Global constants for calculating temperatures and setting pin definitions 


const int       LEDPIN = 13;                          // used for heartbeat
const int       frequency = 93750;
const int       resistor = 10000;                     // ohms of the voltage divider resistor
const float     roomTemp = 298.15;                    // roomTemp in Kelvin
const float     roomResistance = 20000;               // resistance at roomTemp
const int       beta = 4300;                          // beta value of thermistor
const double    a = 0.001101;                         // taken using dataset and matlab
const double    b = 0.0002388;                        // taken using dataset and matlab
const double    c = 6.815e-8;                         // taken using dataset and matlab
const int       baudRate = 115200;                    // baud rate
const int       analogBitCount = 14;                  // analog bit count
const int       analogMax = (1 << analogBitCount) - 1;// maximum analog value
const int       heater_1_Pin = 5;                     // Heater 1 PWM pin
const int       heater_2_Pin = 6;                     // Heater 2 PWM pin
const int       heater_3_Pin = 9;                     // Heater 3 PWM pin
const int       thermistor_count = 3;                 // number of heaters  
const int       middle_index = 1;                     // 0...8 is 4. The index of the middle of the array of temps. Ex. for 3 thermistors we have 0,1,2. Middle is 1.

const int TS = 100;                                   // Time sample
const float KP = .25;                                 // Proportional gain
const float KI = .05;                                 // Integral gain
const float TF_SP = 295.5;                            // Target temperature


 #endif