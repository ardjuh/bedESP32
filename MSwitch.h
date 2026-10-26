#ifndef MSWITCH_H
#define MSWITCH_H

#include "MLight.h"

constexpr uint8_t GPIO1_PIN = 1;
int lastGpio1State = LOW;
unsigned long lastGpio1Read = 0;

class MSwitch
{
private:
    MLight &light;

public:
    explicit MSwitch(MLight &light) : light(light) {}

    void begin()
    {
        pinMode(GPIO1_PIN, INPUT_PULLDOWN);
    }

    void loop()
    {
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
};

#endif