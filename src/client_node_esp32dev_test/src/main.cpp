/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-client-server-wi-fi/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <WiFi.h>
#include <Arduino.h>
//#include "ESPAsyncWebServer.h"
#include <HTTPClient.h>

const char *ssid = "ArcadeBrigade";
const char *password = "kingpong";

//Your IP address or domain name with URL path
const char *serverName = "http://192.168.4.1/request";

#include <Wire.h>

String response;

unsigned long previousMillis = 0, time_wifi_initialized = 0;
long interval = 20;
int successful_requests = 0, failed_requests = 0;

String httpGETRequest(const char *serverName);

void setup()
{
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  time_wifi_initialized = millis();
}

void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    // Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED)
    {
      response = httpGETRequest(serverName);
      if (successful_requests == 100)
      {
        interval = 10000000;
        Serial.print("Time taken for 100 requests: ");
        Serial.print(millis() - time_wifi_initialized);
        Serial.println("ms.");
        Serial.print("Requests failed: ");
        Serial.println(failed_requests);
      }

      // save the last HTTP GET Request
      previousMillis = currentMillis;
    }
    else
    {
      Serial.println("WiFi Disconnected");
    }
  }
}

String httpGETRequest(const char *serverName)
{
  HTTPClient http;

  // Your IP address with path or Domain name with URL path
  http.begin(serverName);

  // Send HTTP POST request
  int httpResponseCode = http.GET();

  String payload = "--";

  if (httpResponseCode > 0)
  {
    //Serial.print("HTTP Response code: ");
    //Serial.println(httpResponseCode);
    payload = http.getString();
    successful_requests++;
  }
  else
  {
    failed_requests++;
    //Serial.print("Error code: ");
    //Serial.println(httpResponseCode);
  }
  http.end();

  return payload;
}