#include <Arduino.h>
#include <Wire.h>                                    // Include Wire if you're using I2C
#include "SparkFun_Qwiic_Joystick_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_joystick
#include <SFE_MicroOLED.h>                           // Include the SFE_MicroOLED library
#include <SPI.h>
#include <esp_now.h>
#include <WiFi.h>

#define PIN_RESET 9
#define DC_JUMPER 1

uint8_t esp1[] = {0x24, 0x0A, 0xC4, 0x62, 0x5B, 0x08};
uint8_t esp2[] = {0x30, 0xAE, 0xA4, 0x6B, 0x92, 0x4C};
uint8_t esp3[] = {0x24, 0x0A, 0xC4, 0x60, 0xF1, 0x5C};

// Variable to store if sending data was successful
String success;

char messageToBeSent[40] = "This is a message from ESP 3";
char messageReceived[40];

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&messageReceived, incomingData, len);
  //Serial.print("Number received: ");
  //Serial.println(messageReceived);
}

MicroOLED oled(PIN_RESET, DC_JUMPER); // I2C declaration
JOYSTICK joystick;                    //Create instance of this object

void shapeExample();

void setup()
{
  Serial.begin(115200);
  delay(100);
  Wire.begin();
  oled.begin();     // Initialize the OLED
  oled.clear(ALL);  // Clear the display's internal memory
  oled.display();   // Display what's in the buffer (splashscreen)
  delay(1000);      // Delay 1000 ms
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

  esp_now_peer_info_t esp2Peer;
  memcpy(esp2Peer.peer_addr, esp2, 6);
  esp2Peer.channel = 0;
  esp2Peer.encrypt = false;

  esp_now_peer_info_t esp3Peer;
  memcpy(esp3Peer.peer_addr, esp3, 6);
  esp3Peer.channel = 0;
  esp3Peer.encrypt = false;

  if (esp_now_add_peer(&esp2Peer) != ESP_OK)
  {
    Serial.println("Failed to add peer ESP2");
    return;
  }

  if (esp_now_add_peer(&esp3Peer) != ESP_OK)
  {
    Serial.println("Failed to add peer ESP3");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}

void shapeExample()
{

  // Silly pong demo. It takes a lot of work to fake pong...
  int paddleW = 3;  // Paddle width
  int paddleH = 15; // Paddle height
  // Paddle 0 (left) position coordinates
  int opponent_Y = (oled.getLCDHeight() / 2) - (paddleH / 2);
  int paddle0_X = 2;
  // Paddle 1 (right) position coordinates
  int player_Y = (oled.getLCDHeight() / 2) - (paddleH / 2);
  int paddle1_X = oled.getLCDWidth() - 3 - paddleW;
  int ball_rad = 2; // Ball radius
  // Ball position coordinates
  int ball_X = paddle0_X + paddleW + ball_rad;
  int ball_Y = random(1 + ball_rad, oled.getLCDHeight() - ball_rad); //opponent_Y + ball_rad;
  int ballVelocityX = 2;                                             // Ball left/right velocity
  int ballVelocityY = 2;                                             // Ball up/down velocity
  int opponent_paddle_velocity = -2;                                          // Paddle 0 velocity
  int player_paddle_velocity = 2;                                           // Paddle 1 velocity

  //while(ball_X >= paddle0_X + paddleW - 1)
  while ((ball_X - ball_rad > 1) &&
         (ball_X + ball_rad < oled.getLCDWidth() - 2))
  {
    // Increment ball's position
    ball_X += ballVelocityX;
    ball_Y += ballVelocityY;
    // Check if the ball is colliding with the left paddle
    if (ball_X - ball_rad < paddle0_X + paddleW)
    {
      // Check if ball is within paddle's height
      if ((ball_Y > opponent_Y) && (ball_Y < opponent_Y + paddleH))
      {
        ball_X++;                       // Move ball over one to the right
        ballVelocityX = -ballVelocityX; // Change velocity
      }
    }
    // Check if the ball hit the right paddle
    if (ball_X + ball_rad > paddle1_X)
    {
      // Check if ball is within paddle's height
      if ((ball_Y > player_Y) && (ball_Y < player_Y + paddleH))
      {
        ball_X--;                       // Move ball over one to the left
        ballVelocityX = -ballVelocityX; // change velocity
      }
    }
    // Check if the ball hit the top or bottom
    if ((ball_Y <= ball_rad) || (ball_Y >= (oled.getLCDHeight() - ball_rad - 1)))
    {
      // Change up/down velocity direction
      ballVelocityY = -ballVelocityY;
    }

    bool didPaddleHitLeftEdge = (player_Y <= 1);
    bool didPaddleHitRightEdge = (player_Y > oled.getLCDHeight() - 2 - paddleH);

    if (joystick.getHorizontal() > 750 && !didPaddleHitRightEdge)
      player_paddle_velocity = 2;
    else if (joystick.getHorizontal() < 250 && !didPaddleHitLeftEdge)
      player_paddle_velocity = -2;
    else
      player_paddle_velocity = 0;

    // Move the paddles up and down
    opponent_Y += opponent_paddle_velocity;
    player_Y += player_paddle_velocity;

    sprintf(messageToBeSent, "%d;%d;%d", player_Y, ball_X, ball_Y); // PADDLE_Y, BALL_X, BALL_Y
    esp_err_t result = esp_now_send(esp3, (uint8_t *)&messageToBeSent, sizeof(messageToBeSent));

    // Change paddle 0's direction if it hit top/bottom
    if ((opponent_Y <= 1) || (opponent_Y > oled.getLCDHeight() - 2 - paddleH))
    {
      //opponent_paddle_velocity = -opponent_paddle_velocity;
    }
    // Change paddle 1's direction if it hit top/bottom
    if ((player_Y <= 1) || (player_Y > oled.getLCDHeight() - 2 - paddleH))
    {
      //player_paddle_velocity = -player_paddle_velocity;
    }

    sscanf(messageReceived, "%d", &opponent_Y);

    // Draw the Pong Field
    oled.clear(PAGE); // Clear the page
    // Draw an outline of the screen:
    oled.rect(0, 0, oled.getLCDWidth() - 1, oled.getLCDHeight());
    // Draw the center line
    oled.rectFill(oled.getLCDWidth() / 2 - 1, 0, 2, oled.getLCDHeight());
    // Draw the Paddles:
    oled.rectFill(paddle0_X, opponent_Y, paddleW, paddleH);
    oled.rectFill(paddle1_X, player_Y, paddleW, paddleH);
    // Draw the ball:
    oled.circle(ball_X, ball_Y, ball_rad);
    // Actually draw everything on the screen:
    oled.display();
    delay(1); // Delay for visibility
  }
  delay(10);
}

void loop()
{
  shapeExample(); // Then the shape example
}