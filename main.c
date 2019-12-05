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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <syslog.h>

int output_fd, sockfd;
int terminate;
int alarm_stop = 0;
unsigned int alarm_period = 1;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

#define INTERVAL 250
#define FILENAME "/var/tmp/joystickData.txt"

static void signal_handler (int signo)
 {   
     terminate = 1;
      
      struct timeval tv;
      tv.tv_sec = 1;
      tv.tv_usec = 250;
      setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
     
     if (signo == SIGINT){
         printf ("Caught signal, exiting.\n");
         syslog (LOG_INFO, "\nCaught signal, exiting.");
     } else if (signo == SIGTERM) {
         printf ("Caught signal, exiting.\n");
         syslog (LOG_INFO, "\nCaught signal, exiting.");
     } else {
         /* this should never happen */
         fprintf (stderr, "Unexpected signal!\n");
         syslog (LOG_INFO, "Caught unexpected signal.");
     }
 }


void * log_thread(void *arg){
    int errnum, error;
    char *direction;
    printf("\nIn log thread");
    while(1) {
        direction = joyGlobal.Dir;
        printf("\nWrite %s", direction);
        if( access(FILENAME, F_OK ) != -1 ) {
            output_fd = open(FILENAME,O_RDWR | O_APPEND, S_IRWXU | S_IRWXG | S_IRWXO); //file exists
 
            if (output_fd == -1){
                errnum = errno;
                fprintf(stderr, "Open Error: %s\n", strerror( errnum ));
            } else if (output_fd > 0) {
                printf("\nSuccesfully opened file");
            }
        } else { //create file
            output_fd = open(FILENAME, O_RDWR|O_CREAT|O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
 
            if (output_fd == -1){
                errnum = errno;
                fprintf(stderr, "Create Error: %s\n", strerror( errnum ));
            } else if (output_fd > 0) {
                printf("\nSuccesfully created file");
            }
        }
        pthread_mutex_lock(&lock);
        error = write(output_fd, direction, strlen(direction));
        if (error == -1) {
            printf("\nWrite error");
        }
        pthread_mutex_unlock(&lock);
        usleep(250000);
    } /* while */

}

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
    pthread_t ledThread, logThread;
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

    terminate = 0;

    if (signal (SIGINT, signal_handler) == SIG_ERR) {
         fprintf (stderr, "Cannot handle SIGINT!\n");
         exit (EXIT_FAILURE);
     }
 
     if (signal (SIGTERM, signal_handler) == SIG_ERR) {
         fprintf (stderr, "Cannot handle SIGTERM!\n");
         exit (EXIT_FAILURE);
     }


    printf("\nSetting up joystick");
    setupJoystick();

    printf("\nSetting up LED Display");
    setupLEDDisplay();

    /* Clear Screen */
    clearScreen();    
    
    /* Initialize joystick struct */
    joyGlobal.xAxis = 525;
    joyGlobal.yAxis = 525;
    joyGlobal.Dir = "";
    /* Thread to handle LED */
    printf("\nLED Thread");
    if(pthread_create(&ledThread, NULL, led_thread, (void*)NULL) < 0) {
        printf("\nFailed to create LED thread\n");
    }

    /* Thread to handle logging */
    printf("\nLogging Thread");
    if(pthread_create(&logThread, NULL, log_thread, (void*)NULL) < 0) {
        printf("\nFailed to create LOG thread\n");
    }


    while(1){
        if (terminate == 1){
            printf("\nTerminating");
            close(output_fd);
            remove(FILENAME);
            exit(0);
        }
        sleep(5);
    }
    
    return 0;
}
