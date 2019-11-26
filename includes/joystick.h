#include <stdio.h>
#include <stdlib.h>

#ifndef JOYSTICK_H
#define JOYSTICK_H

typedef struct { 
   int xAxis; 
   int yAxis;
} joystick_t;

//extern joystick_t joyGlobal;

int setupJoystick();
void readJoystick();

#endif
