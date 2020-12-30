#include <Arduino.h>
#include <Wire.h>          // Include Wire if you're using I2C
#include <SFE_MicroOLED.h> // Include the SFE_MicroOLED library
#include <SPI.h>
#include <esp_now.h>
#include <WiFi.h>
#include "joystick.h"

#define PIN_RESET 9
#define DC_JUMPER 1

// uint8_t esp1[] = {0x24, 0x0A, 0xC4, 0x62, 0x5B, 0x08};
// uint8_t esp2[] = {0x30, 0xAE, 0xA4, 0x6B, 0x92, 0x4C};
// uint8_t esp3[] = {0x24, 0x0A, 0xC4, 0x60, 0xF1, 0x5C};
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t opponent[6];

// Variable to store if sending data was successful
String success;

uint8_t handshake = '!';
uint8_t pongMasterMessage[2] = {'p', 'm'};
uint8_t pongSlaveMessage[2] = {'p', 's'};
char messageToBeSent[40];
char messageReceived[40];

String games[] = {"Pong", "Mafia"};

int currentGame = 0;
enum PongStatus
{
  MASTER,
  SLAVE
};

enum State
{
  MENU,
  PONG_WAITING,
  PLAYING
};

PongStatus pong = MASTER;
State state = MENU;

// this is a bit of a thick boi, so far it hasn't caused any problems but keep in mind that if expanded further it may cause issues
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  // if the incoming data is from an unknown address, check if the device is already on the list and add it
  if (!esp_now_is_peer_exist(mac))
  {
    esp_now_peer_info_t newPeer;
    memcpy(newPeer.peer_addr, mac, 6);
    newPeer.channel = 0;
    newPeer.encrypt = false;
    newPeer.ifidx = ESP_IF_WIFI_STA;

    esp_now_add_peer(&newPeer);

    esp_now_send(broadcastAddress, &handshake, sizeof(handshake)); // also send back current device address
  }
  if (incomingData[0] == 'p')
  {
    if (state == MENU)
    {
      memcpy(opponent, mac, 6);
      pong = SLAVE;
      //Serial.println("Pong changed to SLAVE.");
    }
    else if (state == PONG_WAITING)
    {
      memcpy(opponent, mac, 6);
      state = PLAYING;
      if (incomingData[1] == 's')
      {
        esp_now_send(opponent, (uint8_t *)&pongMasterMessage, sizeof(pongMasterMessage));
      }
      else
      {
        esp_now_send(opponent, (uint8_t *)&pongSlaveMessage, sizeof(pongSlaveMessage));
        pong = SLAVE;
      }
    }
  }
  else
  {
    memcpy(&messageReceived, incomingData, len);
  }
}

MicroOLED oled(PIN_RESET, DC_JUMPER); // I2C declaration

void printTitle(String title, int font);
void getJoystickInput();
void changeGame();
void pongGame();

void setup()
{
  Serial.begin(115200);
  delay(100);
  Wire.begin();
  oled.begin();     // Initialize the OLED
  oled.clear(ALL);  // Clear the display's internal memory
  oled.clear(PAGE); // Clear the buffer.

  if (joystick.begin() == false)
  {
    Serial.println("Joystick does not appear to be connected. Please check wiring. Freezing...");
    while (1)
      ;
  }

  randomSeed(analogRead(A0));

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // the broadcast address needs to be added as a peer
  esp_now_peer_info_t broadcastDevice;
  memcpy(broadcastDevice.peer_addr, broadcastAddress, 6);
  broadcastDevice.channel = 0;
  broadcastDevice.encrypt = false;

  if (esp_now_add_peer(&broadcastDevice) != ESP_OK)
  {
    Serial.println("Failed to add peer broadcast device");
    return;
  }

  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);

  esp_now_send(broadcastAddress, &handshake, 1);

  // show the initial menu screen
  printTitle(games[currentGame], 0);
}

void loop()
{
  getJoystickInput();
  changeGame();

  if (state == PLAYING)
    pongGame();
}

void printTitle(String title, int font)
{
  int middleX = oled.getLCDWidth() / 2;
  int middleY = oled.getLCDHeight();

  oled.clear(PAGE);
  oled.setFontType(font);
  // Try to set the cursor in the middle of the screen
  oled.setCursor(middleX - (oled.getFontWidth() * (strlen("Game:") / 2)),
                 middleY * 0.25 - (oled.getFontWidth() / 2));
  // Print the title:
  oled.print("Game:");
  oled.setCursor(middleX - (oled.getFontWidth() * (title.length() / 2)),
                 middleY * 0.75 - (oled.getFontWidth() / 2));
  // Print the title:
  oled.print(title);
  oled.display();
  delay(10);
  oled.clear(PAGE);
}

void changeGame()
{
  if (state != PONG_WAITING)
  {
    if (isButtonPressed && currentGame == 0)
    {
      isButtonPressed = !isButtonPressed;
      if (pong == MASTER)
      {
        int middleX = oled.getLCDWidth() / 2;
        int middleY = oled.getLCDHeight();

        oled.clear(PAGE);
        oled.setFontType(0);
        oled.setCursor(middleX - (oled.getFontWidth() * (strlen("Waiting") / 2)),
                       middleY * 0.25 - (oled.getFontWidth() / 2));
        oled.print("Waiting");
        oled.setCursor(middleX - (oled.getFontWidth() * (strlen("for") / 2)),
                       middleY * 0.5 - (oled.getFontWidth() / 2));
        oled.print("for");
        oled.setCursor(middleX - (oled.getFontWidth() * (strlen("opponent") / 2)),
                       middleY * 0.75 - (oled.getFontWidth() / 2));
        oled.print("opponent");
        oled.display();

        esp_now_send(broadcastAddress, (uint8_t *)&pongMasterMessage, sizeof(pongMasterMessage));
        state = PONG_WAITING;
        //Serial.println("State changed to PONG_WAITING.");
      }
      else
      {
        esp_now_send(opponent, (uint8_t *)&pongSlaveMessage, sizeof(pongSlaveMessage));
        state = PLAYING;
        //Serial.println("State changed to PLAYING.");
      }
    }
    else if (currentPos != MIDDLE && previousPos == MIDDLE)
    {
      currentGame = !currentGame;
      previousPos = currentPos;
      printTitle(games[currentGame], 0);
    }
    else if (currentPos == MIDDLE)
    {
      previousPos = MIDDLE;
    }
  }
}

void pongGame()
{
  int playerScore = 0;
  int opponentScore = 0;

  if (pong == SLAVE)
    delay(20); //so that the master has enough time to perform the calculations and send the message

  while (playerScore < 3 && opponentScore < 3)
  {
    int paddleW = 3;  // Paddle width
    int paddleH = 15; // Paddle height
    int ball_rad = 1; // Ball radius

    // Opponent paddle (left) position coordinates
    int opponent_Y = (oled.getLCDHeight() / 2) - (paddleH / 2);
    int opponent_X = 2;
    // Player paddle (right) position coordinates
    int player_Y = (oled.getLCDHeight() / 2) - (paddleH / 2);
    int player_X = oled.getLCDWidth() - 3 - paddleW;
    // Ball position coordinates
    int ball_X = opponent_X + paddleW + ball_rad;
    int ball_Y = random(1 + ball_rad, oled.getLCDHeight() - ball_rad); //opponent_Y + ball_rad;
    int ballVelocityX = 2;                                             // Ball left/right velocity
    int ballVelocityY = 2;                                             // Ball up/down velocity
    //int opponent_paddle_velocity = -2;                                 // Paddle 0 velocity
    int player_paddle_velocity = 2;   // Paddle 1 velocity
    uint32_t lastTimeBallHitWall = 0; // used to fix a bug related to ball radius and wall thickness that sometimes caused the ball to glitch in the wall
    // also, the ball will bounce incorrectly once if a single game is played for more than 8 years

    //While the ball is in the bounds of the playing field
    while ((ball_X > 0) &&
           (ball_X + ball_rad < oled.getLCDWidth()))
    {
      // Move ball
      ball_X += ballVelocityX;
      ball_Y += ballVelocityY;
      // Check if the ball is colliding with the opponent (left) paddle
      if (ball_X - ball_rad < opponent_X + paddleW)
      {
        // Check if ball is within paddle's height
        if ((ball_Y > opponent_Y) && (ball_Y < opponent_Y + paddleH))
        {
          ball_X++;                       // Move ball over one to the right
          ballVelocityX = -ballVelocityX; // Change velocity
        }
      }
      // Check if the ball hit the right paddle
      if (ball_X + ball_rad > player_X)
      {
        // Check if ball is within paddle's height
        if ((ball_Y > player_Y) && (ball_Y < player_Y + paddleH))
        {
          ball_X--;                       // Move ball over one to the left
          ballVelocityX = -ballVelocityX; // change velocity
        }
      }

      // Check if the ball hit the top or bottom
      if ((ball_Y <= ball_rad) || (ball_Y >= (oled.getLCDHeight() - ball_rad - 4)))
      {
        // Change up/down velocity direction
        if (millis() - lastTimeBallHitWall > 500)
        {
          ballVelocityY = -ballVelocityY;
          lastTimeBallHitWall = millis();
        }
      }

      bool didPaddleHitLeftEdge = (player_Y <= 1);
      bool didPaddleHitRightEdge = (player_Y > oled.getLCDHeight() - 2 - paddleH);

      if (joystick.getVertical() > 750 && !didPaddleHitLeftEdge)
        player_paddle_velocity = -2;
      else if (joystick.getVertical() < 250 && !didPaddleHitRightEdge)
        player_paddle_velocity = 2;
      else
        player_paddle_velocity = 0;

      // Move the paddles up and down
      //opponent_Y += opponent_paddle_velocity;
      player_Y += player_paddle_velocity;

      if (pong == MASTER)
      {
        sprintf(messageToBeSent, "%d;%d;%d;%d;%d", player_Y, ball_X, ball_Y, playerScore, opponentScore);
        /*esp_err_t result = */ esp_now_send(opponent, (uint8_t *)&messageToBeSent, sizeof(messageToBeSent));
        sscanf(messageReceived, "%d", &opponent_Y);
      }
      else
      {
        sprintf(messageToBeSent, "%d", player_Y);
        /*esp_err_t result = */ esp_now_send(opponent, (uint8_t *)&messageToBeSent, sizeof(messageToBeSent));
        sscanf(messageReceived, "%d;%d;%d;%d;%d", &opponent_Y, &ball_X, &ball_Y, &opponentScore, &playerScore);
        ball_X = oled.getLCDWidth() - ball_X - 2;
      }

      // Draw the Pong Field
      oled.clear(PAGE); // Clear the page
      // Draw an outline of the screen:
      oled.rect(0, 0, oled.getLCDWidth() - 1, oled.getLCDHeight());
      // Draw the center line
      oled.rectFill(oled.getLCDWidth() / 2 - 1, 0, 2, oled.getLCDHeight());
      // Draw the Paddles:
      oled.rectFill(opponent_X, opponent_Y, paddleW, paddleH);
      oled.rectFill(player_X, player_Y, paddleW, paddleH);
      // Draw the ball:
      oled.circle(ball_X, ball_Y, ball_rad);
      // Draw the score:
      oled.drawChar(oled.getLCDWidth() / 2 + 3, oled.getLCDHeight() * 0.75, playerScore + '0');
      oled.drawChar(oled.getLCDWidth() / 2 - 8, oled.getLCDHeight() * 0.75, opponentScore + '0');
      // Actually draw everything on the screen:
      oled.display();
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
      else if (ball_X >= oled.getLCDWidth() - ball_rad)
      {
        opponentScore++;
      }
      sprintf(messageToBeSent, "%d;%d;%d;%d;%d", player_Y, ball_X, ball_Y, playerScore, opponentScore);
      esp_now_send(opponent, (uint8_t *)&messageToBeSent, sizeof(messageToBeSent));
    }
    delay(10);
  }
  state = MENU;
  //Serial.println("State changed to MENU.");
  pong = MASTER;
  //Serial.println("Pong changed to MASTER.");
  // show the initial menu screen
  printTitle(games[currentGame], 0);
}
