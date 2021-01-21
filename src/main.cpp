#include <esp_now.h>
#include <WiFi.h>
#include "joystick.h"
#include "screen.h"
#include "pong.h"
#include "communication.h"
#include "menu.h"
#include "mafia.h"

void setup()
{
  Serial.begin(115200);
  delay(100);

  Wire.begin();

  initializeScreen();

  while (joystick.begin() == false)
  {
    Serial.println("Joystick does not appear to be connected. Please check wiring.");
    delay(100);
  }

  randomSeed(analogRead(A0));

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  while (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    delay(100);
  }

  // the broadcast address needs to be added as a peer
  esp_now_peer_info_t broadcastDevice;
  memcpy(broadcastDevice.peer_addr, broadcastAddress, 6);
  broadcastDevice.channel = 0;
  broadcastDevice.encrypt = false;

  // add the broadcast device (does not check whether adding was a success)
  esp_now_add_peer(&broadcastDevice);

  // Register for a callback function that will be called when data is received/sent
  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent);

  esp_now_send(broadcastAddress, &handshake, 1);

  // show the initial menu screen
  printTitle(games[currentGame], 0);
}

void loop()
{
  getJoystickInput();
  menu();

  if (state == PONG_PLAYING)
    pongGame();

  else if (state == MAFIA_PLAYING)
    mafiaGame();
}