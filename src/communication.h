#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <Arduino.h>

// Mac addresses of the three esp's used for development for debugging
// uint8_t esp1[] = {0x24, 0x0A, 0xC4, 0x62, 0x5B, 0x08};
// uint8_t esp2[] = {0x30, 0xAE, 0xA4, 0x6B, 0x92, 0x4C};
// uint8_t esp3[] = {0x24, 0x0A, 0xC4, 0x60, 0xF1, 0x5C};

extern uint8_t pongMasterMessage[2];
extern uint8_t pongSlaveMessage[2];
extern uint8_t pongFoundMessage[2];

extern uint8_t mafiaNarratorMessage[2];
extern uint8_t mafiaJoinMessage[2];
extern uint8_t mafiaDisbandMessage[2];
extern uint8_t mafiaMafiosoMessage[2];
extern uint8_t mafiaCivilianMesssage[2];
extern uint8_t mafiaHealerMessage[2];
extern uint8_t mafiaPolicemanMessage[2];

extern uint8_t handshake;
extern uint8_t broadcastAddress[];

extern int unsuccessfulSendsMafia;
extern int unsuccessfulSendsPong;
extern char messageToBeSent[40];
extern char messageReceived[40];

extern uint8_t pongOpponent[6];
extern uint8_t narrator[6];
extern uint8_t mafiaPlayers[10][6];

extern int nrOfMafiaPlayers;

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);

void pongDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);
void mafiaDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);

#endif