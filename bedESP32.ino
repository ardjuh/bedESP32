#include <WiFi.h>
#include <WebServer.h>
#include "password.h"
#include "MLight.h"

WebServer server(80);
MLight light;

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

void setup()
{
  Serial.begin(115200);

  light.begin();

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

  server.on("/", HTTP_GET, handleRoot);
  server.on("/light/on", HTTP_GET, handleLightOn);
  server.on("/light/off", HTTP_GET, handleLightOff);
  server.on("/light/toggle", HTTP_GET, handleLightToggle);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop()
{
  server.handleClient();
}