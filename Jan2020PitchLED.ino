#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_L3GD20_U.h>
#include <Adafruit_10DOF.h>
#include <FastLED.h>
#define NUM_LEDS 5
#define DATA_PIN 6


 /* 
   
Please also make sure to reference the Google Doc for additional documentation
This code will currently only be using the X axis data to inform the LED system. 
Please orient the chip in the sub such that the X's big arrow is pointed towards the head of the sub.

*/ 


/* Assign a unique ID to the sensors */
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(30301);
Adafruit_LSM303_Mag_Unified   mag   = Adafruit_LSM303_Mag_Unified(30302);
Adafruit_BMP085_Unified       bmp   = Adafruit_BMP085_Unified(18001);
Adafruit_L3GD20_Unified       gyro  = Adafruit_L3GD20_Unified(20);
/*Does something with alloting memory. I don't know, but you probably don't need to touch or reference this*/
CRGB leds[NUM_LEDS];

/*    Below are simple functions which make the LED strip return different colors/patterns. 
      The first 2 are basically just test functions.
      The rest are skeleton functions for the things we actually want to display.*/
void makeLedRed(){
      leds[0] = CRGB::Red; 
      FastLED.show();
      Serial.println("Red");      
}
void makeLedBlue(){
      leds[0] = CRGB::Blue;
      FastLED.show();
      Serial.println("Blue");
}
void solidGreen(){

}
void blinkingRed(){
// Include a 'blink intensity' function based on the magnitude of the X rotation value (tracked by xSum)  
}
void blinkingPurple(){
  
}
void blinkIntensity(){
  
}

void setup(void)
{
  Serial.begin(115200);
  Serial.println(F("Adafruit 10DOF Tester")); Serial.println("");
  if(!gyro.begin()) //Diagnostic test to ensure the gyroscope is working
  {
    /* There was a problem detecting the L3GD20 ... check your connections */
    Serial.print("Ooops, no L3GD20 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }  
  /* Display some basic information on this sensor */
  displaySensorDetails();
  /*initializes the LED strip*/
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
}

void loop(void)
{
  double xSum = 0.0; // Initialize the X value for rotation
        /* Get a new sensor event */
        sensors_event_t event;
        gyro.getEvent(&event);
          Serial.print(F("GYRO  "));
          Serial.print("X: "); Serial.print(event.gyro.x); Serial.print("  ");Serial.println("rad/s ");
        /* Increment the X rotational position value*/ 
        xSum = xSum + event.gyro.x; 
          Serial.println("Current X: " ); Serial.print(xSum);
    /*
       Conditional Statements which return the desired LED look based on the given X value
       TBD
    */
  delay(250);// Set the refresh rate- lower values of time will yield higher precision- up to a point. The current low value is for testin purposes

}



/*
Stuff we won't really need, but that was in the original starter code which I left:
*/

void displaySensorDetails(void)
{
  sensor_t sensor;

  gyro.getSensor(&sensor);
  Serial.println(F("------------- GYROSCOPE -----------"));
  Serial.print  (F("Sensor:       ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:   ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:    ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:    ")); Serial.print(sensor.max_value); Serial.println(F(" rad/s"));
  Serial.print  (F("Min Value:    ")); Serial.print(sensor.min_value); Serial.println(F(" rad/s"));
  Serial.print  (F("Resolution:   ")); Serial.print(sensor.resolution); Serial.println(F(" rad/s"));
  Serial.println(F("------------------------------------"));
  Serial.println(F(""));
  delay(500);
}
