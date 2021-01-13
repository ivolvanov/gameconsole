#include <esp_now.h>
#include "menu.h"
#include "joystick.h"
#include "communication.h"
#include "screen.h"
#include "pong.h"

State state = MENU;
int currentGame = 0;

String games[] = {"Pong", "Mafia"};

void menu()
{
  if (state != PONG_WAITING)
  {
    if (isButtonPressed && currentGame == 0)
    {
      isButtonPressed = !isButtonPressed;
      if (pong == MASTER)
      {
        printWaitingForOpponent();
        esp_now_send(broadcastAddress, (uint8_t *)&pongMasterMessage, sizeof(pongMasterMessage));
        state = PONG_WAITING;
      }
      else
      {
        esp_now_send(opponent, (uint8_t *)&pongSlaveMessage, sizeof(pongSlaveMessage));
        state = PLAYING;
      }
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
}