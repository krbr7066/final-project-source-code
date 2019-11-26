#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <wiringPiSPI.h>
#include "led.h"
#include "joystick.c"
#include "lookup.h"

uint8_t buf[2];

#define CHANNEL 1

void writeLED(uint8_t reg, uint8_t val) {
     buf[0] = reg;
     buf[1] = val;
     wiringPiSPIDataRW(CHANNEL, buf, 2);
     usleep(50);
 }

void clearScreen(){
    for (int i = 0; i < 8; i++) {
        buf[0] = i;
        buf[1] = 0x00;
        wiringPiSPIDataRW(CHANNEL, buf, 2);
        usleep(20);
    }
    buf = {0xFF, 0x00}
    wiringPiSPIDataRW(CHANNEL, buf, 2);
}

void setupLEDDisplay(){

	if (wiringPiSPISetup(CHANNEL, 1000000) < 0) {
		fprintf (stderr, "SPI Setup failed: %s\n", strerror(errno));
		exit(errno);
    }

    writeLED(0x09,0x00);
    writeLED(0x0B,0x07);
    writeLED(0x0A,0xFF);
    writeLED(0x0C,0x01);

}

void convertJoytoLED(){
   static int xDir = 4;
   static int yDir = 4;
   int xLookup = joyGlobal.xAxis;
   int yLookup = joyGlobal.yAxis;
 
   if (xLookup < 500) {
        printf("\nX less than 500");
        if (xDir == 0) {
            xDir = 0;
        } else {
            xDir--;
        }
    } else if (xLookup > 550) {
        printf("\nX greater than 550");
        if (xDir == 7) {
            xDir = 7;
        } else {
            xDir++;
        }
    }

    if (yLookup < 500) {
        printf("\nY less than 500");
        if (yDir == 0) {
            yDir = 0;
        } else {
            yDir--;
        }
    } else if (yLookup > 550) {
        printf("\nY greater than 550");
        if (yDir == 7) {
            yDir = 7;
        } else {
            yDir++;
        }
    }
    uint8_t reg = YValues[yDir];
    uint8_t val = XValues[xDir];
    printf("\nReg: %02X Val: %02X", reg, val);
    clearScreen();
    writeLED(reg, val);
}

