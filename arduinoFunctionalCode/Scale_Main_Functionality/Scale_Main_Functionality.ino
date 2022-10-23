#include "Scale_Main_Functionality.h"


Scale scale("ver1") ;


//states for swti
enum State_enum {mouseIn, waitBuffer, getZero};
uint8_t state = waitBuffer;

void setup() {
  Serial.println("startup");

  scale.startUp();
  Serial.println("left startup");
  scale.proxLimitforMouseIn = 90 ;
 

  Serial.println("entering menu");
  scale.start_up_menu();
  scale.start_timer = millis();
  scale.scale.set_scale(scale.calibration_factor);
  // scale.autoCalibrate();
  scale.taring_timer = millis();
}

void loop() {
  scale.Sense();
  switch (state) {

    case mouseIn:
      Serial.println("mouse in state");
      scale.Sense();
      scale.mouseHere_timer = millis();
      scale.mouseInHouse = true;
      scale.WriteToSD();
      scale.updateDisplay();
      //changed to C grams
      scale.calcMovingAvg(scale.cgrams);
      //changed from 100 t0 200
      delay(100);
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
      //changed to explore how many data points we need 
      delay(500);

      if ((millis() - scale.taring_timer) > 1000000) {
        scale.display.ssd1306_command(SSD1306_DISPLAYOFF);
      }
      break;

    // add escape state if havent been zerod in long time


    case getZero:
      Serial.println("getting a zero");
      scale.withinLimits = true;
      if (scale.range < 60) {
        state = mouseIn;
      }
      scale.calcZeroAvg(); 
      //state = waitBuffer;
      break;


  }
  scale.ReadBatteryLevel();
}
