#
# Makefile:
#	wiringPi - A "wiring" library for the Raspberry Pi
#	https://projects.drogon.net/wiring-pi
#
#	Copyright (c) 2012-2015 Gordon Henderson
#################################################################################
# This file is part of wiringPi:
#	Wiring Compatable library for the Raspberry Pi
#
#    wiringPi is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    wiringPi is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
#################################################################################

ifneq ($V,1)
Q ?= @
endif

ifeq ($(CC),)
     CC = $(CROSS_COMPILE)gcc
endif
 
ifeq ($(CCFLAGS),)
     CCFLAGS  = -g -Wall
endif
 
ifeq ($(LDFLAGS),)
     LDFLAGS = -L/usr/local/lib -lwiringPi -lwiringPiDev -lpthread -lm -lcrypt -lrt
endif

DEBUG	= -O3

TARGET	=	joystick.c                          

OBJ	=	$(TARGET:.c=.o)

BINS	=	$(TARGET:.c=)

all:	joystick

#CROSS_COMPILE:
#	arm-unknown-linux-gnueabi-gcc joystick.c -o joystick $(LDFLAGS)

really-all:	$(BINS)

joystick:	joystick.c
	$Q echo [link]
	$Q $(CC) -o joystick joystick.c $(LDFLAGS)
#	$Q $(CC) -o $@ joystick.o $(LDFLAGS)
 
#$(LDLIBS)

#$(TARGET):	$(TARGET).c
#	$Q echo [link]
#	$Q $(CC) -o $@ $(TARGET).o $(LDFLAGS) 

.c.o:
	$Q echo [CC] $<
	$Q $(CC) -c $(CFLAGS) $< -o $@

clean:
	$Q echo "[Clean]"
	$Q rm -f $(OBJ) *~ core tags $(BINS)

tags:	$(TARGET)
	$Q echo [ctags]
	$Q ctags $(TARGET)

depend:
	makedepend -Y $(TARGET)

# DO NOT DELETE
