/**********************************************

Logger.h

Data logger library header

Andy Wickert, started 27 September 2011

Designed to greatly simplify Arduino sketches 
for my data logger and reduce what the end 
user needs to do into relatively simple 
one-line calls.


# LICENSE: GNU GPL v3

Logger.h is part of Logger, an Arduino library written by Andrew D. Wickert.
Copyright (C) 2011-2013, Andrew D. Wickert

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

**********************************************/

/////////////////////////////////
// DEFINE SELF / INCLUDE GUARD //
/////////////////////////////////

#ifndef Logger_h
#define Logger_h

///////////////////////
// INCLUDE LIBRARIES //
///////////////////////

// Due to the release of Arduino 1.0, I need to check the version and 
// import libraries based on that.
//#if ARDUINO < 100
//#include <WProgram.h>
//#include <NewSoftSerial.h> // in case Decagon loggers are used / Digital IO is needed -- becomes Software Serial in Arduino 1.0 and greater
//#else
#include <Arduino.h>
#include <SoftwareSerial.h>
//#endif

// Standard libraries
#include <SdFat.h>
#include <Wire.h>
#include <DS3231.h>
#include <math.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <stdlib.h> // For turning incoming ASCII character
                    // strings into int with atol

// Sensor-centric libraries
//#include <Adafruit_Sensor.h>
//#include <Adafruit_BMP085_U.h>
//#include <eRCaGuy_NewAnalogRead.h>

// Outside of class definitions
void wakeUpNow();
void wakeUpNow_tip();
void _ISR_void();
void _anemometer_count_increment();

// The rest of the library

class Logger {

  public:
    // Constructor
    Logger();
    
    // Initialization
    void initialize(char* _logger_name, char* _sitecode, int _dayInterval, \
                    int _hourInterval, int _minInterval, int _secInterval, \
                    bool _ext_int=false, bool _LOG_ON_BUCKET_TIP=false);
    void setupLogger();
    
    // Code for sleeping, starting up from sleep, synching SD card
    void sleep();
    void startLogging();
    void endLogging();
    void startAnalog();
    void endAnalog();
    
    // Getters and setters
    bool get_use_sleep_mode();
    
    // Sensors - standard procedure (wake up, log, sleep)
    float readPin(int pin);
    float readPinOversample(int pin, int bits);
    float analogReadOversample(int pin, int adc_bits=10, int nsamples=1, \
                               debug=false);
    //debug tool delete when finished
    float analogReadOversample_Debug(int pin, int adc_bits=10, int nsamples=1);
    float thermistorB(float R0, float B, float Rref, float T0degC, \
                      int thermPin, bool Rref_on_GND_side=true);
    //debug tool delete when finished
    float thermistorB_Debug(float R0, float B, float Rref, float T0degC, \
                            int thermPin, bool Rref_on_GND_side=true);
    // Print order: Distance [cm], standard deviation [cm]
    void ultrasonicMB_analog_1cm(int nping, int EX, int sonicPin, \
                                 bool writeAll);
    float maxbotixHRXL_WR_Serial(int Ex, int Rx, int nping, bool writeAll, \
                                 int maxRange, bool RS232=false);
    // Print order: Distance [cm], standard deviation [cm]
    void maxbotixHRXL_WR_analog(int nping=10, int sonicPin=A0, int EX=99, \
                                bool writeAll=true);
    // Print order: Dielectric permittivity [-unitless-], 
    // Electrical Conductivity [dS/m], Temperature [degrees C]
    //void decagon5TE(int excitPin, int dataPin);
    void maxbotixHRXL_WR_analog_oneping(int sonicPin);
    void DecagonGS1(int pin, float Vref);
    void vdivR(int pin, float Rref, bool Rref_on_GND_side=true);
    //debug tool delete when finished
    void vdivR_Debug(int pin, float Rref, bool Rref_on_GND_side=true);
    void flex(int flexPin, float Rref, float calib1, float calib2);
    void linearPotentiometer(int linpotPin, float Rref, float slope, \
                             float intercept);
    void AtlasScientific(char* command, int softSerRX=6, int softSerTX=7, \
                         uint32_t baudRate=38400, bool printReturn=true, \
                         bool saveReturn=true);
    void HTM2500LF_humidity_temperature(int humidPin, int thermPin, \
                                        float Rref_therm, \
                                        float ADC_resolution_nbits=14);
    void HM1500LF_humidity_with_external_temperature(int humidPin, float Vref, \
                                        float R0, float B, float Rref, \
                                        float T0degC, int thermPin,
                                        float ADC_resolution_nbits=14);
    void Inclinometer_SCA100T_D02_analog_Tcorr(int xPin, int yPin, \
                                               float V_ADC, float VDD, \
                                               float R0, float B, float Rref, \
                                               float T0degC, int thermPin,
                                               float ADC_resolution_nbits=14);
    void Anemometer_reed_switch(int interrupt_number, 
                                unsigned long reading_duration_milliseconds, \
                                float meters_per_second_per_rotation);
    void Wind_Vane_Inspeed(int vanePin);
    void Pyranometer(int analogPin, float raw_mV_per_W_per_m2, \
                     float gain, float V_ref);
    //void Barometer_BMP180();
    
    // Sensors - triggered
    // Camera on/off function; decision made in end-user Arduino script
    void HackHD(int control_pin, bool want_camera_on);
    // Rain gage - will wake logger up and cause it to log to a different file
    // The default interrupt is Pin 3, INT1 on ATmega328 (ALog BottleLogger)
    void TippingBucketRainGage();
    float Honeywell_HSC_analog(float Vsupply, float Pmin, float Pmax, \
                               int TransferFunction, int units, int pin);

  private:
    void pinUnavailable(int pin);
    char *nameFile(char * _sitecode);

    // Sleep and alarms
    void sleepNow();
    void sleepNow_nap();
    // wakeUpNow defined outside of class; see above
    void alarm(int dayInterval ,int hourInterval, int minInterval, \
               int secInterval);
    void displayAlarms(); //debug tool delete if desired.
    void checkAlarms();  //debug tool delete if desired. 
    void checkTime();   //debug tool delete if desired. 
    // LED signals
    void LEDwarn(int nflash);
    void LEDgood();
    void LEDtimeWrong(int ncycles);
    
    // Time
    void unixDatestamp();
    
    // Logger-computer communications
    void name();
    void print_time();
    void set_time_main();
    void announce_start();
    void startup_sequence();
    
    // Clock power
    void RTCon();
    void RTCsleep();
    
    // SD card power
    void SDpowerOn();
    void SDpowerOff();
    
    // Clock setting
    void clockSet();
    void GetDateStuff(byte& Year, byte& Month, byte& Day, byte& DoW, 
		byte& Hour, byte& Minute, byte& Second);
    
    // Sensor utility codes
    float _vdivR(int pin, float Rref, int adc_bits=10, \
                 bool Rref_on_GND_side=true);
    //Debug tool delete when finished
    float _vdivR_Debug(int pin, float Rref, int adc_bits=10, \
                       bool Rref_on_GND_side=true);
    int maxbotix_soft_Serial_parse(int Ex, int Rx, bool RS232=false);
    int maxbotix_Serial_parse(int Ex);
    float standard_deviation_from_array(float values[], int nvalues, \
                                        float mean);
    float standard_deviation_from_array(int values[], int nvalues, float mean);

    
    // Logging
    void start_logging_to_otherfile(char* filename);
    void end_logging_to_otherfile();
    void endLine();

};

#endif  

