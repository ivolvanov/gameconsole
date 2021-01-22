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
void resetMafia();

extern unsigned long long int timeSinceLastRoleAssignment;
extern unsigned long long int timeSinceLastNarratorAnnouncement;
extern MafiaRole mafiaRole;

#endif