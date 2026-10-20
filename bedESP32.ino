#include "password.h"
#include "MLight.h"
#include "MServer.h"
#include "MSwitch.h"

MLight mLight;
MServer mServer(mLight);
MSwitch mSwitch(mLight);

void setup()
{
  Serial.begin(115200);
  pinMode(GPIO1_PIN, INPUT);
  mLight.begin();
  mSwitch.begin();
  mServer.begin();
}

void loop()
{
  mServer.loop();
  mSwitch.loop();
}