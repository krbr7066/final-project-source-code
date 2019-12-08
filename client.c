/* 
* Author: Kristina Brunsgaard
* Date: December 8, 2019
* File: client.c
* Description: Client program to connect to raspberry pi through static ip a* nd receive joystick data file
* Open Sources Used:
* http://www.science.smith.edu/dftwiki/index.php/Tutorial:_Client/Server_on_the_Raspberry_Pi
*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 

void error(char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno = 2000, n;
    char serverIp[] = "192.168.1.3";
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[4096];
    char* data;

    if (argc < 3) {
      // error( const_cast<char *>( "usage myClient2 hostname port\n" ) );
      printf( "contacting %s on port %d\n", serverIp, portno );
      // exit(0);
    }
    if ( ( sockfd = socket(AF_INET, SOCK_STREAM, 0) ) < 0 )
        error( "ERROR opening socket" );

    if ( ( server = gethostbyname( serverIp ) ) == NULL ) 
        error( "ERROR, no such host\n" );
    
    bzero( (char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy( (char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if ( connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error( "ERROR connecting" );
    
//    data = getData(sockfd);
 
int error = 0;
socklen_t len = sizeof (error);
int retval;

    while(1) {
       // memset(buffer, 0, sizeof(buffer));
        #if 0
        retval = getsockopt (sockfd, SOL_SOCKET, SO_ERROR, &error, &len);
        
    if (retval != 0) {
        /* there was a problem getting the error code */
        fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
        return -1;
    }

    if (error != 0) {
        /* socket has a non zero error status */
        fprintf(stderr, "socket error: %s\n", strerror(error));
    }
        
        if ( (n = read(sockfd,buffer,4096) ) <= 0 ) { 
            printf( "\nERROR reading from socket" );
            close(sockfd);
            exit(0);
        }
        printf("%s", buffer);
    }
   #endif

            memset(buffer, 0, sizeof(buffer)); //clear buffer
		    n = recv(sockfd, buffer, sizeof(buffer), 0);
		    if (n < 0){
       	          printf("\nError reading socket");
                  close(sockfd);
                  exit(0);
       		} else if (n == 0){
                printf("\nClosed connection");
                close(sockfd);
                exit(0);
            } else {
			    printf("%s", buffer);
            }
    }

    close( sockfd );
    return 0;
}
