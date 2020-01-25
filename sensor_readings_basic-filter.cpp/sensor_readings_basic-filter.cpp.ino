/**
 * 2018-19 Electronics Team Code for Sensor Display
 * Written by Parth, Tyler, and Derek. Add your name if you work on this
 * 
 * Code reads in data from arduino sensors and displays it to the pilot.
 * Uses a basic averaging filter
 * 
 * Most recent change: Adding averaging filter
 * 
 * Known Issues: The averaging filter almost definitely doesn't work between -179 and 180.
 * 
 * Misc Notes: Code compiled suspiciously easily. Rigorous testing reccomended.
 */
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_L3GD20_U.h>
#include <Adafruit_10DOF.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include "MS5837.h"
#include <SPI.h>
#include <SD.h>

#define UPDATE_DELAY 1000
#define BAUD 115200

#define LCD_COL 16
#define LCD_ROW 2
#define LCD_RS 12
#define LCD_ENABLE 11
#define LCD_D4 28
#define LCD_D5 26
#define LCD_D6 24
#define LCD_D7 22

//SS (Slave Select) pin
#define SD_PIN 53

//#define USING_DEPTH
#define USING_10DOF

/**
 * 1.225 for air (round to 1)
 * 997 for water
 */
#define FLUID_DENSITY 1

#ifdef USING_10DOF
/* Assign a unique ID to the sensors */
Adafruit_10DOF                dof   = Adafruit_10DOF();
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(30301);
Adafruit_LSM303_Mag_Unified   mag   = Adafruit_LSM303_Mag_Unified(30302);
Adafruit_BMP085_Unified       bmp   = Adafruit_BMP085_Unified(18001);

#endif

#ifdef USING_DEPTH

MS5837 depthSensor;

#endif

#define FILTER_SIZE 10

/* Update this with the correct SLP for accurate altitude measurements */
float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;

static float headingCorrection;

static LiquidCrystal lcd = LiquidCrystal(LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

static File logData;
static String fileName = "fnf"; // file not found (yet)
static bool sdInit = false;

#ifdef USING_10DOF
static float headingReadings[FILTER_SIZE];
#endif

#ifdef USING_DEPTH
static float detphReadings[FILTER_SIZE];
#endif

static int readingCount = 0;

void printLog(String toPrint) {
  Serial.print(toPrint);
  if(sdInit) {
    logData.print(toPrint);
  }
}

void printLogln(String toPrint) {
  Serial.println(toPrint);
  if(sdInit) {
    logData.println(toPrint);
  }
}

/**************************************************************************/
/*!
    @brief  Initialises all the sensors used by this example
*/
/**************************************************************************/
void initSensors()
{
  #ifdef USING_10DOF
  
  if(!accel.begin())
  {
    /* There was a problem detecting the LSM303 ... check your connections */
    printLogln(F("accel: no LSM303 detected ... Check wiring"));
    while(1);
  }
  if(!mag.begin())
  {
    /* There was a problem detecting the LSM303 ... check your connections */
    printLogln("mag: no LSM303 detected ... Check wiring");
    while(1);
  }
  if(!bmp.begin())
  {
    /* There was a problem detecting the BMP180 ... check your connections */
    printLogln("no BMP180 detected ... Check wiring");
    while(1);
  }
  // init values for measurement
  for(int i = 0; i < FILTER_SIZE; i++) {
    headingReadings[i] = 0;
  }
  #endif

  #ifdef USING_DEPTH
  
  // init the depth sensor (Wire.begin() is not a bool)
  Wire.begin();  
  depthSensor.init();
  depthSensor.setFluidDensity(FLUID_DENSITY);
  // initialize average values
  for(int i = 0; i < FILTER_SIZE; i++) {
    depthReadings[i] = 0;
  }
  #endif
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
void setup(void)
{
  sensors_event_t mag_event;
  sensors_vec_t   orientation;

  Serial.begin(BAUD);
  
  //init SD card
  if(!SD.begin(SD_PIN))
  {
    sdInit = false;
    printLogln("SD init failed! data will not be saved.");
  } else {
    // select filename to be a file which does not exist yet
    fileName = "log_data_";
    int i = 1;
    while(SD.exists((String)(fileName + i + ".txt"))) 
    {
      printLogln("filename " + fileName + i + ".txt taken");
      i++; // check log_data_1, then log_data_2, etc etc until name not taken
    }
    fileName = fileName + i + ".txt";
    sdInit = true;// SD successfully init
  }
  
  printLogln(F("Adafruit 10 DOF and Bar30 Depth meter")); 
  printLogln("");
  
  /* Initialise the sensors */
  initSensors();

  #ifdef USING_10DOF
  
  /* Calculate the heading using the magnetometer */
  mag.getEvent(&mag_event);
  if (dof.magGetOrientation(SENSOR_AXIS_Z, &mag_event, &orientation))
  {
    /* 'orientation' should have valid .heading data now */
    printLog(F("Starting Heading: "));
    printLog((String)orientation.heading);
    printLogln(F(""));

    headingCorrection = orientation.heading;
  }
  
  #endif

  lcd.begin(LCD_COL, LCD_ROW);

}

//Sets the starting heading to 0 //NOTICE: this code currently just returns rawHeading for testing purposes
float correctHeading(float rawHeading) {
  float retval = rawHeading - headingCorrection;
  if(retval > 180){
    retval -= 360;
  } else if(retval <= -180){
    retval += 360;
  }
  //return retval;
  return rawHeading;
}

int takeAverageInt(float * intArr, int arrSz) {
  int sum = 0;
  for(int i = 0; i < arrSz; i++) {
    sum += intArr[i];
  }
  return (int)(sum / arrSz);
}
/**************************************************************************/
/*!
    @brief  Constantly check the roll/pitch/heading/altitude/temperature
*/
/**************************************************************************/
void loop(void)
{
  #ifdef USING_10DOF

  //sensors_event_t accel_event; unused
  sensors_event_t mag_event;
  //sensors_event_t bmp_event; unused
  sensors_vec_t   orientation;

  #endif

  //Start writing to the SD card if sdInit == true
  if(sdInit) {
    logData = SD.open(fileName, FILE_WRITE);
    if(!logData){
      printLog(F("File "));
      printLog(fileName);
      printLogln(F(" failed to open D:"));
      sdInit = false; // keeps us from printing to nonexistant file
    }
  }

  #ifdef USING_10DOF
  /* Calculate the heading using the magnetometer */
  mag.getEvent(&mag_event);
  if (dof.magGetOrientation(SENSOR_AXIS_Z, &mag_event, &orientation))
  {
    /* 'orientation' should have valid .heading data now */
    printLog(F("Heading: "));
    printLog((String)correctHeading(orientation.heading));
    printLog(F("; "));
    headingReadings[readingCount] = correctHeading(orientation.heading);
  }
  #endif

#ifdef USING_DEPTH
  // populate depth sensor data
  depthSensor.read();
  
  // get the depth from our depth sensor
  printLog(F("Depth: "));
  printLog((String)depthSensor.depth());
  printLogln(F(" m"));
  depthReadings[readingCount] = depthSensor.depth();
#endif

  // Save file data
  if(sdInit) {
    logData.close();
  }

#ifdef USING_10DOF
  // print our data to the lcd
  lcd.clear();
  lcd.print("Heading: ");
  lcd.print(takeAverageInt(headingReadings, FILTER_SIZE));
#endif

#ifdef USING_DEPTH
  lcd.setCursor(0, 1);
  lcd.print("Depth: ");
  lcd.print(takeAverageInt(depthReadings, FILTER_SIZE));
#endif

  
  printLogln("");

  // update reading count
  readingCount++;
  if(readingCount >= FILTER_SIZE) {
    readingCount = 0;
  }
  
  delay(UPDATE_DELAY); // make sure the screen isn't just flashing crazily
}
