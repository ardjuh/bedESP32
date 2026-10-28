#ifndef MLIGHT_H
#define MLIGHT_H
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <time.h>
#include <math.h>

constexpr uint8_t LIGHT_PIN = 2;
constexpr uint16_t LIGHT_PIXEL_COUNT = 120;
constexpr uint8_t LIGHT_BRIGHTNESS = 50;
constexpr float NETHERLANDS_LATITUDE = 52.3676f;
constexpr float NETHERLANDS_LONGITUDE = 4.9041f;

class MLight
{
private:
    bool lightState = false;
    uint32_t currentColor = 0;
    uint8_t currentBrightness = LIGHT_BRIGHTNESS;
    int lastScheduleMinute = -1;
    Adafruit_NeoPixel strip{
        LIGHT_PIXEL_COUNT, LIGHT_PIN, NEO_GRB + NEO_KHZ800};

    void setColor(uint32_t color)
    {
        if (currentColor == color)
        {
            return;
        }
        strip.fill(color);
        strip.show();
        currentColor = color;
    }

    void setLight(bool enabled)
    {
        lightState = enabled;
        lastScheduleMinute = -1;
        if (!enabled)
        {
            setColor(0);
            return;
        }
        updateColorByTime();
    }

    void updateColorByTime()
    {
        if (!lightState)
        {
            return;
        }

        tm localTime;
        if (!getLocalTime(&localTime, 0))
        {
            // Fall back to a neutral white light when the clock is not available yet.
            // Otherwise the light stays logically "on" but never actually emits any color.
            if (currentColor == 0)
            {
                setColor(strip.Color(255, 255, 255));
            }
            return;
        }

        const int currentMinute = localTime.tm_hour * 60 + localTime.tm_min;
        if (currentMinute == lastScheduleMinute)
        {
            return;
        }
        lastScheduleMinute = currentMinute;

        uint32_t color;
        if (currentMinute < 12 * 60)
        {
            color = strip.Color(128, 128, 255);
        }
        else if (currentMinute >= 22 * 60)
        {
            color = strip.Color(255, 0, 0);
        }
        else if (currentMinute >= 19 * 60)
        {
            color = strip.Color(255, 70, 70);
        }
        else
        {
            color = strip.Color(255, 255, 255);
        }
        setColor(color);
    }

public:
    MLight() = default;
    void begin()
    {
        strip.begin();
        strip.setBrightness(currentBrightness);
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

    void setBrightness(uint8_t brightness)
    {
        currentBrightness = brightness;
        strip.setBrightness(brightness);
        strip.show();
    }

    void setRgb(uint8_t red, uint8_t green, uint8_t blue)
    {
        setLight(true);
        setColor(strip.Color(red, green, blue));
    }

    uint8_t getBrightness() const
    {
        return currentBrightness;
    }
};

#endif