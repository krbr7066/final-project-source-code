/**
* Author: Kristina Brunsgaard
* Date: November 14, 2019
* File: main.c
* Description:
*
*/
#include "led.c"
#include "joystick.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <wiringPiSPI.h>
#include <signal.h>
#include <pthread.h>

//struct joystick joyGlobal;
int alarm_stop = 0;
unsigned int alarm_period = 1;

void * led_thread(void *arg){
    printf("In led thread");
    #if 0
    while(1){
        writeLED(0x1, 0x1);
        clearScreen();
        writeLED(0x2, 0x2);
        clearScreen();
        writeLED(0x3, 0x4);
        clearScreen();
        writeLED(0x4, 0x8);
        clearScreen();
        writeLED(0x5, 0x10);
        clearScreen();
        writeLED(0x6, 0x20);
        clearScreen();
        writeLED(0x7, 0x40);
        clearScreen();
        writeLED(0x8, 0x80);
    }
    #endif
    while(1){
        convertJoytoLED();
        sleep(1);
    }
}

void on_alarm(){
    if (alarm_stop) return;
    else alarm(alarm_period);
    printf("\nIn read joystick");
    readJoystick();
    printf("\nXAxis: %4d", joyGlobal.xAxis);
    printf("\nYAxis: %4d", joyGlobal.yAxis);
}

int main(int argc, char *argv[])
{
    pthread_t ledThread;
    printf("\nSetting up joystick");
    setupJoystick();

    printf("\nSetting up LED Display");
    setupLEDDisplay();
    
    
    signal(SIGALRM, on_alarm);
    alarm(alarm_period);

    /* Thread to handle LED */
    printf("\nLED Thread");
    if(pthread_create(&ledThread, NULL, led_thread, (void*)NULL) < 0) {
        printf("\nFailed to create thread\n");
    }

    while(1){
        printf("\nRun");
        sleep(5);
    }


    
    return 0;
}
