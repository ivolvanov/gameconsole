/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-two-way-communication-esp32/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/
#include <Arduino.h>

#include <esp_now.h>
#include <WiFi.h>

// REPLACE WITH THE MAC Address of your receiver
uint8_t esp1[] = {0x24, 0x0A, 0xC4, 0x62, 0x5B, 0x08};
uint8_t esp2[] = {0x30, 0xAE, 0xA4, 0x6B, 0x92, 0x4C};
uint8_t esp3[] = {0x24, 0x0A, 0xC4, 0x60, 0xF1, 0x5C};

// Variable to store if sending data was successful
String success;

char messageToBeSent[40] = "This is a message from ESP 3";
char messageReceived[40];

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status == 0)
  {
    success = "Delivery Success :)";
  }
  else
  {
    success = "Delivery Fail :(";
  }
}

// Callback when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&messageReceived, incomingData, len);
  Serial.print("Number received: ");
  Serial.println(messageReceived);
}

void setup()
{
  // Init Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register peer
  esp_now_peer_info_t esp1Peer;
  memcpy(esp1Peer.peer_addr, esp1, 6);
  esp1Peer.channel = 0;
  esp1Peer.encrypt = false;

  esp_now_peer_info_t esp2Peer;
  memcpy(esp2Peer.peer_addr, esp2, 6);
  esp2Peer.channel = 0;
  esp2Peer.encrypt = false;

  // // Add peer
  if (esp_now_add_peer(&esp1Peer) != ESP_OK)
  {
    Serial.println("Failed to add peer ESP1");
    return;
  }

  if (esp_now_add_peer(&esp2Peer) != ESP_OK)
  {
    Serial.println("Failed to add peer ESP2");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}

void loop()
{

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(esp1, (uint8_t *)&messageToBeSent, sizeof(messageToBeSent));
  //esp_now_send(esp2, (uint8_t *)&messageToBeSent, sizeof(messageToBeSent));

  // if (result == ESP_OK)
  // {
  //   Serial.println("Sent with success");
  // }
  // else
  // {
  //   Serial.println("Error sending the data");
  // }
  delay(10);
}
