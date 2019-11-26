#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <wiringPiSPI.h>
#include "led.h"

uint8_t buf[2];

#define CHANNEL 1

void writeLED(uint8_t reg, uint8_t val) {
     buf[0] = reg;
     buf[1] = val;
     wiringPiSPIDataRW(CHANNEL, buf, 2);
     usleep(20);
 }

void clearScreen(){
    buf[0] = 0xFF;
    buf[1] = 0x00;
    wiringPiSPIDataRW(CHANNEL, buf, 2);
    usleep(20);
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

//void convertJoytoLED(){
//    joyGlobal.xAxis()
//    joyGlobal.yAxis()
//}

