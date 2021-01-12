#ifndef SCREEN_H
#define SCREEN_H

#include <Arduino.h>
#include <Wire.h> 
#include <SPI.h>
#include <SFE_MicroOLED.h> // Include the SFE_MicroOLED library

#define PIN_RESET 9
#define DC_JUMPER 1

extern MicroOLED screen; // I2C declaration
void printTitle(String title, int font);

#endif