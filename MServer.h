#ifndef MSERVER_H
#define MSERVER_H

#include <WiFi.h>
#include <WebServer.h>
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
                    "ESP32 light server\n");
    }

    void getLightState()
    {
        server.send(200, "text/plain", light.isOn() ? "ON\n" : "OFF\n");
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