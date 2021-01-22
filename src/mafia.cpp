#include <esp_now.h>
#include "joystick.h"
#include "screen.h"
#include "mafia.h"
#include "communication.h"
#include "menu.h"

MafiaRole mafiaRole = NONE;
unsigned long long int timeSinceLastRoleAssignment = 0; // assures that we don't flood the nodes with too many messages

void mafiaGame()
{
    if (mafiaRole == NARRATOR)
    {
        printRole("Narrator");
        if (isButtonPressed == true)
        {
            isButtonPressed = false;
            resetMafia();
        }
        if (millis() - timeSinceLastRoleAssignment > 1000)
        {
            switch (nrOfMafiaPlayers)
            {
                // this case is only for demonstration purposes
            case 2:
                esp_now_send(mafiaPlayers[0], (uint8_t *)&mafiaMafiosoMessage, sizeof(mafiaMafiosoMessage));
                esp_now_send(mafiaPlayers[1], (uint8_t *)&mafiaCivilianMesssage, sizeof(mafiaCivilianMesssage));
                break;
                // while it may seem clunky and predictable to assign roles in a predetermined way
                // it would take a lot of tinkering and trickery to figure out how it works
            case 3:
                esp_now_send(mafiaPlayers[0], (uint8_t *)&mafiaMafiosoMessage, sizeof(mafiaMafiosoMessage));
                esp_now_send(mafiaPlayers[1], (uint8_t *)&mafiaCivilianMesssage, sizeof(mafiaCivilianMesssage));
                esp_now_send(mafiaPlayers[2], (uint8_t *)&mafiaCivilianMesssage, sizeof(mafiaCivilianMesssage));
                break;
            case 4:
                esp_now_send(mafiaPlayers[0], (uint8_t *)&mafiaMafiosoMessage, sizeof(mafiaMafiosoMessage));
                esp_now_send(mafiaPlayers[1], (uint8_t *)&mafiaCivilianMesssage, sizeof(mafiaCivilianMesssage));
                esp_now_send(mafiaPlayers[2], (uint8_t *)&mafiaCivilianMesssage, sizeof(mafiaCivilianMesssage));
                esp_now_send(mafiaPlayers[3], (uint8_t *)&mafiaCivilianMesssage, sizeof(mafiaCivilianMesssage));
                break;
            case 5:
                esp_now_send(mafiaPlayers[0], (uint8_t *)&mafiaMafiosoMessage, sizeof(mafiaMafiosoMessage));
                esp_now_send(mafiaPlayers[1], (uint8_t *)&mafiaCivilianMesssage, sizeof(mafiaCivilianMesssage));
                esp_now_send(mafiaPlayers[2], (uint8_t *)&mafiaCivilianMesssage, sizeof(mafiaCivilianMesssage));
                esp_now_send(mafiaPlayers[3], (uint8_t *)&mafiaCivilianMesssage, sizeof(mafiaCivilianMesssage));
                esp_now_send(mafiaPlayers[4], (uint8_t *)&mafiaCivilianMesssage, sizeof(mafiaCivilianMesssage));
                break;
            case 6:
                esp_now_send(mafiaPlayers[0], (uint8_t *)&mafiaMafiosoMessage, sizeof(mafiaMafiosoMessage));
                esp_now_send(mafiaPlayers[1], (uint8_t *)&mafiaMafiosoMessage, sizeof(mafiaMafiosoMessage));
                esp_now_send(mafiaPlayers[2], (uint8_t *)&mafiaCivilianMesssage, sizeof(mafiaCivilianMesssage));
                esp_now_send(mafiaPlayers[3], (uint8_t *)&mafiaCivilianMesssage, sizeof(mafiaCivilianMesssage));
                esp_now_send(mafiaPlayers[4], (uint8_t *)&mafiaCivilianMesssage, sizeof(mafiaCivilianMesssage));
                esp_now_send(mafiaPlayers[5], (uint8_t *)&mafiaHealerMessage, sizeof(mafiaHealerMessage));
                break;
            case 7:
                esp_now_send(mafiaPlayers[0], (uint8_t *)&mafiaMafiosoMessage, sizeof(mafiaMafiosoMessage));
                esp_now_send(mafiaPlayers[1], (uint8_t *)&mafiaMafiosoMessage, sizeof(mafiaMafiosoMessage));
                esp_now_send(mafiaPlayers[2], (uint8_t *)&mafiaCivilianMesssage, sizeof(mafiaCivilianMesssage));
                esp_now_send(mafiaPlayers[3], (uint8_t *)&mafiaCivilianMesssage, sizeof(mafiaCivilianMesssage));
                esp_now_send(mafiaPlayers[4], (uint8_t *)&mafiaCivilianMesssage, sizeof(mafiaCivilianMesssage));
                esp_now_send(mafiaPlayers[5], (uint8_t *)&mafiaCivilianMesssage, sizeof(mafiaCivilianMesssage));
                esp_now_send(mafiaPlayers[6], (uint8_t *)&mafiaHealerMessage, sizeof(mafiaHealerMessage));
                break;
            case 8:
                esp_now_send(mafiaPlayers[0], (uint8_t *)&mafiaMafiosoMessage, sizeof(mafiaMafiosoMessage));
                esp_now_send(mafiaPlayers[1], (uint8_t *)&mafiaMafiosoMessage, sizeof(mafiaMafiosoMessage));
                esp_now_send(mafiaPlayers[2], (uint8_t *)&mafiaMafiosoMessage, sizeof(mafiaMafiosoMessage));
                esp_now_send(mafiaPlayers[3], (uint8_t *)&mafiaCivilianMesssage, sizeof(mafiaCivilianMesssage));
                esp_now_send(mafiaPlayers[4], (uint8_t *)&mafiaCivilianMesssage, sizeof(mafiaCivilianMesssage));
                esp_now_send(mafiaPlayers[5], (uint8_t *)&mafiaCivilianMesssage, sizeof(mafiaCivilianMesssage));
                esp_now_send(mafiaPlayers[6], (uint8_t *)&mafiaHealerMessage, sizeof(mafiaHealerMessage));
                esp_now_send(mafiaPlayers[7], (uint8_t *)&mafiaPolicemanMessage, sizeof(mafiaPolicemanMessage));
                break;
            case 9:
                esp_now_send(mafiaPlayers[0], (uint8_t *)&mafiaMafiosoMessage, sizeof(mafiaMafiosoMessage));
                esp_now_send(mafiaPlayers[1], (uint8_t *)&mafiaMafiosoMessage, sizeof(mafiaMafiosoMessage));
                esp_now_send(mafiaPlayers[2], (uint8_t *)&mafiaMafiosoMessage, sizeof(mafiaMafiosoMessage));
                esp_now_send(mafiaPlayers[3], (uint8_t *)&mafiaCivilianMesssage, sizeof(mafiaCivilianMesssage));
                esp_now_send(mafiaPlayers[4], (uint8_t *)&mafiaCivilianMesssage, sizeof(mafiaCivilianMesssage));
                esp_now_send(mafiaPlayers[5], (uint8_t *)&mafiaCivilianMesssage, sizeof(mafiaCivilianMesssage));
                esp_now_send(mafiaPlayers[6], (uint8_t *)&mafiaHealerMessage, sizeof(mafiaHealerMessage));
                esp_now_send(mafiaPlayers[7], (uint8_t *)&mafiaPolicemanMessage, sizeof(mafiaPolicemanMessage));
                esp_now_send(mafiaPlayers[8], (uint8_t *)&mafiaCivilianMesssage, sizeof(mafiaCivilianMesssage));
                break;
            case 10:
                esp_now_send(mafiaPlayers[0], (uint8_t *)&mafiaMafiosoMessage, sizeof(mafiaMafiosoMessage));
                esp_now_send(mafiaPlayers[1], (uint8_t *)&mafiaMafiosoMessage, sizeof(mafiaMafiosoMessage));
                esp_now_send(mafiaPlayers[2], (uint8_t *)&mafiaMafiosoMessage, sizeof(mafiaMafiosoMessage));
                esp_now_send(mafiaPlayers[3], (uint8_t *)&mafiaMafiosoMessage, sizeof(mafiaMafiosoMessage));
                esp_now_send(mafiaPlayers[4], (uint8_t *)&mafiaCivilianMesssage, sizeof(mafiaCivilianMesssage));
                esp_now_send(mafiaPlayers[5], (uint8_t *)&mafiaCivilianMesssage, sizeof(mafiaCivilianMesssage));
                esp_now_send(mafiaPlayers[6], (uint8_t *)&mafiaHealerMessage, sizeof(mafiaHealerMessage));
                esp_now_send(mafiaPlayers[7], (uint8_t *)&mafiaPolicemanMessage, sizeof(mafiaPolicemanMessage));
                esp_now_send(mafiaPlayers[8], (uint8_t *)&mafiaCivilianMesssage, sizeof(mafiaCivilianMesssage));
                esp_now_send(mafiaPlayers[9], (uint8_t *)&mafiaCivilianMesssage, sizeof(mafiaCivilianMesssage));
                break;
            default:
                // if there are too many or too few players, reset the game
                resetMafia();
                break;
            }
            timeSinceLastRoleAssignment = millis();
        }
    }
    else
    {
        switch (mafiaRole)
        {
        case MAFIOSO:
            printRole("Mafioso");
            break;
        case CIVILIAN:
            printRole("Civilian");
            break;
        case HEALER:
            printRole("Healer");
            break;
        case POLICEMAN:
            printRole("Policeman");
            break;
        default:
            break;
        }
        esp_now_send(narrator, (uint8_t *)&handshake, sizeof(handshake)); // ensures stable connection with narrator
    }
}

void resetMafia()
{
    esp_now_send(broadcastAddress, (uint8_t *)&mafiaDisbandMessage, sizeof(mafiaDisbandMessage));
    state = MENU;
    mafiaRole = NONE;
    printTitle(games[currentGame], 0);
    timeSinceLastRoleAssignment = 0;
    nrOfMafiaPlayers = 0;
}