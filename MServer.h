#ifndef MSERVER_H
#define MSERVER_H

#include <WiFi.h>
#include <WebServer.h>
#include <cstdlib>
#include "MLight.h"
#include "password.h"

class MServer
{
private:
    static constexpr unsigned long RECONNECT_INTERVAL_MS = 10000;

    MLight &light;
    WebServer server{80};
    unsigned long lastReconnectAttempt = 0;
    bool wasConnected = false;

    void handleLightOn()
    {
        Serial.println("Request: /light/on");
        light.on();
        server.send(200, "text/plain", "Light on\n");
    }

    void handleLightOff()
    {
        Serial.println("Request: /light/off");
        light.off();
        server.send(200, "text/plain", "Light off\n");
    }

    void handleLightToggle()
    {
        Serial.println("Request: /light/toggle");
        light.toggle();
        server.send(200, "text/plain", light.isOn() ? "Light on\n" : "Light off\n");
    }

    void handleRoot()
    {
        server.send(200, "text/plain",
                    "ESP32 light server\n"
                    "Set brightness: /light/brightness?value=0..255\n"
                    "Set RGB: /light/rgb?r=0..255&g=0..255&b=0..255\n");
    }

    void getLightState()
    {
        server.send(200, "text/plain", light.isOn() ? "ON\n" : "OFF\n");
    }

    void handleLightBrightness()
    {
        if (!server.hasArg("value"))
        {
            server.send(400, "text/plain",
                        "Missing brightness value; use /light/brightness?value=0..255\n");
            return;
        }

        const String value = server.arg("value");
        char *end = nullptr;
        const long brightness = strtol(value.c_str(), &end, 10);
        if (value.isEmpty() || end == value.c_str() || *end != '\0' ||
            brightness < 0 || brightness > 255)
        {
            server.send(400, "text/plain",
                        "Brightness must be an integer from 0 to 255\n");
            return;
        }

        light.setBrightness(static_cast<uint8_t>(brightness));
        server.send(200, "text/plain",
                    "Brightness: " + String(light.getBrightness()) + "\n");
    }

    void handleLightRgb()
    {
        if (!server.hasArg("r") || !server.hasArg("g") || !server.hasArg("b"))
        {
            server.send(400, "text/plain",
                        "Missing RGB value; use /light/rgb?r=0..255&g=0..255&b=0..255\n");
            return;
        }

        const String redValue = server.arg("r");
        const String greenValue = server.arg("g");
        const String blueValue = server.arg("b");
        char *redEnd = nullptr;
        char *greenEnd = nullptr;
        char *blueEnd = nullptr;
        const long red = strtol(redValue.c_str(), &redEnd, 10);
        const long green = strtol(greenValue.c_str(), &greenEnd, 10);
        const long blue = strtol(blueValue.c_str(), &blueEnd, 10);
        if (redValue.isEmpty() || redEnd == redValue.c_str() || *redEnd != '\0' ||
            greenValue.isEmpty() || greenEnd == greenValue.c_str() || *greenEnd != '\0' ||
            blueValue.isEmpty() || blueEnd == blueValue.c_str() || *blueEnd != '\0' ||
            red < 0 || red > 255 || green < 0 || green > 255 || blue < 0 || blue > 255)
        {
            server.send(400, "text/plain",
                        "RGB values must each be integers from 0 to 255\n");
            return;
        }

        light.setRgb(static_cast<uint8_t>(red), static_cast<uint8_t>(green),
                     static_cast<uint8_t>(blue));
        server.send(200, "text/plain",
                    "RGB: " + String(red) + "," + String(green) + "," + String(blue) + "\n");
    }

    void handleNotFound()
    {
        server.send(404, "text/plain",
                    "Not found :(\n");
    }

public:
    explicit MServer(MLight &light) : light(light) {}

    void begin()
    {
        WiFi.mode(WIFI_STA);
        WiFi.setAutoReconnect(true);
        WiFi.begin(ssid, password);
        lastReconnectAttempt = millis();
        Serial.print("\nConnecting to ");
        Serial.println(ssid);
        Serial.print("Local ESP32 IP: ");
        Serial.println(WiFi.localIP());

        server.on("/", HTTP_GET, [this]()
                  { handleRoot(); });
        server.on("/light/on", HTTP_GET, [this]()
                  { handleLightOn(); });
        server.on("/light/off", HTTP_GET, [this]()
                  { handleLightOff(); });
        server.on("/light/toggle", HTTP_GET, [this]()
                  { handleLightToggle(); });
        server.on("/light/state", HTTP_GET, [this]()
                  { getLightState(); });
        server.on("/light/brightness", HTTP_GET, [this]()
                  { handleLightBrightness(); });
        server.on("/light/rgb", HTTP_GET, [this]()
                  { handleLightRgb(); });
        server.onNotFound([this]()
                          { handleNotFound(); });
        server.begin();
    }

    void loop()
    {
        server.handleClient();

        const bool connected = WiFi.status() == WL_CONNECTED;
        if (connected && !wasConnected)
        {
            wasConnected = true;
            Serial.print("\nConnected to ");
            Serial.println(ssid);
            Serial.print("Local ESP32 IP: ");
            Serial.println(WiFi.localIP());
        }
        else if (!connected && wasConnected)
        {
            wasConnected = false;
            Serial.println("\nWiFi connection lost");
        }

        const unsigned long now = millis();
        if (!connected && now - lastReconnectAttempt >= RECONNECT_INTERVAL_MS)
        {
            lastReconnectAttempt = now;
            Serial.println("Retrying WiFi connection...");
            WiFi.disconnect(false, false);
            WiFi.begin(ssid, password);
        }
    }
};

#endif