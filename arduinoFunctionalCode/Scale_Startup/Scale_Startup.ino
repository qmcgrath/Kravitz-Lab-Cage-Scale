#include "Scale_Startup.h"

Scale scale("ver1") ;


//states for swti
enum State_enum {mouseIn, waitBuffer, getZero};
uint8_t state = waitBuffer;

void setup() {
  Serial.println("startup");

  scale.startUp();
  Serial.println("left startup");
  Serial.println("entering menu");
  scale.start_up_menu();
  scale.start_timer = millis();
  scale.scale.set_scale(scale.calibration_factor);

}

void loop() {
}
