// rp-mcp3008 Reads analogue values through MCP3008 chip
//Source: http://www.noveldevices.co.uk/rp-mcp3008

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <mcp3004.h>
#include <wiringPiSPI.h>
#include <errno.h>
#include "joystick.h"

#define BASE 100
#define SPI_CHAN 0

joystick_t joyGlobal;

int setupJoystick(){

	if (wiringPiSPISetup(SPI_CHAN,500000) < 0) {
        fprintf (stderr, "SPI for Joystick Setup Failed: %s\n", strerror(errno));
        exit(errno);
        return -1;
    }

    if (mcp3004Setup(BASE,SPI_CHAN) < 0) {
        fprintf (stderr, "MCP3008 Setup Failed: %s\n", strerror(errno));
        exit(errno);  
        return -1;
    }
    
    return 0;
}

void readJoystick(){
    joyGlobal.xAxis = analogRead(BASE+1);
    joyGlobal.yAxis = analogRead(BASE+2);
//    printf("\nChannel 1: value=%4d", joyGlobal.xAxis); //X Values
//    printf("\nChannel 2: value=%4d", joyGlobal.yAxis); //Y Values
}

