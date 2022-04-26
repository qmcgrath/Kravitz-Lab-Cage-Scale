#include "Arduino.h"
#include "calibration2.h"

RTC_PCF8523 rtc;

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


void Scale::simpLinReg(float* x, float* y, float* lrCoef, int n) {
  // pass x and y arrays (pointers), lrCoef pointer, and n.  The lrCoef array is comprised of the slope=lrCoef[0] and intercept=lrCoef[1].  n is length of the x and y arrays.
  // http://en.wikipedia.org/wiki/Simple_linear_regression

  // initialize variables
  float xbar = 0;
  float ybar = 0;
  float xybar = 0;
  float xsqbar = 0;

  // calculations required for linear regression
  for (int i = 0; i < n; i++) {
    xbar = xbar + x[i];
    ybar = ybar + y[i];
    xybar = xybar + x[i] * y[i];
    xsqbar = xsqbar + x[i] * x[i];
  }
  xbar = xbar / n;
  ybar = ybar / n;
  xybar = xybar / n;
  xsqbar = xsqbar / n;

  // simple linear regression algorithm
  lrCoef[0] = (xybar - xbar * ybar) / (xsqbar - xbar * xbar);
  lrCoef[1] = ybar - lrCoef[0] * xbar;
}

void Scale::start_Up() {
  pinMode(13, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(DISGRND, OUTPUT);
  pinMode(DISPLAYPOWER, OUTPUT);
  digitalWrite(DISGRND, LOW);
  digitalWrite(DISPLAYPOWER, HIGH);

  //Start Serial port
  Serial.begin(9600);
  scale.begin(DOUT, CLK);
  scale.set_scale();


  start_timer = millis();
  scale.tare();
  Serial.println("Adafruit VL6180x test!");
  if (! vl.begin()) {
    Serial.println("Failed to find sensor");
  }
  Serial.println("Sensor found!");
  SdFile::dateTimeCallback(dateTime);
  Serial.println("creating datafile");
  CreateDataFile();

  if (!rtc.initialized() || rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  rtc.start();

  //delay (3000);
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 allocation failed");
  }


  display.clearDisplay();
  scale.tare();
}

void Scale::acquireData() {

}

void Scale::getBallParkData() {
  Serial.println("entered cal");
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(1, 4);
  display.setTextColor(SSD1306_WHITE);
  int knownWeights [] = {10 , 20, 30, 40};
  int sizeArrary = ( sizeof(knownWeights) / sizeof(knownWeights[0]) ) - 1 ;
  for (int i = 0; i <= sizeArray; i++) {
    knownWeight = knownWeights[i];
    display.print("place ");
    display.print(knownWeight);
    display.println(" kg now....");
    Serial.print("place ");
    Serial.print(knownWeight);
    Serial.println("kg now... ");

    display.display();
    delay(8000);
    display.clearDisplay();
    timer = millis();
    while ((millis() - timer ) < 10000 ) {
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(2, 6);
      display.print((millis() - timer) / 10000);
      display.display();
      Serial.print("running ");
      Serial.println(knownWeight);
      outputValue = scale.get_units(10);
      buf[bufItter] = outputValue;
      Serial.println(buf[bufItter]);
      bufItter++;
      WriteToSD();
    }

    for (int k = 0; k <= 50; k++) {
      if (buf[k] != 0) {
        avg += buf[k];
        num = num + 1;
      }
    }
    memset(buf, 0, sizeof(buf));
    avg = avg / num10;
    x[funcItter] = knownWeight;
    y[funcItter] = avg;
    funcItter++;
    bufItter = 0;

  }

  simpLinReg(x, y, lrCoef, 4);

}

void Scale::manualFineTune() {
  Serial.println("Take off weight");
  delay(5000);
  Serial.println("Dont touch, taring");
  delay(4000);
  float calibration_factor = 100 * round(lrCoef[0] / 100.0);
  scale.set_scale(calibration_factor);
  scale.tare(); //Reset the scale to 0
  delay(4000);
  bool runTune = true;
  Serial.println("Place 20 g on scale when readings begin");
  while (runTune) {
    scale.set_scale(calibration_factor);
    Serial.print("Reading: ");
    Serial.print(scale.get_units());
    Serial.print(" g"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
    Serial.print(" calibration_factor: ");
    Serial.print(calibration_factor);
    Serial.println();

    if (Serial.available())
    {
      char temp = Serial.read();
      if (temp == 'e') {
        runTune = false;
      }
      if (temp == '+' || temp == 'a')
        calibration_factor += 10;
      else if (temp == '-' || temp == 'z')
        calibration_factor -= 10;
    }
  }

}


/*
  void Scale::getBallParkData() {


  //take output in one minute intervals for 4 known weights, 15 second delay between
  Serial.println("entered cal");
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(1, 4);
  display.setTextColor(SSD1306_WHITE);
  display.print("place 10 kg now....");
  Serial.println("place 10 kg now....");
  display.display();
  delay(10000);
  display.clearDisplay();
  timer = millis();
  while ((millis() - timer ) < 10000 ) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(2, 6);
    display.print((millis() - timer) / 10000);
    display.display();
    Serial.println("running 10");
    knownWeight = 10;
    outputValue = scale.get_units(10);
    buf10[bufItter] = outputValue;
    Serial.println(buf10[bufItter]);
    bufItter++;
    WriteToSD();
  }

  for (int k = 0; k <= 50; k++) {
    if (buf10[k] != 0) {
      avg10 += buf10[k];
      num10 = num10 + 1;
    }
  }

  avg10 = avg10 / num10;
  x[funcItter] = knownWeight;
  y[funcItter] = avg10;
  funcItter++;
  bufItter = 0;


  display.clearDisplay();
  display.setCursor(1, 4);
  display.print("place 20 kg now....");
  Serial.println("place 20 kg now....");
  display.display();
  delay(10000);
  display.clearDisplay();
  timer = millis();
  while ((millis() - timer ) < 10000 ) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(2, 6);
    display.print((millis() - timer) / 10000);
    display.display();
    Serial.println("running 20");
    knownWeight = 20;
    outputValue = scale.get_units(10);
    buf20[bufItter] = outputValue;
    bufItter++;
    WriteToSD();
  }

  for (int k = 0; k <= 50; k++) {
    if (buf20[k] != 0) {
      avg20 += buf20[k];
      num20 ++;
    }
  }
  avg20 = avg20 / num20;
  Serial.println(avg20);
  x[funcItter] = knownWeight;
  y[funcItter] = avg20;
  funcItter++;
  bufItter = 0;

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 4);
  display.setTextColor(SSD1306_WHITE);
  display.print("place 30 kg now....");
  Serial.println("place 30 kg now....");
  display.display();
  delay(10000);
  display.clearDisplay();
  timer = millis();

  while ((millis() - timer ) < 10000 ) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(2, 6);
    display.print((millis() - timer) / 10000);
    display.display();
    Serial.println("running 30");
    knownWeight = 30;
    outputValue = scale.get_units(10);
    buf30[bufItter] = outputValue;
    bufItter++;
    WriteToSD();
  }

  for (int k = 0; k <= 50; k++) {
    if (buf30[k] != 0) {
      avg30 += buf30[k];
      num30 ++;
    }
  };
  avg30 = avg30 / num30;
  x[funcItter] = knownWeight;
  y[funcItter] = avg30;
  funcItter++;
  bufItter = 0;


  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 4);
  display.setTextColor(SSD1306_WHITE);
  display.print("place 40 kg now....");
  Serial.print("place 40 kg now....");
  display.display();
  delay(10000);
  display.clearDisplay();
  timer = millis();

  while ((millis() - timer ) < 10000 ) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(2, 6);
    display.print((millis() - timer) / 10000);
    display.display();
    Serial.println("running 40");
    knownWeight = 40;
    outputValue = scale.get_units(10);
    buf40[bufItter] = outputValue;
    bufItter++;
    WriteToSD();
  }

  for (int k = 0; k <= 50; k++) {
    if (buf40[k] != 0) {
      avg40 += buf40[k];
      num40++;
    }
  };
  avg40 = avg40 / num40;
  Serial.println(avg40);
  x[funcItter] = knownWeight;
  y[funcItter] = avg40;
  funcItter++;
  bufItter = 0;

  for (int k = 0; k <= 3; k++) {
    Serial.println( x[k]);
    Serial.println( y[k]);
  };

  }
*/
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
  dataFile.print(knownWeight);
  dataFile.print(", ");
  dataFile.println(outputValue);
  dataFile.flush();
}

void Scale::writeHeader() {
  dataFile = SD.open(filename, FILE_WRITE);
  if ( ! dataFile ) {
    Serial.println("SD Create error");
    error(2);
  }
  dataFile.println("MM: DD: YYYY hh: mm: ss, Seconds, filename, knownWeight, output_value");

}

void Scale::CreateDataFile() {
  SdFile::dateTimeCallback(dateTime);
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
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
  Serial.println(now.day());
  Serial.println(now.month());
  Serial.println(now.year());

  filename[0] = 'C';
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


void Scale::warmUp() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(2, 4);
  display.setTextColor(SSD1306_WHITE);
  display.print("10 min warm up");
  display.display();
  timer = millis();
  //11 minute hold
  while ((millis() - timer) < 600000) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(2, 4);
    display.print((millis() - timer) / 60000);
    display.display();
    grams = (scale.get_units(100));
  }

}

void Scale::error(uint8_t error) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 2);
  display.print("Check SD card");
  display.display();
}

void Scale::start_up_menu() {
  float start_timer = millis();
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 2);     // Start at top-left corner

  while (start_up ) {
    if ((millis() - start_timer) > 6000)  start_up = false; //after 10 seconds of start up menu, start session
    display.setCursor(2, 2);
    display.println("Scale beginning!");
    display.setCursor(2, 11);
    display.println("filename:    ");
    display.setCursor(2, 20);
    display.println(filename);
    display.display();
  }

  display.clearDisplay();
}
