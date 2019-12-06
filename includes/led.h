#include <stdint.h>

int ledMatrix[8][8];
int ledMode;
void writeLED(uint8_t reg, uint8_t val);
void clearScreen();
void setupLEDDisplay();
void convertJoytoLED();
void singleLED(int yDir, int xDir);
void multipleLEDs(int yDir, int xDir);
int arrayToHex(int yAxis);
int binaryToDecimal(int n);
