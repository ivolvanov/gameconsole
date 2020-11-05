/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-client-server-wi-fi/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

// Import required libraries
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <Arduino.h>

// Set your access point network credentials
const char *ssid = "ArcadeBrigade";
const char *password = "kingpong";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

void setup()
{
  // Serial port for debugging purposes
  Serial.begin(9600);
  Serial.println();

  // Setting the ESP as an access point
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/request", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", "response");
  });
  
  // Start server
  server.begin();
}

void loop()
{
}