#define DBG_OUTPUT_PORT Serial

#include "CL_App.h"

CL_App App;

#define APPLICATION App

void setup() {
//  pinMode(D0, OUTPUT);
//  digitalWrite(D0, LOW);
    App.setup();
}

void loop() {
  // put your main code here, to run repeatedly:
  App.loop();
}
