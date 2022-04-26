
#include "calibration2.h"


Scale scale("ver1") ;
\

void setup() {
  scale.scaleID = 'B';
  Serial.println("startup");
  scale.start_Up();
  Serial.println("left startup");
  scale.start_timer = millis();
  scale.start_up_menu();
}


void loop() {
  scale.getBallParkData();
 
  Serial.print("starting cal factor:");
  Serial.println(scale.lrCoef[0], 8);

  scale.manualFineTune();
  Serial.print("final cal factor: ");
  Serial.print(scale.calibration_factor);
}

/*
void manualFineTune() {
  Serial.println("Take off weight");
  delay(5000);
  Serial.println("Dont touch, taring");
  delay(4000);
  float calibration_factor = 100 * round(scale.lrCoef[0]/100.0);
  scale.scale.set_scale(calibration_factor);
  scale.scale.tare(); //Reset the scale to 0
  delay(4000);
  bool runTune = true;
  Serial.println("Place 20 g on scale when readings begin");
  while (runTune) {
    scale.scale.set_scale(calibration_factor);
    Serial.print("Reading: ");
    Serial.print(scale.scale.get_units());
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
