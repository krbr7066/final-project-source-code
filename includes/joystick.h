/*
* Author: Kristina Brunsgaard
* Date: December 8, 2019
* File: joystick.h
*/

#include <stdio.h>
#include <stdlib.h>

#ifndef JOYSTICK_H
#define JOYSTICK_H

typedef struct { 
   int xAxis; 
   int yAxis;
   char *Dir;
} joystick_t;

//extern joystick_t joyGlobal;

int setupJoystick();
void readJoystick();

#endif
