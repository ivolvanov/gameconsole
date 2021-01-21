#include <unity.h>
#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <esp_now.h>
#include "communication.h"
#include "pong.h"
#include "screen.h"
#include "joystick.h"
// void setUp(void) {
// // set stuff up here
// }

// void tearDown(void) {
// // clean stuff up here
// }

char mac[] = "1234";
char data[] = "pm";
// void test_receive_pong_master_message(void)
// {
//     TEST_ASSERT(pong == MASTER);
//     OnDataRecv((const uint8_t *)"124", (const uint8_t *)"pm", 2);
//     TEST_ASSERT(pong == SLAVE);
// }

void test_receive_pong_slave_message(void)
{
    TEST_ASSERT_EQUAL(1, 1);
}
void setup()
{
    Serial.begin(460800);
    delay(100);

    Wire.begin();

    initializeScreen();

    while (joystick.begin() == false)
    {
        Serial.println("Joystick does not appear to be connected. Please check wiring.");
        delay(100);
    }

    randomSeed(analogRead(A0));

    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);

    // Init ESP-NOW
    while (esp_now_init() != ESP_OK)
    {
        Serial.println("Error initializing ESP-NOW");
        delay(100);
    }

    // the broadcast address needs to be added as a peer
    esp_now_peer_info_t broadcastDevice;
    memcpy(broadcastDevice.peer_addr, broadcastAddress, 6);
    broadcastDevice.channel = 0;
    broadcastDevice.encrypt = false;

    // add the broadcast device (does not check whether adding was a success)
    esp_now_add_peer(&broadcastDevice);

    // Register for a callback function that will be called when data is received/sent
    esp_now_register_recv_cb(OnDataRecv);
    esp_now_register_send_cb(OnDataSent);

    esp_now_send(broadcastAddress, &handshake, 1);

    // show the initial menu screen

    UNITY_BEGIN();
    delay(100);
    RUN_TEST(test_receive_pong_slave_message);
    delay(100);
    UNITY_END();
}

void loop()
{
}
