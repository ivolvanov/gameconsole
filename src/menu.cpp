#include <esp_now.h>
#include "menu.h"
#include "joystick.h"
#include "communication.h"
#include "screen.h"
#include "pong.h"

State state = MENU;
int currentGame = 0;

String games[] = {"Pong", "Mafia"};

void changeGame()
{
  if (state != PONG_WAITING)
  {
    if (isButtonPressed && currentGame == 0)
    {
      isButtonPressed = !isButtonPressed;
      if (pong == MASTER)
      {
        int middleX = screen.getLCDWidth() / 2;
        int middleY = screen.getLCDHeight();

        screen.clear(PAGE);
        screen.setFontType(0);
        screen.setCursor(middleX - (screen.getFontWidth() * (strlen("Waiting") / 2)),
                         middleY * 0.25 - (screen.getFontWidth() / 2));
        screen.print("Waiting");
        screen.setCursor(middleX - (screen.getFontWidth() * (strlen("for") / 2)),
                         middleY * 0.5 - (screen.getFontWidth() / 2));
        screen.print("for");
        screen.setCursor(middleX - (screen.getFontWidth() * (strlen("opponent") / 2)),
                         middleY * 0.75 - (screen.getFontWidth() / 2));
        screen.print("opponent");
        screen.display();

        esp_now_send(broadcastAddress, (uint8_t *)&pongMasterMessage, sizeof(pongMasterMessage));
        state = PONG_WAITING;
        Serial.println("State changed to PONG_WAITING.");
      }
      else
      {
        esp_now_send(opponent, (uint8_t *)&pongSlaveMessage, sizeof(pongSlaveMessage));
        state = PLAYING;
        Serial.println("State changed to PLAYING.");
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