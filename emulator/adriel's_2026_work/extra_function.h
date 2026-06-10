// make the header’s contents be included only once in the same compilation unit
#ifndef EXTRA_FUNCTION_H
    #define EXTRA_FUNCTION_H
#endif

#include <pthread.h>
#include <stdio.h>
#include "../../data/buttons.h"
#include <stdbool.h>  // Required for the bool type, true, and false
#include <string.h>  
#include "system_window.h"  

//printButtonFunction: to print the key code of the pressed key for debugging purposes
//state: current page, key: hexadecimal number of keyboard pressed
void handleButtonFunction(char state[], int key);



