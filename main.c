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
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include "queue.h"
#include <arpa/inet.h>

int output_fd, sockfd;
int terminate;
int alarm_stop = 0;
unsigned int alarm_period = 1;
pthread_mutex_t writeLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t readLock = PTHREAD_MUTEX_INITIALIZER;


#define INTERVAL 250
#define FILENAME "/var/tmp/joystickData.txt"
#define MYPORT "2000"
#define BACKLOG 50

struct threadInfo {
     int flag;
     int fd;
     pthread_t pid;
     SLIST_ENTRY(threadInfo) entries;
} *item, *item_temp, *test;


static void signal_handler (int signo)
 {   
     terminate = 1;
      
      struct timeval tv;
      tv.tv_sec = 1;
      tv.tv_usec = 250;
      setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
     
     if (signo == SIGINT){
         printf ("\nCaught signal, exiting.\n");
         syslog (LOG_INFO, "\nCaught signal, exiting.");
     } else if (signo == SIGTERM) {
         printf ("\nCaught signal, exiting.\n");
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
        pthread_mutex_lock(&writeLock);
        error = write(output_fd, direction, strlen(direction));
        if (error == -1) {
            printf("\nWrite error");
        }
        pthread_mutex_unlock(&writeLock);
        usleep(250000);
    } /* while */

}

void * led_thread(void *arg){
    printf("\nIn led thread");
    
    while(1){
        convertJoytoLED();
        usleep(150000);
    }
}

void * sendFile(void *arg){
    pthread_t tid;
    
    printf("\nIn Send File");
    struct threadInfo *newItem = (struct threadInfo*) arg;
    tid = pthread_self();
    newItem->pid = tid;
    FILE *fp = fopen(FILENAME, "r");    
    pthread_mutex_lock(&readLock);
    file_size = fread(buffSend, sizeof(char), sizeof(buffSend), fp);
    if(send(new_fd, buffSend, file_size, 0) < 0) {
            perror("Failed to send file");
    }
    printf("Sent: %s", buffSend);
    pthread_mutex_unlock(&readLock);

    close(newItem->fd);
    pthread_exit(NULL);
    newItem->flag = 1;
}

void on_alarm(){
    printf("\nIn read joystick");
    readJoystick();
    printf("\nXAxis: %4d", joyGlobal.xAxis);
    printf("\nYAxis: %4d", joyGlobal.yAxis);
}

int main(int argc, char *argv[])
{   
    pthread_t ledThread, logThread, tid;
    struct itimerval it_val;  /* for setting itimer */
    struct sockaddr_in their_addr; /* connector's address information */
    socklen_t addr_size;
    struct addrinfo hints, *res;
    int success = 0;
    int opt = 1;
    int new_fd, errnum, status;

    if (argc != 2) {
        printf("\nArgument Error:\n ->Enter 0 for single LED mode\n ->Enter 1 for multi LED mode\n");
        exit(0);
    }
    
    ledMode = atoi(argv[1]);
    printf("\nArgument: %d", ledMode);

    /* Set interval timer.  We want frequency in ms, 
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

    //Initialize singly linked list
     item = (struct threadInfo *) malloc(sizeof(struct threadInfo));
 
     SLIST_HEAD(slisthead, threadInfo) head;
     SLIST_INIT(&head);


    terminate = 0;

    if (signal (SIGINT, signal_handler) == SIG_ERR) {
         fprintf (stderr, "Cannot handle SIGINT!\n");
         exit (EXIT_FAILURE);
     }
 
     if (signal (SIGTERM, signal_handler) == SIG_ERR) {
         fprintf (stderr, "Cannot handle SIGTERM!\n");
         exit (EXIT_FAILURE);
     }

     memset(&hints, 0, sizeof hints);
     hints.ai_family = AF_INET;  // use IPv4 or IPv6, whichever
     hints.ai_socktype = SOCK_STREAM;
     hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
 
     status = getaddrinfo(NULL, MYPORT, &hints, &res);
     if (status < 0) {
     printf("\ngetaddrinfo error");
     }
    
     // make a socket, bind it, and listen on it:
     sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
     if (sockfd < 0) {
         errnum = errno;
         fprintf(stderr, "Sockfd Error: %s\n", strerror( errnum ));
      }

    if( setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
           sizeof(opt)) < 0 )
     {
         perror("setsockopt");
         exit(EXIT_FAILURE);
     }
 
     new_fd = bind(sockfd, res->ai_addr, res->ai_addrlen);
     if (new_fd < 0) {
         errnum = errno;
         fprintf(stderr, "Bind Error: %s\n", strerror( errnum ));
     } else {
         printf("\nBind");
     }
    
    if (listen(sockfd, BACKLOG) == -1) {
         errnum = errno;
         fprintf(stderr, "Listen Error: %s\n", strerror( errnum ));
     } else {
         printf("\nListen");
     }

    printf("\nSetting up joystick");
    setupJoystick();

    printf("\nSetting up LED Display");
    setupLEDDisplay();

    /* Clear Screen */
    clearScreen();    
    spichar(' ');
    
    /* Initialize joystick struct */
    joyGlobal.xAxis = 450;
    joyGlobal.yAxis = 450;
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

        success = 0;

        if (terminate == 1){
            printf("\nTerminating\n");
            close(output_fd);
            remove(FILENAME);
            exit(0);
        }

        addr_size = sizeof their_addr;
 
         new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);    
         if (new_fd < 0) {
             errnum = errno;
             fprintf(stderr, "Accept Error: %s\n", strerror( errnum ));
         } else {
             syslog (LOG_INFO, "Accepted connection from %s", inet_ntoa(their_addr.sin_addr));
             printf("\nAccepted connection from %s", inet_ntoa(their_addr.sin_addr));
             success = 1;
         }
     if(success) {
        item = (struct threadInfo *) malloc(sizeof(struct threadInfo));
        //Create thread
        item->fd = new_fd;
        if(pthread_create(&tid, NULL, sendFile, (void*)item) == 0) {
             SLIST_INSERT_HEAD(&head, item, entries);
         } else {
             printf("Failed to create thread\n");
         }

    //Check if thread completes 
    SLIST_FOREACH_SAFE(test, &head, entries, item_temp){
         if (test->flag == 1) {
             pthread_join(test->pid, NULL);
             SLIST_REMOVE(&head, test, threadInfo, entries);
             free(test);
         }
     }


        } /* success */
    } /* while */
    return 0;
} /* main */
