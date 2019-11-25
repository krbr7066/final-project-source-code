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

#TARGET	=	joystick.c     

SRC =   led.c         \
        joystick.c    \
        scroll.c                 

BINS    =   $(SRC:.c=)

all:	$(BINS)

CROSS_COMPILE:
	arm-unknown-linux-gnueabi-gcc joystick.c -o joystick $(LDFLAGS)

joystick:	joystick.c
	$(CC) -o joystick joystick.c $(LDFLAGS)

led:    led.c
	$(CC) -o led led.c $(LDFLAGS)  

scroll:    scroll.c
	$(CC) -o scroll scroll.c $(LDFLAGS)

#$(TARGET):	$(TARGET).c
#	$(CC) $(CCFLAGS) -o $(TARGET) $(TARGET).c $(LDFLAGS) 

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
