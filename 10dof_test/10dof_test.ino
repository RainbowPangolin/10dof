#include <Adafruit_LSM303.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_10DOF.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_L3GD20_U.h>
#include <Adafruit_BMP085_U.h>

// Make sure to download the above libraries, as well as any that pop up in the error messages. 
// You will get some 'Error Compiling' messages that show some other libraries are missing- 
// these are dependencies of some of these other libraries that don't need to be included in here.



static Adafruit_10DOF                dof   = Adafruit_10DOF();
static Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(30301);
static Adafruit_LSM303_Mag_Unified   mag   = Adafruit_LSM303_Mag_Unified(30302);
static Adafruit_BMP085_Unified       bmp   = Adafruit_BMP085_Unified(18001);

void setup() {
 
  // put your setup code here, to run once:

}

void loop() {
  
//Put starter code in here to test. The following are some examples taken from
// https://learn.adafruit.com/adafruit-10-dof-imu-breakout-lsm303-l3gd20-bmp180/software


// Example: Objects declared in the first two lines of the following code, which return readings from their respective sensors. 

//sensors_event_t accel_event;
//sensors_vec_t   orientation;
// 
///* Calculate pitch and roll from the raw accelerometer data */
//accel.getEvent(&accel_event);
//if (dof.accelGetOrientation(&accel_event, &orientation))
//{
//  /* 'orientation' should have valid .roll and .pitch fields */
//  Serial.print(F("Roll: "));
//  Serial.print(orientation.roll);
//  Serial.print(F("; "));
//  Serial.print(F("Pitch: "));
//  Serial.print(orientation.pitch);
//  Serial.print(F("; "));
//}

}
