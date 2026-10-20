#include <WiFi.h>
#include <WebServer.h>
#include "password.h"

constexpr uint8_t LIGHT_PIN = LED_BUILTIN;
constexpr uint8_t LIGHT_ON_LEVEL = HIGH;
constexpr uint8_t LIGHT_OFF_LEVEL = LOW;

WebServer server(80);

void setLight(bool enabled) {
    digitalWrite(LIGHT_PIN, enabled ? LIGHT_ON_LEVEL : LIGHT_OFF_LEVEL);
}

void handleLightOn() {
    Serial.println("Request: /light/on");
    setLight(true);
    server.send(200, "text/plain", "Light on\n");
    Serial.println("Response: /light/on");
}

void handleLightOff() {
    Serial.println("Request: /light/off");
    setLight(false);
    server.send(200, "text/plain", "Light off\n");
    Serial.println("Response: /light/off");
}

void handleRoot() {
    server.send(200, "text/plain",
                "ESP32 light server\nUse /light/on or /light/off.\n");
}

void handleNotFound() {
    server.send(404, "text/plain",
                "Not found. Use /light/on or /light/off.\n");
}

void setup() {
    Serial.begin(115200);

    pinMode(LIGHT_PIN, OUTPUT);
    setLight(false);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("\nConnecting to ");
    Serial.println(ssid);

    while(WiFi.status() != WL_CONNECTED){
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

    server.on("/", HTTP_GET, handleRoot);
    server.on("/light/on", HTTP_GET, handleLightOn);
    server.on("/light/off", HTTP_GET, handleLightOff);
    server.onNotFound(handleNotFound);
    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    server.handleClient();
}