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
  configTzTime("CET-1CEST,M3.5.0,M10.5.0/3",
               "pool.ntp.org", "time.nist.gov");
  mLight.begin();
  mSwitch.begin();
  mServer.begin();
}

void loop()
{
  mServer.loop();
  mSwitch.loop();
}