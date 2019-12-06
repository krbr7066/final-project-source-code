#include "led.c"
#include "joystick.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPiSPI.h>
#include "map.inc"

uint8_t on[] = { 0xFF, 0xFF };
uint8_t off[] = { 0xFF, 0x00 };
uint8_t buf[2];
#if 0
 void spi(uint8_t reg, uint8_t val) {
     buf[0] = reg;
     buf[1] = val;
     wiringPiSPIDataRW(CHANNEL, buf, 2);
     usleep(20);
 }
 
 uint8_t display[8];
 
 void push(uint8_t col) {
     for (int i = 0; i < 7; ++i) {
         display[i] = display[i+1];
         display[7] = col;
     }
 }
 
 void show() {
     for (int i = 0; i < 8; ++i) {
         spi(i+1,display[i]);
     }
 }

void spichar(char c) {
     const uint8_t* bits = &font[c * 8];
     for (int i = 0; i < 8; ++i) {
         push(bits[i]);
         show();
         usleep(100000);
     }
}

#endif
int main(int argc, char *argv[]){
    int i;
    setupJoystick();
    setupLEDDisplay();

    
    if (atoi(argv[1]) == 1){
       printf("\nClear Screen1");
       for (i = 0; i < 8; i++) {
         buf[0] = i;
         buf[1] = 0x00;
         wiringPiSPIDataRW(CHANNEL, buf, 2);
         usleep(20);
        }
     buf[0] = 0xFF;
     buf[1] = 0x00;
     wiringPiSPIDataRW(CHANNEL, buf, 2);
    } else if (atoi(argv[1]) == 2) {
        printf("\nClear Screen2");
     for (i = 0; i < 8; i++) {
         buf[0] = i;
         buf[1] = 0x00;
         wiringPiSPIDataRW(CHANNEL, buf, 2);
         usleep(20);
     }
   
    } else if (atoi(argv[1]) == 3){
        printf("\nClear Screen3");
      //  spichar(' ');
    } else if (atoi(argv[1]) == 4){
         for (;;) {
             memcpy(buf, on, 2);
             wiringPiSPIDataRW(CHANNEL, buf, 2);
             sleep(1);
            memcpy(buf, off, 2);
            wiringPiSPIDataRW(CHANNEL, buf, 2);  
            sleep(1);
        }


}      
        
    return 0;
}
