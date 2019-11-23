/* Author: Kristina Brunsgaard
*Assignment 5
*Description: Assignment that creates socket on port 9000, writes received content to file, and sends back to client.
*/

#include "../queue.h"
#include <string.h>
#include <fcntl.h> /* Added for the nonblocking socket */
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <syslog.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/stat.h>
#include <linux/fs.h>
#include <errno.h>
#include <pthread.h>
#include "../aesd_ioctl.h"
#include <sys/ioctl.h>

#define BUFFER_LENGTH 256
#define MYPORT "9000"  // the port users will be connecting to
#define BACKLOG 50     // how many pending connections queue will hold
#define FILENAME    "/var/tmp/JoystickFile.txt"
int terminate;
int sockfd;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t readLock = PTHREAD_MUTEX_INITIALIZER;
int alarm_stop = 0;
unsigned int alarm_period = 10;
static char receive[BUFFER_LENGTH];
//typedef struct threadInfo threadInfo_t;

struct threadInfo {
    int flag;
    int fd;
    pthread_t pid;
    SLIST_ENTRY(threadInfo) entries;
} *item, *item_temp, *test;

void sendFile(int new_fd){
    char buffSend[1024]; 
    int file_size;
    printf("\nSend file");
    FILE *fp = fopen(FILENAME, "r"); 
    pthread_mutex_lock(&readLock);
    file_size = fread(buffSend, sizeof(char), sizeof(buffSend), fp);
    if(send(new_fd, buffSend, file_size, 0) < 0) {
        perror("Failed to send file");
    }
    printf("Sent: %s", buffSend); 
    pthread_mutex_unlock(&readLock);
}

void writeBuffer(char *buffer) {
    int output_fd, errnum, error;
    printf("\nWriting %s", buffer);
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
    error = write(output_fd, buffer, strlen(buffer));
    if (error == -1) {
        printf("\nWrite error");
    } 
    pthread_mutex_unlock(&lock);
}

static void signal_handler (int signo)
{
	terminate = 1;

     struct timeval tv;
     tv.tv_sec = 1;
     tv.tv_usec = 250;
     setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

	if (signo == SIGINT){
		printf ("Caught signal, exiting.\n");
		syslog (LOG_INFO, "Caught signal, exiting.");
	} else if (signo == SIGTERM) {
		printf ("Caught signal, exiting.\n");
		syslog (LOG_INFO, "Caught signal, exiting.");
	} else {
		/* this should never happen */
		fprintf (stderr, "Unexpected signal!\n");
		syslog (LOG_INFO, "Caught unexpected signal.");
	}
}

void * writeSocket(void *arg)
{
    pthread_t tid;
    char buffer[80];
    int len = 0;    
;

    printf("\nIn write socket");
    struct threadInfo *newItem = (struct threadInfo*) arg;
    tid = pthread_self();
    newItem->pid = tid;

    printf("\nCheck: %d", newItem->fd);

    while(1){
        printf("In while loop\n");
        memset(buffer, 0, sizeof(buffer)); //clear buffer
		    len = recv(newItem->fd, buffer, sizeof(buffer), 0);
		    if (len < 0){
       	          printf("\nError reading socket");
                    break;
       		} else if (len == 0){
                printf("\nClosed connection");
                break;
            } else {
			    printf("\nBuffer: %s", buffer);
            }
        printf("LEN = %d", len);   
       /* while(len)*/
    writeBuffer(buffer);
    sendFile(newItem->fd);
    }
    close(newItem->fd);   
    pthread_exit(NULL);
    newItem->flag = 1;
    
}


int main(int argc, char* argv[]){
    int opt = 1;
    //int sockfd, 
    int new_fd;
    int status;
    int errnum;
    struct sockaddr_in their_addr; /* connector's address information */
    socklen_t addr_size;
    struct addrinfo hints, *res;
    int success = 0;
//    int output_fd;
    pthread_t tid;


    //Initialize singly linked list
    item = (struct threadInfo *) malloc(sizeof(struct threadInfo));

    SLIST_HEAD(slisthead, threadInfo) head;
    SLIST_INIT(&head);
    
    terminate = 0; /* Initialize terminal*/	

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
  
    //check for daemon mode, daemon code comes from Linux System Programming
    if (argc == 2) {
         if (strcmp(argv[1], "-d") == 0){
            printf("\nDaemonize!");
	        pid_t process_id = 0;
   
             process_id = fork();
             if (process_id < 0)
             {
                printf("\nFork Error");
                exit(1);
             }

             if (process_id > 0)
             {
                printf("process_id of child process %d \n", process_id);
                exit(0);
             }
        }
    } else if (argc > 2) {
        printf("\nToo many agruments:\n 1. -d to daemonize\n");
    }
    
    if (listen(sockfd, BACKLOG) == -1) {
        errnum = errno;
        fprintf(stderr, "Listen Error: %s\n", strerror( errnum ));
    } else {
        printf("\nListen");
    }  

	while(1) {
        success = 0;

	    if (terminate == 1){ //Clean up functions and exit
            printf("\nTerminating\nClosed connection from %s", inet_ntoa(their_addr.sin_addr));
            freeaddrinfo(res);//free mem
            free(item);
            close(new_fd);
            close(sockfd);    //close socket
            remove(FILENAME); //delete file
        	exit(0); 
	    }   	

    	// now accept an incoming connection:

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
    
        if(pthread_create(&tid, NULL, writeSocket, (void*)item) == 0) {
            printf("\nInsert: %08lX\n", tid);
            SLIST_INSERT_HEAD(&head, item, entries);
        } else {
            printf("Failed to create thread\n");
        } 


        //Check if thread completes

        SLIST_FOREACH_SAFE(test, &head, entries, item_temp){
            if (test->flag == 1) {
                printf("\nstill running: %08lX\n", test->pid);
                pthread_join(test->pid, NULL);
                SLIST_REMOVE(&head, test, threadInfo, entries);
                free(test); 
            }
        } 

    	    
    } /* success */
    
} /* while(1) */
} /* main */

