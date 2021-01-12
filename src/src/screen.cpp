#include "screen.h"

MicroOLED screen(PIN_RESET, DC_JUMPER); // I2C declaration

void printTitle(String title, int font)
{
    int middleX = screen.getLCDWidth() / 2;
    int middleY = screen.getLCDHeight();

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