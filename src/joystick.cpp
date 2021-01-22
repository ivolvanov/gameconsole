#include "joystick.h"

JOYSTICK joystick;
joystickPos currentPos = MIDDLE;
joystickPos previousPos = MIDDLE;
bool isButtonPressed = false;
unsigned long long int timeSinceLastButtonPress = 0;

void getJoystickInput()
{
  if (joystick.getButton() == 0 && millis() - timeSinceLastButtonPress > 1000)
  {
    isButtonPressed = true;
    timeSinceLastButtonPress = millis();
  }

  if (joystick.getVertical() > 750)
    currentPos = RIGHT;
  else if (joystick.getVertical() < 250)
    currentPos = LEFT;
  else
    currentPos = MIDDLE;
}