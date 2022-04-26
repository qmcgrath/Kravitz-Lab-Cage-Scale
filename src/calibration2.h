
#ifndef calibration2_h
#define calibration2_h

#include "Arduino.h"

#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
///#include <vector.h>
#include <Wire.h>
#include "Adafruit_VL6180X.h"
#include <vector>
#include <unistd.h>
#include <numeric>
#include "HX711.h"
#include "RTClib.h"
#include <Adafruit_SSD1306.h>


#define DISGRND 6
#define DISPLAYPOWER 10
#define DISPLAYRESET 5

#define DOUT A1
#define CLK A0
#define VBATPIN A7

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET 5 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

using namespace std;

void dateTime(uint16_t* date, uint16_t* time);

class Scale {

  public:
    Scale(String ver);
    String ver = "undef";
    String library_version = "undef";


    int knownWeight = 0;
    //for curve fitting
    void simpLinReg(float* x, float* y, float* lrCoef, int n);
    void getBallParkData();
    void manualFineTune();
    void acquireData();
    
    float y[4] ;
    float x[4] ;
    float lrCoef[2] = {0, 0};

    //for func
    float buf[50];
    float avg =0; 
    float num = 0; 

    
    float buf10[50];
    float buf20[50];
    float buf30[50];
    float buf40[50];
    float avg10 = 0;
    float avg20 = 0;
    float avg30 = 0;
    float avg40 = 0;
    float num10 = 0;
    float num20 = 0;
    float num30 = 0;
    float num40 = 0;
    int bufItter = 0;
    int funcItter = 0;
    //basic functions

    void warmUp();
    void start_Up();
    float timer = millis();
    //load cells
    HX711 scale;
    unsigned long mill = millis();
    //float calibration_factor = 4610;
    float scaleChange = 0;    // Load cell 1
    float lastReading = 0;    // Load cell 1
    float outputValue;        // Load cell 1
    float grams;

    bool calibrate_active = true;
    bool calibrated = false;

    float calibrateBuffer[200];
    float calibrationAvg = 0;
    float totalCal = 0;
    int numInCal = 0;


    unsigned long start_timer = 0;
    unsigned long taring_timer = 0;
    bool start_up = true;
    int proxLimitforMouseIn = 80;
    unsigned long delayTime = 1000;
    unsigned long tareWait = 30000;


    //proximity senor
    Adafruit_VL6180X vl = Adafruit_VL6180X();
    float lux = 0;
    uint8_t range = 0;
    unsigned long mouseHere_timer = 0;

    //display
    Adafruit_SSD1306 display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

    void updateDisplay();
    void start_up_menu();

    //sd file
    File dataFile;               // Create file object
    char filename[15] ;          // Array for file name data logged to named in setup
    const int chipSelect = 4;
    void CreateDataFile();
    void writeHeader();
    void WriteToSD();
    void error(uint8_t errno);
    void getFilename(char *filename);
    unsigned long unixtime = 0;
    int SCL = 1;

    char scaleID = 'A';

    float measuredvbat = 1.00;
    void ReadBatteryLevel();

    //rtc
    const byte seconds = 0;
    const byte minutes = 0;
    const byte hours = 16;
    const byte day = 15;
    const byte month = 6;
    const byte year = 15;
    bool calibrate = true;
};

#endif
