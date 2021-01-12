#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "SparkFun_Qwiic_Joystick_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_joystick
extern JOYSTICK joystick;                                   //Create instance of this object
extern bool isButtonPressed;
enum joystickPos
{
    LEFT,
    MIDDLE,
    RIGHT
};

extern joystickPos currentPos;
extern joystickPos previousPos;

void getJoystickInput();

#endif