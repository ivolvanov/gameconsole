#include "screen.h"

MicroOLED screen(PIN_RESET, DC_JUMPER); // I2C declaration

int middleX = screen.getLCDWidth() / 2;
int middleY = screen.getLCDHeight();

void initializeScreen()
{
    screen.begin();     // Initialize the OLED
    screen.clear(ALL);  // Clear the display's internal memory
    screen.clear(PAGE); // Clear the buffer.
}

void printTitle(String title, int font)
{
    screen.clear(PAGE);
    screen.setFontType(font);
    // Try to set the cursor in the middle of the screen
    screen.setCursor(middleX - (screen.getFontWidth() * (strlen("Game:") / 2)),
                     middleY * 0.25 - (screen.getFontWidth() / 2));
    // Print the title:
    screen.print("Game:");
    screen.setCursor(middleX - (screen.getFontWidth() * (title.length() / 2)),
                     middleY * 0.75 - (screen.getFontWidth() / 2));
    // Print the title:
    screen.print(title);
    screen.display();
    delay(10);
    screen.clear(PAGE);
}

void printWaitingForOpponent()
{
    screen.clear(PAGE);
    screen.setFontType(0);
    screen.setCursor(middleX - (screen.getFontWidth() * (strlen("Waiting") / 2)),
                     middleY * 0.25 - (screen.getFontWidth() / 2));
    screen.print("Waiting");
    screen.setCursor(middleX - (screen.getFontWidth() * (strlen("for") / 2)),
                     middleY * 0.5 - (screen.getFontWidth() / 2));
    screen.print("for");
    screen.setCursor(middleX - (screen.getFontWidth() * (strlen("opponent") / 2)),
                     middleY * 0.75 - (screen.getFontWidth() / 2));
    screen.print("opponent");
    screen.display();
}

void printConnectionLost()
{
    screen.clear(PAGE);
    screen.setFontType(0);
    screen.setCursor(0,
                     middleY * 0.33 - (screen.getFontWidth() / 2));
    screen.print("Connection");
    screen.setCursor(middleX - (screen.getFontWidth() * (strlen("lost") / 2)),
                     middleY * 0.66 - (screen.getFontWidth() / 2));
    screen.print("lost");
    screen.display();
    delay(3000);
}