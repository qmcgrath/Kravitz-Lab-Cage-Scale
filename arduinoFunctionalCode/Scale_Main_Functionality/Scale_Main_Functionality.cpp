#include "Scale_Main_Functionality.h"

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

  //get file id if it exists, if not will have to be set manually in h file. If settings differs from hfile, sd will overwrite her

  Serial.println("Sensor found!");
  Serial.println(scaleID);
  SdFile::dateTimeCallback(dateTime);
  Serial.println("creating datafile");
  CreateDataFile();

  display.clearDisplay();
  scale.tare();

  if (SD.exists(calFilename)) {
    calFile = SD.open("settings.txt");
    scaleID = calFile.read();
    calFile.close();

  }
  else {
    noSettingsFile();
  }

}



//must manually set scale id
void Scale::noSettingsFile() {
  float start_timer = millis();
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner

  display.println("NO SETTINGS FILE");
  display.println("Flash startup code");
  display.display();

  while (true) {
    Serial.println("no settings file");
  }

}



void Scale::start_up_menu() {
  float start_timer = millis();
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner

  display.println("Starting up");
  display.display();
  //wait x seconds for heavy press
  delay(4000);
  float starttime = millis();
  start_up_ScreenTwo();
}

void Scale::start_up_ScreenTwo() {
  // read in and set calibration factor from filename
  dataFile.close();

  // re-open the file for reading:
  calFile = SD.open(calFilename);
  int cal;
  if (calFile) {
    Serial.println(calFilename);

    // read from the file until there's nothing else in it:

    scaleID = calFile.read();
    cal = calFile.parseInt();
    Serial.print(cal);
    while (calFile.available()) {
      calFile.read();
    }

    calFile.close();
  }
  else {
    // if the file didn't open, print an error:
    Serial.println("error opening setting.txt");
  }

  Serial.println("id read from file:");
  Serial.println(scaleID);
 ///!!!!!!!!!!!!!!!
  calibration_factor = 3530 ;
  Serial.println("cal read from file:");
  Serial.println(calibration_factor);

  //write and open main file
  dataFile = SD.open(filename, FILE_WRITE);

  float start_timer = millis();

  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner


  while (start_up ) {
    if ((millis() - start_timer) > 6000) start_up = false; //after 10 seconds of start up menu, start session
    display.setCursor(2, 2);
    display.println("Scale beginning!");
    display.setCursor(2, 11);
    display.println("filename:    ");
    display.setCursor(2, 20);
    display.println(filename);
    display.display();
  }
  display.clearDisplay();
  display.setCursor(2, 10);
  display.println("dont touch...taring");
  display.display();
  delay(3000);
  scale.tare();
  display.clearDisplay();
  Serial.println("end of startup");
}



////////////////menus and display functions


void Scale::updateDisplay() {
  Serial.println("in update display");
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 2);
  display.setTextColor(SSD1306_WHITE);
  display.print("grams: ");
  display.println(grams);
  display.print("Range: ");
  display.println(range);
  display.print("Avg Zero: ");
  display.println(movingZeroAvg );
  display.print("Cgrams: ");
  display.println(cgrams);
  display.display();
}



////////////file and SD functions
void Scale::error(uint8_t error) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 2);
  display.print("Check SD card");
  display.display();
}

void Scale::CreateDataFile() {
  SdFile::dateTimeCallback(dateTime);
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 2);
    display.setTextColor(SSD1306_WHITE);
    display.print("SD Card failed, or not present");
    while (1);
  }
  strcpy(filename, "________.CSV");  // placeholder filename
  Serial.println("entering get filename");
  getFilename(filename);
  Serial.println(filename);
  writeHeader();
}


void Scale::getFilename(char *filename) {

  DateTime now = rtc.now();
  filename[0] = 'S';
  filename[1] = scaleID;
  filename[2] = now.month() / 10 + '0';
  filename[3] = now.month() % 10 + '0';
  filename[4] = now.day() / 10 + '0';
  filename[5] = now.day() % 10 + '0';
  filename[8] = '.';
  filename[9] = 'C';
  filename[10] = 'S';
  filename[11] = 'V';

  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = '0' + i / 10;
    filename[7] = '0' + i % 10;

    if (! SD.exists(filename)) {
      break;
    }
  }
  Serial.println(filename);
  return;
}

void Scale::writeHeader() {
  dataFile = SD.open(filename, FILE_WRITE);
  if ( ! dataFile ) {
    Serial.println("SD Create error");
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 2);
    display.setTextColor(SSD1306_WHITE);
    display.print("SD File Create Error");
    error(2);
  }
  dataFile.println("MM: DD: YYYY hh: mm: ss, Seconds, filename, calibration factor, output_value, lux, range, grams, tareCount, weightAvg, zeroAvg, cgrams, currentMaxHist, batteryLevel");
}

void Scale::WriteToSD() {
  DateTime now = rtc.now();
  dataFile.print(now.month());
  dataFile.print(" / ");
  dataFile.print(now.day());
  dataFile.print(" / ");
  dataFile.print(now.year());
  dataFile.print(" ");
  dataFile.print(now.hour());
  dataFile.print(": ");
  if (now.minute() < 10)
    dataFile.print('0');      // Trick to add leading zero for formatting
  dataFile.print(now.minute());
  dataFile.print(": ");
  if (now.second() < 10)
    dataFile.print('0');      // Trick to add leading zero for formatting
  dataFile.print(now.second());
  dataFile.print(", ");

  dataFile.print((millis() - start_timer) / 1000.0000);
  dataFile.print(", ");
  dataFile.print(filename);
  dataFile.print(", ");
  dataFile.print(calibration_factor);
  dataFile.print(", ");
  dataFile.print(outputValue);
  dataFile.print(", ");
  dataFile.print(lux);
  dataFile.print(", ");
  dataFile.print(range);
  dataFile.print(", ");
  dataFile.print(grams);
  dataFile.print(", ");
  dataFile.print(taredCount);
  dataFile.print(", ");
  dataFile.print(movingWeightAvg);
  dataFile.print(", ");
  dataFile.print(movingZeroAvg);
  dataFile.print(", ");
  dataFile.print(cgrams);
  dataFile.print(", ");
  dataFile.print(histToWrite);
  dataFile.print(", ");
  dataFile.println(measuredvbat);
  dataFile.flush();
}

void Scale::manageHist(float grams) {
  if ((grams >= 15) && (grams <= 40)) {
    hist.add(grams);
  }
  if (hist.count() >= 2000) {
    histToWrite = bounds[hist.findMax()];
    hist.clear();
  }
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

void Scale::calcZeroAvg() {
  currentGram = scale.get_units(5);
  if (currentGram <= 15) {
    avgZeroBuffer[numInAvgZero] = currentGram;
    if (numInAvgZero != 0 && numInAvgZero % 4 == 0) {
      for (int i = 0; i < 5; i++ ) {
        totalZero += avgZeroBuffer[i];
      }
      movingZero =  totalZero / numInAvgZero;
      numInAvgZero = -1;
      totalZero = 0;
    }
    numInAvgZero++;
  }
  for (int i = 0; i < 5; i++ ) {
    if (abs( avgZeroBuffer[i] - movingZeroAvg) > .2) {
      withinLimits = false;
    }
  }
  //write to sd
  if (withinLimits) {
    Serial.println("passing within moving avg limits");
    movingZeroAvg = movingZero;
  }
  updateDisplay();
}



/////////check battery
void Scale::ReadBatteryLevel() {
  measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
}
