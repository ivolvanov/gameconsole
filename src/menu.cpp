#include <esp_now.h>
#include "menu.h"
#include "joystick.h"
#include "communication.h"
#include "screen.h"
#include "pong.h"
#include "mafia.h"

State state = MENU;
int currentGame = 0;

String games[] = {"Pong", "Mafia"};

void menu()
{
  if (state == MENU)
  {
    handleMenuState();
  }

  else if (state == MAFIA_WAITING && mafiaRole == NARRATOR)
  {
    if (isButtonPressed)
    {
      isButtonPressed = !isButtonPressed;
      state = MAFIA_PLAYING;
    }
  }

  else if (state == MAFIA_WAITING && mafiaRole != NARRATOR)
  {
    esp_now_send(narrator, (uint8_t *)&handshake, sizeof(handshake));
  }
}

void handleMenuState()
{
  if (isButtonPressed)
  {
    isButtonPressed = !isButtonPressed;
    
    // pong entered
    if (currentGame == 0)    
      handlePong();    

    // mafia entered
    else if (currentGame == 1)    
      handleMafia();    
  }

  else if (currentPos != MIDDLE && previousPos == MIDDLE)
  {
    currentGame = !currentGame;
    previousPos = currentPos;
    printTitle(games[currentGame], 0);
  }
  else if (currentPos == MIDDLE)
  {
    previousPos = MIDDLE;
  }
}

void handlePong()
{
  if (pong == MASTER)
  {
    printWaitingForOpponent();
    esp_now_send(broadcastAddress, (uint8_t *)&pongMasterMessage, sizeof(pongMasterMessage));
    state = PONG_WAITING;
  }
  else
  {
    esp_now_send(pongOpponent, (uint8_t *)&pongSlaveMessage, sizeof(pongSlaveMessage));
    state = PONG_PLAYING;
  }
}

void handleMafia()
{
  if (nrOfMafiaPlayers == 0)
  {
    esp_now_send(broadcastAddress, (uint8_t *)&mafiaNarratorMessage, sizeof(mafiaNarratorMessage));
    mafiaRole = NARRATOR;
    printPressToStart();
  }
  else
  {
    esp_now_send(broadcastAddress, (uint8_t *)&mafiaJoinMessage, sizeof(mafiaJoinMessage));
    printWaitingForNarrator();
  }
  state = MAFIA_WAITING;
}