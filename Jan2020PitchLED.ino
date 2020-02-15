#include <FastLED.h> // Ensure you download version 3.3.0, not any higher versions
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_L3GD20_U.h>
#include <Adafruit_10DOF.h>
#define NUM_LEDS 5
#define DATA_PIN 6


 /* 
   
Please also make sure to reference the Google Doc for additional documentation:

https://docs.google.com/document/d/1MR0bQWZMU0a5Lc630uQSLEU7wz0lCnDvpQG80wKguqQ/edit


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

/*    
 *     Below are simple functions which make the LED strip return different colors/patterns. 
 */

void solidGreen(){
      leds[0] = CRGB::Green;
      leds[1] = CRGB::Green; 
      leds[2] = CRGB::Green; 
      leds[3] = CRGB::Green; 
      leds[4] = CRGB::Green;  
      FastLED.show();
  //    Serial.println("Green");
}
void makeAllRed(){
      leds[0] = CRGB::Red;
      leds[1] = CRGB::Red; 
      leds[2] = CRGB::Red; 
      leds[3] = CRGB::Red; 
      leds[4] = CRGB::Red;
      FastLED.show();
}
void makeAllDark(){
      leds[0] = CRGB::Black;
      leds[1] = CRGB::Black; 
      leds[2] = CRGB::Black; 
      leds[3] = CRGB::Black; 
      leds[4] = CRGB::Black;
      FastLED.show();
}

/*The following functions are slightly more complicated so I will explain them more*/


/* Function to make lights turn on, and change color based on xSum
 * UNFINISHED*/
void redToBlue(){
  // 
}

/* Function to make a certain number of lights turn on. 
 * Calls redToBlue() 
 * UNFINISHED*/
void incrementingLights(int xRotation){

}

/*
 * Function to make LEDs blink. It calls the redToBlue() function above, which 
 * makes LEDs turn on and to a specific color based on the value of xSum
 * Declare some dummy variables for the blinking lights.*/
long previousMillis = 0;  
double xSum = 0.0; // Initialize the X value for rotation. The units for this is radians.
double interval = 1000+(xSum); /*interval is based on xSum; higher values of xSum should result in a smaller interval*/
int toggle = 1;
void blinkingLED(int xRotation){
  long currentMillis = millis();
  if(currentMillis - previousMillis > interval) {
    /* save the last time you blinked the LED */
    previousMillis = currentMillis;
    Serial.println("If statement launched");
      /*Toggle the 'toggle' variable*/
      Serial.println(toggle);
      if (toggle == 1) {
        toggle = 0;
      }
      else{
        toggle = 1;  
      }
  }
    /*Turn LEDs on when toggle = 1; 
      The toggle exists to make the LED blinking more consistent*/
    if(toggle == 0){
      //redBlueShift(xSum); //Function has not yet been written
      makeAllRed(); //Placeholder until better function is written
    }
    else{
      makeAllDark();
    }
}

void setup(void)
{
  Serial.begin(115200);
  Serial.println(F("Adafruit 10DOF Tester")); Serial.println("");
  if(!gyro.begin()) //Diagnostic test to ensure the gyroscope is working
  {
    /* There was a problem detecting the L3GD20 ... check your connections */
    Serial.print("Ooops, no L3GD20 (gryoscope) detected ... Check your wiring or I2C ADDR!");
    while(1);
  }  
  /* Display some basic information on this sensor */
  displaySensorDetails();
  /*initializes the LED strip*/
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
}


  
void loop(void)
{

        /* Get a new sensor event - the event.gyro.x call below returns an angular velocity*/
        sensors_event_t event;
        gyro.getEvent(&event);
        /*Add the angular velocity value to the current xSum value. If the intervals are small enough,,
        this should get us something close to our angular position- if you disagree, retake Calculus II*/
        xSum = xSum + event.gyro.x + .018; //The plus .018 is because there's a weird constant offset on the gyro. This keeps it about constant. 
        Serial.println(xSum); //To check our xSum value
    /*
       Conditional Statements which return the desired LED look based on the given X value
    */
        if (xSum <= 15 && xSum >= -15) { // Alter the radians as needed.
          solidGreen();
        }
        else{
          blinkingLED(xSum);
        }

  /*
  
    We can either write modular code to make programming the LEDs more convenient,
    or we could just brute force specify each case we want. I will be writing some code for the
    modular approach for right now.
  
  */
  delay(25);
  /*The delay() function call should be removed/reexamined before we finalize the chip. Since the program is literally just adding
  up the angular velocity readings everytime we loop, the loop intervals should be such that they support the
  precision of the chip.
  
  Removing the delay() function is likely to make the chip get wildly miscalibrated with small movements, but
  delays that are too long will reduce the sensitivity of the rotational position variable xSum.*/

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
