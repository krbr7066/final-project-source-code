/**
* Author: Kristina Brunsgaard
* Date: November 14, 2019
* File: main.c
* Description:
* Open Sources Used:
* https://stackoverflow.com/questions/2086126/need-programs-that-illustrate-use-of-settimer-and-alarm-functions-in-gnu-c
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
#include <sys/time.h>

int alarm_stop = 0;
unsigned int alarm_period = 1;

#define INTERVAL 250

void * led_thread(void *arg){
    printf("In led thread");
    
    while(1){
        convertJoytoLED();
        usleep(250000);
    }
}

void on_alarm(){
    printf("\nIn read joystick");
    readJoystick();
    printf("\nXAxis: %4d", joyGlobal.xAxis);
    printf("\nYAxis: %4d", joyGlobal.yAxis);
}

int main(int argc, char *argv[])
{   
    pthread_t ledThread;
    struct itimerval it_val;  /* for setting itimer */

    /* Upon SIGALRM, call DoStuff().
    * Set interval timer.  We want frequency in ms, 
    * but the setitimer call needs seconds and useconds. */
    if (signal(SIGALRM, (void (*)(int)) on_alarm) == SIG_ERR) {
        perror("Unable to catch SIGALRM");
        exit(1);
    }
    it_val.it_value.tv_sec =     INTERVAL/1000;
    it_val.it_value.tv_usec =    (INTERVAL*1000) % 1000000;   
    it_val.it_interval = it_val.it_value;
    if (setitimer(ITIMER_REAL, &it_val, NULL) == -1) {
        perror("error calling setitimer()");
        exit(1);
    }
    printf("\nSetting up joystick");
    setupJoystick();

    printf("\nSetting up LED Display");
    setupLEDDisplay();

    /* Clear Screen */
    clearScreen();    
    
    //signal(SIGALRM, on_alarm);
    //alarm(alarm_period);

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
