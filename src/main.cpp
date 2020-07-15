/*
Teensy 3.2 Oasis Thermistor Code
Reads the analog value of the thermistor and calculates temperature
Sends temperature through PID Loop
Sends PID loop values to PWM code
Sends important data to Beaglebone
*/

#include <Arduino.h>
#include <kinetis.h>
#include "Constants.h"
#include "thermistorCalcs.h"
#include <string>

#define BBB_comms Serial                              // Serial is USB. Serial1 is Pins 0 and 1. BBB_comms is a more descriptive name in the main code

double  error = 0;                                     // Error from the target temperature, 0 at startup so not null
double  integrator_term = 0;                           // integrater term, 0 at startup so not null
double  duty_cycle = 0;                                // Duty cycle, 0 at startup so not null
//int     percent =   analogMax / 2;                   // initializes the percent to 50% of the maximum analog value. Not sure I like this, so it is commented out. To add back in, do an analog write in the setup()
double  thermistor_reading;
double  temperature_readings[thermistor_count];

String temperature_string;
String mini_temperature;
String duty_cycle_str;
String mini_duty_cycle;

IntervalTimer wdTimer;                                // Initialize Interval Timer          
void WatchDogReset();


void setup() {
    BBB_comms.begin(baudRate);                       // change baud rate accordingly
    analogReadRes(analogBitCount);                // set analog reading bit count

    //WATCHDOG code
    noInterrupts();                               // interrupts must be disabled while setting up the watchdog timer
    WDOG_UNLOCK = WDOG_UNLOCK_SEQ1;               // Unlock both sequences for Watch dog
    WDOG_UNLOCK = WDOG_UNLOCK_SEQ2;
    delayMicroseconds(1);
    WDOG_STCTRLH = 0x0001;                        // Watchdog Status & Control Register High value
    WDOG_TOVALL = 0x1388;                         // Lower 16 bits  (Time-out value low)
    WDOG_TOVALH = 0;                              // Upper 16 bits  (Time-out value high)
    WDOG_PRESC = 0;                               // Prescaler clock set at 1kHz instead of 200Hz
    wdTimer.begin(WatchDogReset, 900000);         // begin watchdog timer
    interrupts();                                 // interrupts re-enabled
    
    //PWM Code
    analogWriteFrequency(heater_1_Pin, frequency); // Set PWM write Frequency
    analogWriteResolution(analogBitCount);         // Set bit resolution (For now, 14 bits until further updated)
    analogWriteFrequency(heater_2_Pin, frequency); // Set PWM write Frequency
    analogWriteFrequency(heater_3_Pin, frequency); // Set PWM write Frequency
    pinMode(LEDPIN, OUTPUT);
}

void WatchDogReset() {
    noInterrupts();
    WDOG_REFRESH = 0xA602;
    WDOG_REFRESH = 0xB480;
    interrupts();
}

void loop() {
    //digitalWrite(LEDPIN, HIGH);                                         // blips LED on                                   
    digitalWrite(LEDPIN, HIGH);

    // read analog pins in sequence, calculate the temperature for each and store into array
    for (int i = 0; i < thermistor_count; i++){
        thermistor_reading = analogRead(i);
        temperature_readings[i] = calcTemp(thermistor_reading);
    }
    /* Realize PI Controller */
    error = TF_SP - selected_PID_input(temperature_readings);           // error is the SET_POINT - ACTUAL (TF_PV)
    integrator_term = integrator_term + (TS*error)/1000;
    duty_cycle = KP*error + KI*integrator_term;
    if (duty_cycle > 1){                                                 // protection to not make duty cycle nonsensical
        duty_cycle =1;}
    else if (duty_cycle < 0){
        duty_cycle = 0;}
 
    /* Realize PI Tuning (Tyreus-Luyben) from Frequency Response */
    //Record ultimate gain and ultimate period: Ku and Pu  
    Kp = Ku/3.2; 
    KI = 2.2*Pu;
 
    //when steady state is reached, stop adding, but maybe not needed for our design to that level of percision
//     if (error == 0){
//         integrator_term = 0;} //makes sure that when we have reached SET_POINT stop adding heat
    
    //PI Integrator anti-windup Check Clamping Method (aka conditional integration)
    //goal is to stop integrating if beyond our limits  between our threshold and it breaking down 
    //defined saturation limits around spectrometer, with a 5 degree C buffer between absolute min and max capable temperatures
    

    if (saturationHigh < selected_PID_input(temperature_readings)) //check if current temperature is higher than value
    {
        if (integrator_term > 0) // check if integrator is still adding
        {
        integrator_term = 0;
        //clamps when reaching a "hot temperature" and error term is reset
        }
    }
    
    if (saturationLow < selected_PID_input(temperature_readings)) //check if current temperature is lower than value
    {
        if (integrator_term < 0) // check if integrator is still subtracting
        {
        integrator_term = 0;
        //clamps when reaching a "cold temperature" and error term  is reset
        }
    }
    else
       {
        integrator_term = integrator_term + (TS*error)/1000;
        //restores integrator when between an optimal range 
       }

    
    // write duty cycle to heater pins
    analogWrite(heater_1_Pin, duty_cycle*analogMax);                    // writes an pwm signal proportional to (analogMax * duty_cycle) to the PWM pin. Ex. 255*0.5 or 255*.1
    analogWrite(heater_2_Pin, duty_cycle*analogMax);     
    analogWrite(heater_3_Pin, duty_cycle*analogMax);     

    
    // report each temperatures to BBB
        BBB_comms.print('|');                                           // start each block of temps with a |
        for(int i = 0; i < thermistor_count; i++){
            if (i < thermistor_count - 1){
            //double temperature = temperature_readings[i];
            temperature_string = String(temperature_readings[i],DEC);   // sent double to string
            mini_temperature = temperature_string.substring(0,6);       // save 3 digits before decimal, decimal point, and 2 digits after
            BBB_comms.print(mini_temperature + ',');                    // comma separate the values
            }
            else {
                temperature_string = String(temperature_readings[thermistor_count-1],DEC);   // sent double to string
                mini_temperature = temperature_string.substring(0,6);
                BBB_comms.print(mini_temperature + ',');                // last value gets printed with a ; to mark end
                duty_cycle_str = String(duty_cycle, DEC);
                mini_duty_cycle = duty_cycle_str.substring(0,6);
                BBB_comms.println(mini_duty_cycle);   // print the duty cycle to make Normen happy
                }               
        }
        // for PID debugging. Prints the duty cycle.
        /*
        BBB_comms.println();
        double selected_temp = selected_PID_input(temperature_readings);
        String selected_PID_temp = String(selected_temp, DEC);
        BBB_comms.println("selected PID temp is: "+ selected_PID_temp);
        String duty_cycle_str = String(duty_cycle, DEC);
        BBB_comms.println("duty cycle is: "+ duty_cycle_str);
        String integrator_str = String(integrator_term, DEC);
        BBB_comms.println("integrator term is: "+ integrator_str);
        BBB_comms.println();
        */
    // tidying up
    WatchDogReset();
    digitalWrite(LEDPIN, LOW);
    delay(1000);                // used to slow the PI(D) update, also sets minimum period for LED blink
}
