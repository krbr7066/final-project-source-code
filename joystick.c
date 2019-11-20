// rp-mcp3008 Reads analogue values through MCP3008 chip
//Source: http://www.noveldevices.co.uk/rp-mcp3008

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "spi-driver/wiringPi.h"
#include "spi-driver/mcp3004.h"
#include "spi-driver/wiringPiSPI.h"

#define BASE 100
#define SPI_CHAN 0

int main (int argc, char *argv[])
{
	int i;

	printf("wiringPiSPISetup RC=%d\n",wiringPiSPISetup(0,500000));

	mcp3004Setup(BASE,SPI_CHAN);
	
// Loop indefinitely, waiting for 100ms between each set of data
	while(1)
	{	for(i=0;i<8;i++)printf("Channel %d: value=%4d\n",i,analogRead(BASE+i));
		printf("\n");
		usleep(500000);
	}
}

