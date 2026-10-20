#include "password.h"
#include "MLight.h"
#include "MServer.h"

MLight light;
MServer server(light);

void setup()
{
  Serial.begin(115200);
  light.begin();
  server.begin();
}

void loop()
{
  server.loop();
}