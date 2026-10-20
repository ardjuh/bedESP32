#ifndef MLIGHT_H
#define MLIGHT_H
#include <Arduino.h>

constexpr int8_t LIGHT_PIN = LED_BUILTIN;
constexpr uint8_t LIGHT_ON_LEVEL = LOW;
constexpr uint8_t LIGHT_OFF_LEVEL = HIGH;

class MLight
{
private:
    bool lightState = false;

    void setLight(bool enabled)
    {
        digitalWrite(LIGHT_PIN, enabled ? LIGHT_ON_LEVEL : LIGHT_OFF_LEVEL);
        lightState = enabled;
        Serial.print("Light state changed to: ");
        Serial.println(lightState ? "ON" : "OFF");
    }

public:
    MLight() = default;
    void begin()
    {
        pinMode(LIGHT_PIN, OUTPUT);
        setLight(false);
    }

    void on()
    {
        setLight(true);
    }

    void off()
    {
        setLight(false);
    }

    void toggle()
    {
        setLight(!lightState);
    }

    bool isOn() const
    {
        return lightState;
    }
};

#endif