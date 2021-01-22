#ifndef PONG_H
#define PONG_H

enum PongStatus
{
  MASTER,
  SLAVE
};

void pongGame();

extern PongStatus pong;
extern bool connectionLost;

#endif
