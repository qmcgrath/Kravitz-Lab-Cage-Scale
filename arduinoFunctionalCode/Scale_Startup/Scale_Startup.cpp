#include "Scale_Startup.h"

RTC_PCF8523 rtc;
//for histogram

float bounds[400];
Histogram hist(400, bounds);

void dateTime(uint16_t* date, uint16_t* time) {
  DateTime now = rtc.now();
  // return date using FAT_DATE macro to format fields
  *date = FAT_DATE(now.year(), now.month(), now.day());

  // return time using FAT_TIME macro to format fields
  *time = FAT_TIME(now.hour(), now.minute(), now.second());
}

Scale::Scale(String ver) {
  library_version = ver;
}

/////////////////start UP
void Scale::startUp() {
  ///CHANGE VALUE TO SET SCALE ID !!!!!!!!!!!!!!!!!!!
  scaleID = 'A';
  pinMode(13, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(DISGRND, OUTPUT);
  pinMode(DISPLAYPOWER, OUTPUT);
  digitalWrite(DISGRND, LOW);
  digitalWrite(DISPLAYPOWER, HIGH);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 allocation failed");
  }

  //Start Serial port
  Serial.begin(9600);
  scale.begin(DOUT, CLK);
  scale.set_scale();

  rtc.begin();
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  start_timer = millis();
  scale.tare();
  Serial.println("Adafruit VL6180x test!");
  if (! vl.begin()) {
    Serial.println("Failed to find sensor");
  }


  SdFile::dateTimeCallback(dateTime);
  display.clearDisplay();
  scale.tare();

}


void Scale::start_up_menu() {
  float start_timer = millis();
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner

  display.println("Running calibration");
  display.println("prepare weights");
  display.display();
  display.println("...wait 5 sec");
  display.display();
  //wait x seconds for heavy press
  delay(5000);
  starttime = millis();

  newTare();

}

//called when indicated by user at start up, will calibrate and over write calibration value in txt file
//must manually set scale id
void Scale::newTare() {
  if (SD.exists(calFilename)) {
    Serial.println("removing existings settings");
    SD.remove(calFilename);

    float start_timer = millis();
    display.clearDisplay();
    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 0);     // Start at top-left corner

    display.println("Overwriting previous");
    display.println("file");
    display.display();
    display.println("prep 20 g weight");
    display.display();
  }
  delay(2000);
  //calibrate func return val

  getBallParkData();
  manualFineTune();

  calFile = SD.open(calFilename, FILE_WRITE);
  // if the file opened okay, write to it:
  calFile.println(scaleID);
  calFile.println(calibration_factor);
  // close the file:
  calFile.close();
  Serial.println("done.");
  if (calFile) {
    Serial.print("Writing to settings.txt...");
    calFile.println(scaleID);
    calFile.println(calibration_factor);
    // close the file:
    calFile.close();
    Serial.println("done.");
  }
  else {
    // if the file didn't open, print an error:
    Serial.println("error opening setting.txt");
  }
}



////////////////menus and display functions



void Scale::manualFineTune() {
  float start_timer = millis();
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner

  display.println("Running calibration");
  display.println("Round 2");
  display.display();
  Serial.println("Round 2");



  delay(2000);
  bool runTune = true;

  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.println("Running calibration");
  display.println("Round 2");
  display.display();
  display.println("Place 20g");

  delay(3000);
  Serial.println("Place 20 g on scale when readings begin");
  bool runningCal = true;

  while (runningCal) {
    calibrationAvg = 0.0;
    scale.set_scale(calibration_factor);
    delay(500);
    for (int i = 0; i < 20; i++) {
      calibrateBuffer[i] = scale.get_units();
      calibrationAvg += calibrateBuffer[i];

    }
    calibrationAvg = calibrationAvg / 20;
    Serial.println(calibrationAvg);
    Serial.println(calibration_factor);
    if (calibrationAvg > 20.05) {
      timesCorrect = 0;
      calibration_factor = calibration_factor + 5;
    }
    else if (calibrationAvg < 19.95) {
      timesCorrect = 0;
      calibration_factor = calibration_factor - 5;
    }
    else {
      timesCorrect += 1;
      if (timesCorrect == 3) {
        runningCal = false;
      }
    }
  }
  Serial.println("done with fine tune");
}

void Scale::getBallParkData() {
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.println("Running calibration");
  display.println("Round 1");
  display.display();
  display.println("...wait 5 sec");
  display.println("taring");
  display.display();
  delay(2000);
  calibration_factor = 3000;
  scale.set_scale(calibration_factor);
  scale.tare(); //Reset the scale to 0
  delay(2000);


  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.println("Running calibration");
  display.println("Round 1");
  display.display();
  display.println("Place 20g");

  delay(3000);


  bool runTune = true;
  Serial.println("Place 20 g on scale when readings begin");
  bool runningCal = true;

  while (runningCal) {
    calibrationAvg = 0.0;
    scale.set_scale(calibration_factor);
    delay(500);
    for (int i = 0; i < 20; i++) {
      calibrateBuffer[i] = scale.get_units();
      calibrationAvg += calibrateBuffer[i];

    }
    calibrationAvg = calibrationAvg / 20;
    Serial.println(calibrationAvg);
    Serial.println(calibration_factor);
    if (calibrationAvg > 21) {
      timesCorrect = 0;
      calibration_factor = calibration_factor + 100;
    }
    else if (calibrationAvg < 19) {
      timesCorrect = 0;
      calibration_factor = calibration_factor - 100;
    }
    else {
      timesCorrect += 1;
      if (timesCorrect == 3) {
        runningCal = false;
      }
    }
  }
  Serial.println("done with ballpark tune");

}


void Scale::Sense() {
  grams = (scale.get_units(5));
  if (grams < 0) {
    grams = 0;
  }
  Serial.print(scale.get_units(5));
  cgrams = grams - movingZeroAvg;
  Serial.print("grams = ");
  Serial.println(grams);

  outputValue = scale.read();
  Serial.print("failing at read scale");

  range = vl.readRange();
  lux =  vl.readLux(VL6180X_ALS_GAIN_5 );
  Serial.print("range = ");
  Serial.println(range);

  Serial.print("failing at range");
}

void Scale::Tare() {
  scale.tare();
  taredCount ++;
}

void Scale::calcMovingAvg(float currentGram) {
  if ( (currentGram >= 15) && (currentGram <= 40)) {
    avgWeightBuffer[numInAvgWeight ] = currentGram;
    if (numInAvgWeight != 0 && numInAvgWeight % 9 == 0) {
      for (int i = 0; i < 10; i++ ) {
        totalWeight += avgWeightBuffer[i];
      }

      movingWeightAvg =  (totalWeight) / numInAvgWeight;
      Serial.println(movingWeightAvg);
      numInAvgWeight = -1;
      totalWeight = 0;
    }
    numInAvgWeight++;
  }
}
