#include "led.c"
#include "joystick.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]){
    setupJoystick();
    setupLEDDisplay();

    
        if (atoi(argv[1]) == 1){
            printf("\nClear Screen");
            clearScreen();
        }
    return 0;
}
