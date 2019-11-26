ifneq ($V,1)
Q ?= @
endif

INCLUDES_COMMON= -I includes/

ifeq ($(CC),)
     CC = $(CROSS_COMPILE)gcc
endif
 
ifeq ($(CCFLAGS),)
     CCFLAGS  = -g -Wall $(INCLUDES_COMMON)
endif
 
ifeq ($(LDFLAGS),)
    LDFLAGS = -L/usr/local/lib -lwiringPi -lwiringPiDev -lpthread -lm -lcrypt -lrt   
endif

#TARGET	=	joystick.c     

SRC =   led_test.c         \
        joystick_test.c    \
        scroll.c      \
        main.c   

BINS    =   $(SRC:.c=)

all:	$(BINS)

CROSS_COMPILE:
	arm-unknown-linux-gnueabi-gcc joystick.c -o joystick $(LDFLAGS)

joystick:	joystick.c
	$(CC) $(CCFLAGS) -o joystick joystick.c $(LDFLAGS)

led:    led.c
	$(CC) $(CCFLAGS) -o led led.c $(LDFLAGS)  

scroll:    scroll.c
	$(CC) $(CCFLAGS) -o scroll scroll.c $(LDFLAGS)

test:   test.c
	$(CC) $(CCFLAGS) -o test test.c $(LDFLAGS)

main:   main.c
	$(CC) $(CCFLAGS) -o main main.c $(LDFLAGS)

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
