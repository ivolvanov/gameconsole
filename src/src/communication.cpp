#include <esp_now.h>
#include "communication.h"
#include "menu.h"
#include "pong.h"

uint8_t pongSlaveMessage[2] = {'p', 's'};
uint8_t pongMasterMessage[2] = {'p', 'm'};
uint8_t handshake = '!';
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

char messageToBeSent[40];
char messageReceived[40];
uint8_t opponent[6];

// this is a bit of a thick boi, so far it hasn't caused any problems but keep in mind that if expanded further it may cause issues
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  // if the incoming data is from an unknown address, check if the device is already on the list and add it
  if (!esp_now_is_peer_exist(mac))
  {
    esp_now_peer_info_t newPeer;
    memcpy(newPeer.peer_addr, mac, 6);
    newPeer.channel = 0;
    newPeer.encrypt = false;
    newPeer.ifidx = ESP_IF_WIFI_STA;

    esp_now_add_peer(&newPeer);

    esp_now_send(broadcastAddress, &handshake, sizeof(handshake)); // also send back current device address
  }
  if (incomingData[0] == 'p')
  {
    if (state == MENU)
    {
      memcpy(opponent, mac, 6);
      pong = SLAVE;
      Serial.println("Pong changed to SLAVE.");
    }
    else if (state == PONG_WAITING)
    {
      memcpy(opponent, mac, 6);
      state = PLAYING;
      Serial.println("State changed to PLAYING.");
      if (incomingData[1] == 's')
      {
        esp_now_send(opponent, (uint8_t *)&pongMasterMessage, sizeof(pongMasterMessage));
        pong = MASTER;
      }
      else
      {
        esp_now_send(opponent, (uint8_t *)&pongSlaveMessage, sizeof(pongSlaveMessage));
        pong = SLAVE;
        Serial.println("Pong changed to SLAVE.");
      }
    }
  }
  else
  {
    memcpy(&messageReceived, incomingData, len);
  }
}