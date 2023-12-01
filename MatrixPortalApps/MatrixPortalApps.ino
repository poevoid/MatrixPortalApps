#include <Wire.h>                  // For I2C communication
#include <Adafruit_LIS3DH.h>       // For accelerometer
#include "Adafruit_SHTC3.h" //temp/humidity
#include "Adafruit_SGP40.h"//voc/mox
#include <Adafruit_Protomatter.h>  // For RGB matrix

#include <NTPClient.h>
#include <WiFiNINA.h>
//#include <Adafruit_PixelDust.h>
#include "func.h"


void setup() {
  // put your setup code here, to run once:
  ProtomatterStatus status = matrix.begin();
  if (!accel.begin(0x19)) {
    Serial.println("Couldn't find accelerometer");
    err(250);  // Fast bink = I2C error
  }
  accel.setRange(LIS3DH_RANGE_2_G);  // 2, 4, 8 or 16 G!
  accel.setClick(2, CLICKTHRESHHOLD);
  
#if defined(BACK_BUTTON)
  pinMode(BACK_BUTTON, INPUT_PULLUP);
#endif
#if defined(NEXT_BUTTON)
  pinMode(NEXT_BUTTON, INPUT_PULLUP);
#endif
}

void loop() {
  // put your main code here, to run repeatedly:
  switch (app) {
    case Application::Pong:
      playPong();
      if (!digitalRead(NEXT_BUTTON)) {
        app = Application::Environs;
      }
      if (!digitalRead(BACK_BUTTON)) {
        app = Application::Cistercia;
      }
      //if "next" button pressed, change apps.
      break;

    case Application::Environs:
      monitorEnvironment();
      if (!digitalRead(NEXT_BUTTON)) {
        app = Application::Cistercia;
      }
      if (!digitalRead(BACK_BUTTON)) {
        app = Application::Pong;
      }
      break;

    case Application::Cistercia:
      //do later
      cistercianClock();
      if (!digitalRead(BACK_BUTTON)) {
          app = Application::Environs;
      }
      break;
  }
}
