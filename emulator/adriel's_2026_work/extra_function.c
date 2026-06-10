#include <stdio.h>
#include "extra_function.h"

// How to debug using int main(): gcc extra_function.c -I"WHERE_TO_OPEN_FILE" -o extra_function.exe

void handleButtonFunction(char state[], int key){
    char str2[] = "PAINTING_SPACE";
    //show what page you are in first
    printf("Current page: %s \n", state);
    //default page is painting space
    switch(key){
        case 0x31: //Button 1
            printf("You selected 'LOAD'.\n");
            break;
        case 0x32: //Button 2
            printf("You selected 'BRIGTHNESS CONTROL'.\n");
            break;
        case 0x33: //Button 3
            printf("You selected 'SAVE 9 TO RESET AFTER SAVED'.\n");
            break;
        case 0x34: //Button 4: Return to Programing Space or Color for foreground
            if(strcmp(state, str2) == 0){
                printf("You selected 'COLOR FOR FOREGROUND'.\n");
            }
            else{
                printf("You selected 'RETURN TO PROGRAMMING SPACE'.\n");
            }
            break;
        case 0x35: //Button 5: Result or Nil
            if(strcmp(state, str2) == 0){
                printf("You selected 'NIL'.\n");
            }
            else{
                printf("You selected 'RESULT'.\n");
            }
            break;
        case 0x36: //Button 6: Run simulation or Color for background
            if(strcmp(state, str2) == 0){
                printf("You selected 'COLOR FOR BACKGROUND'.\n");
            }
            else{
                printf("You selected 'RUN SIMULATION'.\n");
            }
            break;
        case 0x37: //Button 7: Clear or To Coding Space 
            if(strcmp(state, str2) == 0){
                printf("You selected 'TO CODING SPACE'.\n");
                strcpy(pageState, "CODING_SPACE"); //Change pageState to coding space
            }
            else{
                printf("You selected 'CLEAR'.\n");
            }
            break;
        case 0x38: //Button 8: Clear current page or Bucket fill
            if(strcmp(state, str2) == 0){
                printf("You selected 'BUCKET FILL'.\n");
            }
            else{
                printf("You selected 'CLEAR CURRENT PAGE'.\n");
            }
            break;
        case 0x39: //Button 9: To painting space or clear screen
            if(strcmp(state, str2) == 0){
                printf("You selected 'CLEAR SCREEN'.\n");
            }
            else{
                printf("You selected 'TO PAINTING SPACE'.\n");
                strcpy(pageState, "PAINTING_SPACE"); //Change pageState to painting space
            }
            break;
        default:
            printf("Invalid selection.\n");
    }
}

// int main() {
//     // Example usage of printButtonFunction
//     printButtonFunction("CODING_SPACE", 0x32); // Should print "Key code 65 pressed" (A key)
//     return 0;
// }