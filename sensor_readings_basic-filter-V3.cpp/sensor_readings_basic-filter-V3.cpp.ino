/**
 * 2018-19 Electronics Team Code for Sensor Display
 * Written by Parth, Tyler, and Derek. Add your name if you work on this
 * 
 * Code reads in data from arduino sensors and displays it to the pilot.
 * Uses a basic averaging filter
 * 
 * Most recent change: Moved the Adafruit stuff to TenDOF.cpp
 * 
 * Known Issues: The averaging filter almost definitely doesn't work between -179 and 180.
 * 
 * Misc Notes: Code compiled suspiciously easily. Rigorous testing reccomended.
 */
//10DOF includes
#include "TenDOF.h"

//LCD Include
#include <LiquidCrystal.h>

//Depth Include
#include <Wire.h>
#include "MS5837.h"

//SD saving include
#include <SPI.h>
#include <SD.h>

/*Conditional Compiling Indicators*/
//#define USING_DEPTH
#define USING_10DOF

//Printing definitions
#define UPDATE_DELAY 1000
#define BAUD 115200

//pins for LCD
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

//used for moving average filter
#define FILTER_SIZE 10

/**
 * 1.225 for air (round to 1)
 * 997 for water
 */
#define FLUID_DENSITY 1

#ifdef USING_DEPTH
//declare depth sensor
MS5837 depthSensor;
#endif

static LiquidCrystal lcd = LiquidCrystal(LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

//SD card things
static File logData;
static String fileName = "fnf"; // file not found (yet)
static bool sdInit = false;

// create filters for the things we're using
#ifdef USING_10DOF
static float headingReadings[FILTER_SIZE];
#endif

#ifdef USING_DEPTH
static float detphReadings[FILTER_SIZE];
#endif

static int readingCount = 0;

// prints to both serial and sd card
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

int takeAverageInt(float * intArr, int arrSz) {
  int sum = 0;
  for(int i = 0; i < arrSz; i++) {
    sum += intArr[i];
  }
  return (int)(sum / arrSz);
}

int headingModularAverage(float * headings, int arrSz) {
  for(int i = arrSz - 1; i >= 0; i--) {
    
  }
}

/*
 * Initializes the sensors we're going to use. Stops the code if things are missing.
 */
void initSensors()
{
  int err_code = init10DOF()
  if(err_code != SUCCESS) {
    printLog(F("10DOF error code "));
    printLogln((String)err_code);
    while(1);
  }

  //depth activations
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

void initSD

/**************************************************************************/
/*!

*/
/**************************************************************************/
void setup(void)
{  
  Serial.begin(BAUD);
  
  //init SD card
  if(!SD.begin(SD_PIN)){
    sdInit = false;
    printLogln("SD init failed! data will not be saved.");
  } else {
    // select filename to be a file which does not exist yet
    fileName = "log_data_";
    int i = 1;
    while(SD.exists((String)(fileName + i + ".txt"))) {
      printLogln("filename " + fileName + i + ".txt taken");
      i++; // check log_data_1, then log_data_2, etc etc until name not taken
    }
    fileName = fileName + i + ".txt";
    sdInit = true;// SD successfully init
  }
  
  printLogln(F("Submarine Log Data is go!")); 
  printLogln("");
  
  initSensors();

  lcd.begin(LCD_COL, LCD_ROW);
}

/*
 * Our tasks for the loop:
 *  Open up the SD for reading
 *  If we have 10DOF, record the heading
 *  If we have depth, record the depth
 *  Print this data to the SD
 *  Print this data to the LCD
 *  Close and save the SD
 */
void loop(void)
{

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
  // get heading data
  int heading = getHeading();
  headingReadings[readingCount] = heading;
  printLog(F("Heading: "));
  printLog((String)heading);
  printLog(F("; "));

  // print our data to the lcd
  lcd.clear();
  lcd.print("Heading: ");
  lcd.print(headingModularAverage(headingReadings, FILTER_SIZE));
#endif

#ifdef USING_DEPTH
  // populate depth sensor data
  depthSensor.read();
  
  // get the depth from our depth sensor
  printLog(F("Depth: "));
  printLog((String)depthSensor.depth());
  printLogln(F(" m"));
  depthReadings[readingCount] = depthSensor.depth();

  // print data to lcd
  lcd.setCursor(0, 1);
  lcd.print("Depth: ");
  lcd.print(takeAverageInt(depthReadings, FILTER_SIZE));
#endif

  printLogln("");

  // Save file data
  if(sdInit) {
    logData.close();
  }

  // update reading count
  readingCount++;
  if(readingCount >= FILTER_SIZE) {
    readingCount = 0;
  }
  
  delay(UPDATE_DELAY); // make sure the screen isn't just flashing crazily
}
