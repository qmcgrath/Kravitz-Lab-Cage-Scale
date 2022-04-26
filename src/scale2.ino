#include "scale2.h"

Scale scale("ver1") ;

enum State_enum {mouseIn, waitBuffer, getZero};
uint8_t state = waitBuffer;
float zeroBuffer[20];
int numInZero = 0;
float total = 0;
float movingZero = 0;
bool withinLimits = true;

void setup() {
  Serial.println("startup");

  scale.calibration_factor  = 3320; //A
  scale.scaleID ='A';
  //scale.calibration_factor  = 3530; //B
  //scale.scaleID = 'B';
//  scale.calibration_factor  = 3530; //B
  //scale.calibration_factor  = 5000; //C
 //scale.scaleID ='C';


  scale.startUp();
  Serial.println("left startup");
  scale.proxLimitforMouseIn = 90 ;
  scale.delayTime = 1000;
  scale.tareWait = 30000;

  Serial.println("entering meu");
  scale.start_up_menu();
  scale.start_timer = millis();
  scale.scale.set_scale(scale.calibration_factor);
  // scale.autoCalibrate();
  scale.taring_timer = millis();
}

boolean mouseInHouse = false;
boolean notInHouse = false;
boolean sleepState = false;

float scaleChange = 0;    // Load cell 1
float totalChange = 0;    // Load cell 1
float lastReading = 0;    // Load cell 1

void loop() {
  scale.Sense();
  switch (state) {

    case mouseIn:
      Serial.println("mouse in state");
      scale.Sense();
      scale.mouseHere_timer = millis();
      mouseInHouse = true;
      scale.WriteToSD();
      lastReading = scale.grams;
      scale.updateDisplay();
      //changed to C grams
      scale.calcMovingAvg(scale.cgrams);
      //changed from 100 t0 200
      delay(200);
      scale.manageHist(scale.cgrams);
      //switch if mouse leaves
      if (scale.range > 70) {
        state = waitBuffer;
      }

      break;

    case waitBuffer:
      Serial.println("waiting");
      scale.Sense();
      scale.WriteToSD();
      scale.updateDisplay();

      //wait 20 seconds, if mouse in then go back, if no then zero
      if (scale.range < 60) {
        state = mouseIn;
      }
      if ((millis() - scale.mouseHere_timer) > 20000 ) {
        state = getZero;
      }
      //changed from 500 to 1000
      delay(1000);

      if ((millis() - scale.taring_timer) > 1000000) {
        scale.display.ssd1306_command(SSD1306_DISPLAYOFF);
      }
      break;

    // add escape state if havent been zerod in long time


    case getZero:
      Serial.println("getting a zero");
      withinLimits = true;
      if (scale.range < 60) {
        state = mouseIn;
      }
      getZeroAvg();
      //state = waitBuffer;
      break;


  }
  scale.ReadBatteryLevel();
}


void getZeroAvg() {
  scale.Sense();
  if (scale.grams <= 15) {
    zeroBuffer[numInZero] = scale.grams;
    if (numInZero != 0 && numInZero % 19 == 0) {
      for (int i = 0; i < 20; i++ ) {
        total += zeroBuffer[i];
      }
      movingZero =  total / numInZero;
      numInZero = -1;
      total = 0;
    }
    numInZero++;
  }
  for (int i = 0; i < 20; i++ ) {
    if (abs( zeroBuffer[i] - movingZero) > .1) {
      withinLimits = false;
    }
  }
  //write to sd
  if (withinLimits) {
    Serial.println("passing within moving avg limits");
    scale.movingZeroAvg = movingZero;
  }
  scale.updateDisplay();
  delay(200);
}




float timer = millis();
void checkPower() {
  while (true) {
    //everything on
    scale.display.ssd1306_command(SSD1306_DISPLAYON);
    scale.scale.power_up();
    timer = millis();
    while ((millis() - timer) < 30000) {
      Serial.println("everything on");
      scale.Sense();
      scale.updateDisplay();
    }


    //screen off
    timer = millis();
    scale.display.ssd1306_command(SSD1306_DISPLAYOFF);
    while ((millis() - timer) < 30000) {
      scale.Sense();
      Serial.println("screen off, still sensing");
    }

    //screen and hx711 off
    timer = millis();
    scale.scale.power_down();
    while ((millis() - timer) < 30000) {
      Serial.println("everything off");
    }
  }
}
