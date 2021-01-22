#ifndef MENU_H
#define MENU_H

#include <Arduino.h>

extern String games[];
extern int currentGame;
extern void menu();
void handleMenuState();
void handlePong();
void handleMafia();

enum State
{
  MENU,
  PONG_WAITING,
  MAFIA_WAITING,
  PONG_PLAYING, 
  MAFIA_PLAYING
};

extern State state;

#endif