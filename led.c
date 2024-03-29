/*
* Author: Kristina Brunsgaard
* Date: December 8, 2019
* File: led.c
* Description: Led functions to setup led, convert joystick values to led matrix values, mode to light up single led, mode to light up multiple leds
* Open Sources Used:
* https://github.com/raspberry-alpha-omega/spi
*/


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <wiringPiSPI.h>
#include <stdbool.h> 
#include "led.h"
#include "joystick.c"
#include "lookup.h"
#include "map.inc"

uint8_t buf[2];
uint8_t display[8];

#define CHANNEL 1

void writeLED(uint8_t reg, uint8_t val) {
     buf[0] = reg;
     buf[1] = val;
     wiringPiSPIDataRW(CHANNEL, buf, 2);
     usleep(50);
 }

void push(uint8_t col) {
     for (int i = 0; i < 7; ++i) {
          display[i] = display[i+1];
          display[7] = col;
     }
}
 
void show() {
      for (int i = 0; i < 8; ++i) {
          writeLED(i+1,display[i]);
      }
 }
 
void spichar(char c) {
      const uint8_t* bits = &font[c * 8];
      for (int i = 0; i < 8; ++i) {
          push(bits[i]);
          show();
          usleep(50);
      }
 }


void clearScreen(){
    for (int i = 0; i < 8; i++) {
        buf[0] = i;
        buf[1] = 0x00;
        wiringPiSPIDataRW(CHANNEL, buf, 2);
        usleep(20);
    }
    buf[0] = 0xFF;
    buf[1] = 0x00;
    wiringPiSPIDataRW(CHANNEL, buf, 2);
}

void setupLEDDisplay(){
    int i, j;
    
	if (wiringPiSPISetup(CHANNEL, 1000000) < 0) {
		fprintf (stderr, "SPI Setup failed: %s\n", strerror(errno));
		exit(errno);
    }

    writeLED(0x09,0x00);
    writeLED(0x0B,0x07);
    writeLED(0x0A,0xFF);
    writeLED(0x0C,0x01);

    /* Write 0 to led matrix */
    for (j = 0; j < 8; j++) {
        for (i = 0; i < 8; i++) {
            ledMatrix[j][i] = 0;
        }
    }
}

void convertJoytoLED(){
   static int xDir = 4;
   static int yDir = 4;
   int xLookup = joyGlobal.xAxis;
   int yLookup = joyGlobal.yAxis;
   bool left = false;
   bool right = false;
   bool up = false;
   bool down = false;
 
   if (xLookup < 500) {
        left = true;
        if (xDir == 0) {
            xDir = 7;
        } else {
            xDir--;
        }
    } else if (xLookup > 550) {
        right = true;
        if (xDir == 7) {
            xDir = 0;
        } else {
            xDir++;
        }
    }

    if (yLookup < 500) {
        up = true;
        if (yDir == 0) {
            yDir = 7;
        } else {
            yDir--;
        }
    } else if (yLookup > 550) {
        down = true;
        if (yDir == 7) {
            yDir = 0;
        } else {
            yDir++;
        }
    }

    if (up && left)
        joyGlobal.Dir = "\nUP-LEFT";
    else if (up && right)
        joyGlobal.Dir = "\nUP-RIGHT";
    else if (down && left)
        joyGlobal.Dir = "\nDOWN-LEFT";
    else if (down && right)
        joyGlobal.Dir = "\nDOWN-RIGHT";
    else if (down)
        joyGlobal.Dir = "\nDOWN";
    else if (up)
        joyGlobal.Dir = "\nUP";
    else if (left)
        joyGlobal.Dir = "\nLEFT";
    else if (right)
        joyGlobal.Dir = "\nRIGHT";
    else
        joyGlobal.Dir = "";

    if (ledMode == 0){
        singleLED(yDir, xDir);
    } else if (ledMode == 1) {
        multipleLEDs(yDir, xDir);
    }
}

void singleLED(int yDir, int xDir){
    uint8_t reg = YValues[yDir];
    uint8_t val = XValues[xDir];
    spichar(' ');
    writeLED(reg, val);
}

void multipleLEDs(int yDir, int xDir){
    int dec;
    int i;
    ledMatrix[yDir][xDir] = 1;
    for (i = 0; i < 8; i++){
        dec = arrayToHex(i);
        writeLED(i, dec);
    }
}

int arrayToHex(int yAxis){
    int dec, bin;
    char first[9];
    int one = 1;
    int zero = 0;
    char tmp[3];
    int i;
        for (i = 0; i < 8; i++){
            if(ledMatrix[yAxis][i] == 0){
                if (i == 0){
                    sprintf(first, "%d", zero);
                } else {
                    sprintf(tmp, "%d", zero);
                    strcat(first, tmp);
                }
            } else {
                if (i == 0){
                    sprintf(first, "%d", one);
                } else {
                    sprintf(tmp, "%d", one);
                    strcat(first, tmp);
                }
            } 
        }
        bin = atoi(first);
        dec = binaryToDecimal(bin);
        return dec;
}
/* From: https://www.geeksforgeeks.org/program-binary-decimal-conversion */
int binaryToDecimal(int n) 
{ 
    int num = n; 
    int dec_value = 0; 
  
    // Initializing base value to 1, i.e 2^0 
    int base = 1; 
  
    int temp = num; 
    while (temp) { 
        int last_digit = temp % 10; 
        temp = temp / 10; 
  
        dec_value += last_digit * base; 
  
        base = base * 2; 
    } 
  
    return dec_value; 
} 
