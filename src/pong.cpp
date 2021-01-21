#include <esp_now.h>
#include "joystick.h"
#include "screen.h"
#include "pong.h"
#include "communication.h"
#include "menu.h"

PongStatus pong = MASTER;

void pongGame()
{
    int playerScore = 0;
    int opponentScore = 0;

    if (pong == SLAVE)
        delay(20); //so that the master has enough time to perform the calculations and send the message

    while (playerScore < 3 && opponentScore < 3)
    {
        if (state != PONG_PLAYING)
        {
            printConnectionLost();
            break;
        }

        int paddleW = 3;     // Paddle width
        int paddleH = 15;    // Paddle height
        int ball_radius = 1; // Ball radius

        // Opponent paddle (left) position coordinates
        int opponent_Y = (screen.getLCDHeight() / 2) - (paddleH / 2);
        int opponent_X = 2;
        // Player paddle (right) position coordinates
        int player_Y = (screen.getLCDHeight() / 2) - (paddleH / 2);
        int player_X = screen.getLCDWidth() - 3 - paddleW;
        // Ball position coordinates
        int ball_X = opponent_X + paddleW + ball_radius;
        int ball_Y = random(1 + ball_radius, screen.getLCDHeight() - ball_radius); //opponent_Y + ball_rad;
        int ballVelocityX = 2;                                                     // Ball left/right velocity
        int ballVelocityY = 2;                                                     // Ball up/down velocity
        //int opponent_paddle_velocity = -2;                                 // Paddle 0 velocity
        int player_paddle_velocity = 2;   // Paddle 1 velocity
        uint32_t lastTimeBallHitWall = 0; // used to fix a bug related to ball radius and wall thickness that sometimes caused the ball to glitch in the wall
        // also, the ball will bounce incorrectly once if a single game is played for more than 8 years

        //While the ball is in the bounds of the playing field
        while ((ball_X > 0) &&
               (ball_X + ball_radius < screen.getLCDWidth() && state == PONG_PLAYING))
        {

            // Move ball
            ball_X += ballVelocityX;
            ball_Y += ballVelocityY;
            // Check if the ball is colliding with the opponent (left) paddle
            if (ball_X - ball_radius < opponent_X + paddleW)
            {
                // Check if ball is within paddle's height
                if ((ball_Y > opponent_Y) && (ball_Y < opponent_Y + paddleH))
                {
                    ball_X++;                       // Move ball over one to the right
                    ballVelocityX = -ballVelocityX; // Change velocity
                }
            }
            // Check if the ball hit the right paddle
            if (ball_X + ball_radius > player_X)
            {
                // Check if ball is within paddle's height
                if ((ball_Y > player_Y) && (ball_Y < player_Y + paddleH))
                {
                    ball_X--;                       // Move ball over one to the left
                    ballVelocityX = -ballVelocityX; // change velocity
                }
            }

            // Check if the ball hit the top or bottom
            if ((ball_Y <= ball_radius) || (ball_Y >= (screen.getLCDHeight() - ball_radius - 4)))
            {
                // Change up/down velocity direction
                if (millis() - lastTimeBallHitWall > 500)
                {
                    ballVelocityY = -ballVelocityY;
                    lastTimeBallHitWall = millis();
                }
            }

            bool didPaddleHitLeftEdge = (player_Y <= 1);
            bool didPaddleHitRightEdge = (player_Y > screen.getLCDHeight() - 2 - paddleH);

            if (joystick.getVertical() > 750 && !didPaddleHitLeftEdge)
                player_paddle_velocity = -2;
            else if (joystick.getVertical() < 250 && !didPaddleHitRightEdge)
                player_paddle_velocity = 2;
            else
                player_paddle_velocity = 0;

            player_Y += player_paddle_velocity;

            if (pong == MASTER)
            {
                sprintf(messageToBeSent, "%d;%d;%d;%d;%d", player_Y, ball_X, ball_Y, playerScore, opponentScore);
                /*esp_err_t result = */ esp_now_send(pongOpponent, (uint8_t *)&messageToBeSent, sizeof(messageToBeSent));
                sscanf(messageReceived, "%d", &opponent_Y);
            }
            else
            {
                sprintf(messageToBeSent, "%d", player_Y);
                /*esp_err_t result = */ esp_now_send(pongOpponent, (uint8_t *)&messageToBeSent, sizeof(messageToBeSent));
                sscanf(messageReceived, "%d;%d;%d;%d;%d", &opponent_Y, &ball_X, &ball_Y, &opponentScore, &playerScore);
                ball_X = screen.getLCDWidth() - ball_X - 2;
            }

            // Draw the Pong Field
            screen.clear(PAGE); // Clear the page
            // Draw an outline of the screen:
            screen.rect(0, 0, screen.getLCDWidth() - 1, screen.getLCDHeight());
            // Draw the center line
            screen.rectFill(screen.getLCDWidth() / 2 - 1, 0, 2, screen.getLCDHeight());
            // Draw the Paddles:
            screen.rectFill(opponent_X, opponent_Y, paddleW, paddleH);
            screen.rectFill(player_X, player_Y, paddleW, paddleH);
            // Draw the ball:
            screen.circle(ball_X, ball_Y, ball_radius);
            // Draw the score:
            screen.drawChar(screen.getLCDWidth() / 2 + 3, screen.getLCDHeight() * 0.75, playerScore + '0');
            screen.drawChar(screen.getLCDWidth() / 2 - 8, screen.getLCDHeight() * 0.75, opponentScore + '0');
            // Actually draw everything on the screen:
            screen.display();
            delay(1); // Delay for visibility
        }

        // the master takes care of scorekeeping
        // the old version used to be able to handle scorekeeping locally, however for some reason it stopped working after playing 1 game
        if (pong == MASTER)
        {
            if (ball_X <= 0)
            {
                playerScore++;
            }
            else if (ball_X >= screen.getLCDWidth() - ball_radius)
            {
                opponentScore++;
            }
            sprintf(messageToBeSent, "%d;%d;%d;%d;%d", player_Y, ball_X, ball_Y, playerScore, opponentScore);
            esp_now_send(pongOpponent, (uint8_t *)&messageToBeSent, sizeof(messageToBeSent));
        }
        delay(10);
    }

    if (playerScore == 3)
        printWin();
    else
        printLoss();

    state = MENU;
    pong = MASTER;
    // show the initial menu screen
    printTitle(games[currentGame], 0);
}