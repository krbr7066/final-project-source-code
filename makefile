# Author: Kristina Brunsgaard
#  Date: September 10, 2019
#  Program Name: writer.c  
#  Decription: Program takes file path and string as arguments, creates
#  file at specified file path and writes user entered string, github 
#  username, and date and time into file.

ifeq ($(CC),)
    CC = $(CROSS_COMPILE)gcc
endif

ifeq ($(CCFLAGS),)
    CCFLAGS  = -g -Wall
endif

ifeq ($(LDFLAGS),)
    LDFLAGS = -pthread -lrt -lwiringPi
endif

TARGET = ./server/aesdsocket

all: $(TARGET)

CROSS_COMPILE:
	arm-unknown-linux-gnueabi-gcc $(TARGET).c -o $(TARGET) 


$(TARGET): $(TARGET).c
	$(CC) $(CCFLAGS) -o $(TARGET) $(TARGET).c $(LDFLAGS)

clean:
	$(RM) $(TARGET)
	rm *.o 
