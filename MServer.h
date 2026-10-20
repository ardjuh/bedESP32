#ifndef MSERVER_H
#define MSERVER_H

#include <WiFi.h>
#include <WebServer.h>
#include "MLight.h"
#include "password.h"

class MServer
{
private:
    MLight &light;
    WebServer server{80};

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
                    "ESP32 light server\nUse /light/on or /light/off.\n");
    }

    void handleNotFound()
    {
        server.send(404, "text/plain",
                    "Not found. Use /light/on or /light/off.\n");
    }

public:
    explicit MServer(MLight &light) : light(light) {}

    void begin()
    {

        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
        Serial.print("\nConnecting to ");
        Serial.println(ssid);

        while (WiFi.status() != WL_CONNECTED)
        {
            Serial.print(".");
            delay(100);
        }

        Serial.print("\nConnected to ");
        Serial.println(ssid);
        Serial.print("Local ESP32 IP: ");
        Serial.println(WiFi.localIP());
        Serial.print("WiFi gateway: ");
        Serial.println(WiFi.gatewayIP());
        Serial.print("WiFi subnet: ");
        Serial.println(WiFi.subnetMask());
        Serial.print("WiFi RSSI: ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");

        server.on("/", HTTP_GET, [this]() { handleRoot(); });
        server.on("/light/on", HTTP_GET, [this]() { handleLightOn(); });
        server.on("/light/off", HTTP_GET, [this]() { handleLightOff(); });
        server.on("/light/toggle", HTTP_GET, [this]() { handleLightToggle(); });
        server.onNotFound([this]() { handleNotFound(); });
        server.begin();
    }

    void loop()
    {
        server.handleClient();
    }
};

#endif