#ifndef MENU_H
#define MENU_H

#include <Arduino.h>

extern String games[];
extern int currentGame;
extern void changeGame();

enum State
{
  MENU,
  PONG_WAITING,
  PLAYING
};

extern State state;

#endif