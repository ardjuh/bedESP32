#include "password.h"
#include "MLight.h"
#include "MServer.h"

MLight light;
MServer server(light);

constexpr uint8_t GPIO1_PIN = 1;
int lastGpio1State = LOW;
unsigned long lastGpio1Read = 0;

void setup()
{
  Serial.begin(115200);
  pinMode(GPIO1_PIN, INPUT);
  light.begin();
  server.begin();
}

void loop()
{
  server.loop();

  const unsigned long now = millis();
  if (now - lastGpio1Read >= 100)
  {
    lastGpio1Read = now;
    const int gpio1State = digitalRead(GPIO1_PIN);
    if (gpio1State != lastGpio1State)
    {
      lastGpio1State = gpio1State;
      light.toggle();
      Serial.print("GPIO1: ");
      Serial.println(gpio1State == HIGH ? "HIGH" : "LOW");
    }
  }
}