#ifndef MLIGHT_H
#define MLIGHT_H
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <time.h>
#include <math.h>

constexpr uint8_t LIGHT_PIN = 2;
constexpr uint16_t LIGHT_FIRST_PIXEL = 100;
constexpr uint16_t LIGHT_PIXEL_COUNT = 11;
constexpr uint16_t LIGHT_STRIP_PIXEL_COUNT =
    LIGHT_FIRST_PIXEL + LIGHT_PIXEL_COUNT;
constexpr uint8_t LIGHT_BRIGHTNESS = 128;
constexpr float NETHERLANDS_LATITUDE = 52.3676f;
constexpr float NETHERLANDS_LONGITUDE = 4.9041f;

class MLight
{
private:
    bool lightState = false;
    uint32_t currentColor = 0;
    int lastScheduleMinute = -1;
    Adafruit_NeoPixel strip{
        LIGHT_STRIP_PIXEL_COUNT, LIGHT_PIN, NEO_GRB + NEO_KHZ800};

    void setColor(uint32_t color)
    {
        if (currentColor == color)
        {
            return;
        }
        for (uint16_t pixel = LIGHT_FIRST_PIXEL;
             pixel < LIGHT_FIRST_PIXEL + LIGHT_PIXEL_COUNT; ++pixel)
        {
            strip.setPixelColor(pixel, color);
        }
        strip.show();
        currentColor = color;
    }

    // Magic
    int sunsetMinute(const tm &localTime) const
    {
        const int dayOfYear = localTime.tm_yday + 1;
        const float longitudeHour = NETHERLANDS_LONGITUDE / 15.0f;
        const float timeApproximation =
            dayOfYear + (18.0f - longitudeHour) / 24.0f;
        const float solarMeanAnomaly =
            (0.9856f * timeApproximation) - 3.289f;
        const float solarLongitude = fmod(
            solarMeanAnomaly + 1.916f * sin(solarMeanAnomaly * DEG_TO_RAD) +
                0.020f * sin(2.0f * solarMeanAnomaly * DEG_TO_RAD) + 282.634f,
            360.0f);
        const float rawRightAscension =
            atan(0.91764f * tan(solarLongitude * DEG_TO_RAD)) / DEG_TO_RAD;
        const float longitudeQuadrant =
            floor(solarLongitude / 90.0f) * 90.0f;
        const float rightAscensionQuadrant =
            floor(rawRightAscension / 90.0f) * 90.0f;
        const float rightAscension = fmod(
            rawRightAscension + longitudeQuadrant - rightAscensionQuadrant,
            360.0f);
        const float rightAscensionHours =
            rightAscension / 15.0f;
        const float declinationSin =
            0.39782f * sin(solarLongitude * DEG_TO_RAD);
        const float declinationCos =
            cos(asin(declinationSin));
        const float cosHourAngle =
            (cos(90.833f * DEG_TO_RAD) -
             sin(NETHERLANDS_LATITUDE * DEG_TO_RAD) * declinationSin) /
            (cos(NETHERLANDS_LATITUDE * DEG_TO_RAD) * declinationCos);

        if (cosHourAngle >= 1.0f)
        {
            return 0;
        }
        if (cosHourAngle <= -1.0f)
        {
            return 24 * 60;
        }

        const float hourAngle = acos(cosHourAngle) / DEG_TO_RAD / 15.0f;
        const float localMeanTime =
            hourAngle + rightAscensionHours -
            0.06571f * timeApproximation - 6.622f;
        const int daylightSavingOffset = localTime.tm_isdst > 0 ? 120 : 60;
        int localMinutes = static_cast<int>(
            round((localMeanTime - longitudeHour) * 60.0f +
                  daylightSavingOffset));
        return (localMinutes + 24 * 60) % (24 * 60);
    }

    void setLight(bool enabled)
    {
        lightState = enabled;
        if (!enabled)
        {
            lastScheduleMinute = -1;
            setColor(0);
            return;
        }
        lastScheduleMinute = -1;
        updateColorByTime();
        Serial.print("Light state changed to: ");
        Serial.println(lightState ? "ON" : "OFF");
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
            return;
        }

        const int currentMinute = localTime.tm_hour * 60 + localTime.tm_min;
        if (currentMinute == lastScheduleMinute)
        {
            return;
        }
        lastScheduleMinute = currentMinute;

        const int sunset = sunsetMinute(localTime);
        uint32_t color;
        if (currentMinute < 12 * 60)
        {
            color = strip.Color(0, 0, 255);
        }
        else if (currentMinute >= sunset || currentMinute >= 22 * 60)
        {
            color = strip.Color(255, 0, 0);
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
        strip.setBrightness(LIGHT_BRIGHTNESS);
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