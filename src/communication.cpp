#include <esp_now.h>
#include "communication.h"
#include "menu.h"
#include "pong.h"
#include "mafia.h"
#include "screen.h"

uint8_t pongSlaveMessage[2] = {'p', 's'};
uint8_t pongMasterMessage[2] = {'p', 'm'};
uint8_t mafiaNarratorMessage[2] = {'m', 'n'};
uint8_t mafiaJoinMessage[2] = {'m', 'j'};
uint8_t mafiaDisbandMessage[2] = {'m', 'd'};
uint8_t mafiaMafiosoMessage[2] = {'m', 'm'};
uint8_t mafiaCivilianMesssage[2] = {'m', 'c'};
uint8_t mafiaHealerMessage[2] = {'m', 'h'};
uint8_t mafiaPolicemanMessage[2] = {'m', 'p'};
uint8_t handshake = '!';
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

int unsucccessfulSends = 0;
char messageToBeSent[40];
char messageReceived[40];
uint8_t pongOpponent[6];
uint8_t mafiaPlayers[10][6];
int currentMafiaPlayers = 0;

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
  // handles communication for pong
  if (incomingData[0] == 'p')
  {
    if (state == MENU)
    {
      memcpy(pongOpponent, mac, 6);
      pong = SLAVE;
    }
    else if (state == PONG_WAITING)
    {
      memcpy(pongOpponent, mac, 6);
      state = PONG_PLAYING;
      if (incomingData[1] == 's')
      {
        esp_now_send(pongOpponent, (uint8_t *)&pongMasterMessage, sizeof(pongMasterMessage));
        pong = MASTER;
      }
      else
      {
        esp_now_send(pongOpponent, (uint8_t *)&pongSlaveMessage, sizeof(pongSlaveMessage));
        pong = SLAVE;
      }
    }
  }

  // handles communication for mafia
  else if (incomingData[0] == 'm')
  {
    bool doesPlayerExist = false;
    // add the player to the mafia players list
    for (int i = 0; i < currentMafiaPlayers; i++)
    {
      if (mafiaPlayers[i] == mac)
        doesPlayerExist = true;
    }
    if (!doesPlayerExist)
      memcpy(mafiaPlayers[currentMafiaPlayers++], mac, 6);

    switch (incomingData[1])
    {
    case 'n':
      if (mafiaRole == NARRATOR)
      {
        esp_now_send(broadcastAddress, (uint8_t *)&mafiaDisbandMessage, sizeof(mafiaDisbandMessage));

        mafiaRole = NONE;
        state = MENU;
        printTitle(games[currentGame], 0);
      }
      break;
    case 'd':
      mafiaRole = NONE;
      state = MENU;
      printTitle(games[currentGame], 0);
      break;
    case 'm':
      mafiaRole = MAFIOSO;
      state = MAFIA_PLAYING;
      break;
    case 'c':
      mafiaRole = CIVILIAN;
      state = MAFIA_PLAYING;
      break;
    case 'h':
      mafiaRole = HEALER;
      state = MAFIA_PLAYING;
      break;
    case 'p':
      mafiaRole = POLICEMAN;
      state = MAFIA_PLAYING;
      break;
    }
  }
  else
  {
    memcpy(&messageReceived, incomingData, len);
  }
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  if (status == ESP_NOW_SEND_FAIL)
  {
    if (state == PONG_PLAYING)
      unsucccessfulSends++;

    if (unsucccessfulSends >= 3)
    {
      state = MENU;
      pong = MASTER;
    }
  }
  else
  {
    unsucccessfulSends = 0;
  }
}