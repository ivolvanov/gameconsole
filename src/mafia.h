#ifndef MAFIA_H
#define MAFIA_H

enum MafiaRole
{
    NONE,
    NARRATOR,
    MAFIOSO,
    CIVILIAN,
    HEALER,
    POLICEMAN
};

void mafiaGame();

extern int howManyMessages;
extern MafiaRole mafiaRole;

#endif