/*
Teensy 3.2 Oasis Thermistor Code
Reads the analog value of the thermistor and calculates temperature
Sends temperature through PID Loop
Sends PID loop values to PWM code
Sends important data to Beaglebone
*/

#include <Arduino.h>
#include <kinetis.h>
#include <Constants.h>

double e = 0;                                         // Error from the target temperature, 0 at startup so not null
float  x = 0;                                          // integrater term, 0 at startup so not null
double u = 0;                                         // Duty cycle, 0 at startup so not null
int percent =   analogMax / 2;                        // initializes the percent to 50% of the maximum analog value
 
IntervalTimer wdTimer;                                // Initialize Interval Timer
            
void WatchDogReset();
void setup() {
    Serial.begin(baudRate);                       // change baud rate accordingly
    analogReadRes(analogBitCount);                // set analog reading bit count

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
    analogWriteFrequency(PWM_pin, frequency);      // Set PWM write Frequency
    analogWriteResolution(analogBitCount);         // Set bit resolution (For now, 14 bits until further updated)
    analogWrite(PWM_pin, percent);                 // Set PWM on and off percent refered to the bit resolution

    pinMode(LEDPIN, OUTPUT);
}

float calcTemp() {
    float totalTemp1 = 0;                         //  total temperature of thermistor1 reading per loop

    for (int i = 0; i < avgThermLoop; i++) {
        int analog1 = analogRead(A1);             // change pin accordingly, measures analog value of thermistor1
        Serial.print("Analog Value:          ");
        Serial.println(analog1);
        float thermistor1 = (analog1 * resistor) / (analogMax - analog1);                       // calculates resistance of thermistor1
        float temperature1 = 1 / (a + b * log(thermistor1) + c * pow(log(thermistor1), 3));       
        Serial.print("Resistance Calculated: ");
        Serial.println(thermistor1);
        
        totalTemp1 += temperature1;               // adds the new measured/calculated temperature to the total of each thermistor
    }
    // list of average temperatures for each sensor
    float avgTemp1 = totalTemp1 / avgThermLoop;

    Serial.print("Temperature (Celcius):  ");
    Serial.println(avgTemp1);
    //delay(1000);  // time before next reading
    return avgTemp1;
}

void WatchDogReset() {
    noInterrupts();
    WDOG_REFRESH = 0xA602;
    WDOG_REFRESH = 0xB480;
    interrupts();
}

void loop() {
    digitalWrite(LEDPIN, HIGH);                     // turns LED on
    delay (1000);                                   // used to slow LED blink 
    
    /* Realize PI Controller */
    e = TF_SP - calcTemp(); // error is the SET_POINT - ACTUAL (TF_PV)
    x = x + TS*e/1000;
    u = KP*e + KI*x;
    if (u >1)
        u =1;
    else if (u<0)
        u = 0;
    
    analogWrite(PWM_pin, u*analogMax);              // writes an pwm signal proportional to (duty cycle * u) to the PWM pin

    //Write
    WatchDogReset();
    digitalWrite(LEDPIN, LOW);
    delay(1000);
}