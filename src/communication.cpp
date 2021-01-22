#include <esp_now.h>
#include "communication.h"
#include "menu.h"
#include "pong.h"
#include "mafia.h"
#include "screen.h"

uint8_t pongSlaveMessage[2] = {'p', 's'};
uint8_t pongMasterMessage[2] = {'p', 'm'};
uint8_t pongFoundMessage[2] = {'p', 'f'};

uint8_t mafiaNarratorMessage[2] = {'m', 'n'};
uint8_t mafiaJoinMessage[2] = {'m', 'j'};
uint8_t mafiaDisbandMessage[2] = {'m', 'd'};
uint8_t mafiaMafiosoMessage[2] = {'m', 'm'};
uint8_t mafiaCivilianMesssage[2] = {'m', 'c'};
uint8_t mafiaHealerMessage[2] = {'m', 'h'};
uint8_t mafiaPolicemanMessage[2] = {'m', 'p'};

uint8_t handshake = '!'; // random message to exahnge mac and presence
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

int unsuccessfulSendsMafia = 0;
int unsuccessfulSendsPong = 0;
char messageToBeSent[40]; // buffer to store messages that will be sent
char messageReceived[40]; // buffer to store received messages

uint8_t pongOpponent[6]; // mac address of the current pong opponent
uint8_t narrator[6];     // mac address of the narrator
uint8_t mafiaPlayers[10][6];

int nrOfMafiaPlayers = 0;

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
    pongDataRecv(mac, incomingData, len);
  }

  // handles communication for mafia
  else if (incomingData[0] == 'm')
  {
    mafiaDataRecv(mac, incomingData, len);
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
    // differentiate between pong disconnect and mafia disconnect
    if (state == PONG_PLAYING)
      unsuccessfulSendsPong++;

    else if (state == MAFIA_WAITING || state == MAFIA_PLAYING)
      unsuccessfulSendsMafia++;

    if (unsuccessfulSendsPong >= 3)
    {
      state = MENU;
      pong = MASTER;
    }
    if (unsuccessfulSendsMafia >= 3)
    {
      resetMafia();
    }
  }
  else
  {
    unsuccessfulSendsPong = 0;
    unsuccessfulSendsMafia = 0;
  }
}

void pongDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  if (state == MENU)
  {
    // the first pong-related message received while in menu is the assumed opponent
    // f means that a game between another pair of players was found, so we have no opponent again
    if (incomingData[1] != 'f')
    {
      memcpy(pongOpponent, mac, 6);
      pong = SLAVE;
    }
    else
    {
      pong = MASTER;
      memset(&pongOpponent, 0, 6);
    }
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

void mafiaDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  bool doesPlayerExist = false;
  // add the player to the mafia players list
  for (int i = 0; i < nrOfMafiaPlayers; i++)
  {
    if (mafiaPlayers[i] == mac)
      doesPlayerExist = true;
  }
  if (!doesPlayerExist)
    memcpy(mafiaPlayers[nrOfMafiaPlayers++], mac, 6);

  // handle all possible mafia messages
  switch (incomingData[1])
  {
  case 'n':
    // if the narrator receives a message from another node that they want to be the narrator
    // neither of them is the narrator and the game is reset
    if (mafiaRole == NARRATOR)
      resetMafia();
    else
      memcpy(narrator, mac, 6);
    break;
  case 'd':
    if (state == MAFIA_PLAYING || state == MAFIA_WAITING || state == MENU)
    {
      state = MENU;
      mafiaRole = NONE;
      printTitle(games[currentGame], 0);
    }
    nrOfMafiaPlayers = 0;
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