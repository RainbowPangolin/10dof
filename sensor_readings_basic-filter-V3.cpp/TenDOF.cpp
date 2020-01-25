/**
 * Holds all the interfacing for the 10DOF chip to make our main file more readable
 * 
 * TODO: add functionality for other measurements besides heading
 */
#include "TenDOF.h"

#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_L3GD20_U.h>
#include <Adafruit_10DOF.h>

//error codes
#define SUCCESS 0
#define ERR_NO_ACCEL 3
#define ERR_NO_MAG 20
#define ERR_NO_BMP 100
#define ERR_READ_FAILED 9999

/**************************************

   Declarations

 **************************************
*/

/* Assign a unique ID to the sensors */
static Adafruit_10DOF                dof   = Adafruit_10DOF();
static Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(30301);
static Adafruit_LSM303_Mag_Unified   mag   = Adafruit_LSM303_Mag_Unified(30302);
static Adafruit_BMP085_Unified       bmp   = Adafruit_BMP085_Unified(18001);

/* Update this with the correct SLP for accurate altitude measurements */
static float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;

static float headingCorrection; // used to alter how heading is displayed

static bool successful_init = false; // checks if sensors were initialized

/********************************

   Methods

 *********************************
*/

//Sets the starting heading to 0 //NOTICE: this code currently just returns rawHeading for testing purposes
float correctHeading(float rawHeading) {
  float retval = rawHeading - headingCorrection;
  if (retval > 180) {
    retval -= 360;
  } else if (retval <= -180) {
    retval += 360;
  }
  //return retval;
  return rawHeading;
}

/*
   gets corrected heading as an int
*/
int getHeading() {
  if(!successful_init) return ERR_READ_FAILED;
  
  sensors_event_t mag_event;
  sensors_vec_t   orientation;

  mag.getEvent(&mag_event); // get data from magnetometer
  if (dof.magGetOrientation(SENSOR_AXIS_Z, &mag_event, &orientation)) {
    // successful read
    return (int)correctHeading(orientation.heading);
  } else {
    // read failed
    return ERR_READ_FAILED;
  }
}

float getRawHeading() {
  if(!successful_init) return ERR_READ_FAILED;
 
  sensors_event_t mag_event;
  sensors_vec_t   orientation;

  mag.getEvent(&mag_event); // get data from magnetometer
  if (dof.magGetOrientation(SENSOR_AXIS_Z, &mag_event, &orientation)) {
    // successful read
    return orientation.heading;
  } else {
    // read failed
    return ERR_READ_FAILED;
  }
}

/*
   Initializes us. If something fails to initialize, we call a failed init and return
*/
int init10DOF() {
  int err_code = 0;
  if (!accel.begin()) err_code += ERR_NO_ACCEL;
  if (!mag.begin()) err_code += ERR_NO_MAG;
  if (!bmp.begin()) err_code += ERR_NO_BMP;
  if (err_code == SUCCESS) successful_init = true;
  if (successful_init) {
    // set up heading correction things
    headingCorrection = getRawHeading();
    if (headingCorrection > 360) err_code += headingCorrection; // check if our heading is broken
  }
  return err_code;
}
