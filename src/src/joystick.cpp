#include "joystick.h"

JOYSTICK joystick;
joystickPos currentPos = MIDDLE;
joystickPos previousPos = MIDDLE;
bool isButtonPressed = false;

void getJoystickInput()
{
  if (joystick.getButton() == 0)
    isButtonPressed = true;
    
  if (joystick.getVertical() > 750)
    currentPos = RIGHT;
  else if (joystick.getVertical() < 250)
    currentPos = LEFT;
  else
    currentPos = MIDDLE;
}