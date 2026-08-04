#include "main.h"

int main(void) {

    SystemInit();
    ADC_init();
    clear();
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
    Delay_Ms(delay);
    i2c_init();
    //printf("I2C Initialized\n");
    init_storage();
    // Hold button Y at startup to reset all paints
    JOY_sound(1000, 100);
    uint16_t delay_countdown = 50;
    while (delay_countdown-- > 0) {
        if (JOY_Y_pressed()) {
            erase_all_paint_saves();
            // Visual indication of paint save reset
            red_screen();
            #ifdef DEBUG_VERBOSE
            printf("Paint reset\n");
            printf("DEBUG: %d\n", __LINE__);
            #endif
            Delay_Ms(1000);
        }
        Delay_Ms(1);
    }

    print_status_storage();

    //app_selected app = rv_paint;

    //display_stored_paints();
    iconShow();

    Delay_Ms(delay*3);

    printf("Select App: %d\n",appChosen);
    appRunningRoutine();
    Delay_Ms(delay);
    while (1) {
        if (JOY_Y_pressed()) {
            NVIC_SystemReset();
        }
        Delay_Ms(200);
    }
}


//////////////////////////////////////////////////
//**********************************************//
//*************   App Selection   **************//
//**********************************************//
//////////////////////////////////////////////////
void appRunningRoutine(void){
    while (1) {
        switch (appChosen) {
            case rv_paint:
                painting_routine();
                break;
            #if !TESTING_MODE
            case rv_code:
                rv_code_routine();
                break;
            #endif
            default:
                red_screen();
                Delay_Ms(1000);
                break;
        }
    }
    printf("App Exited\n");
}



//////////////////////////////////////////////////
//**********************************************//
//****************  RV Code    ****************//
//**********************************************//
//////////////////////////////////////////////////

void rv_code_routine(void) {
    printf("Game Start\n");
    for (int i = 0; i < NUM_LEDS; i++) {
        if(i <= 7 && i >=4){
            if((8-i) == currentPage){
                canvas[i].layer = PAGEGROUND_LAYER;
                canvas[i].color.r=200;
            }
            else{
                canvas[i].layer = PAGEGROUND_LAYER;
                canvas[i].color = pageground;
            }

        }
        else{
            canvas[i].layer = CLEARROUND_LAYER;
            canvas[i].color = clearground;
        }
    }
    currentPage = 1;
    //toCodingSpace(currentPage);
    flushCanvas();
    while (1) {
        Delay_Ms(200);
        int8_t user_input = matrix_pressed_two();
        if (user_input == no_button_pressed) {
            if (JOY_1_pressed()) {
                printf("Enter Code loading screen!\n");
                choose_load_page(rv_code);
                Delay_Ms(500);
                printf("Exit Code loading screen!\n");
                //flushCanvas();

            } else if (JOY_2_pressed()){
                choose_led_brightness();
                Delay_Ms(1000);

            } else if (JOY_3_pressed()){
                // save paint
                for (int _code_line = 0; _code_line <_TOTAL_CODE_LINE; _code_line++) {
                   uint8_t _temp_page = _code_line/7;
                   uint8_t _temp_line = _code_line%7;
                   opCodeToStored[_code_line] = 0;
                   for (int i = 7; i >= 0; i--) {
                       if(opCodeStorage[_temp_page][_temp_line][i]>0)
                            if(i == 7)
                               opCodeToStored[_code_line] = opCodeToStored[_code_line]|0x80;
                            else if(i == 6)
                               opCodeToStored[_code_line] = opCodeToStored[_code_line]|0x40;
                            else if(i == 5)
                               opCodeToStored[_code_line] = opCodeToStored[_code_line]|0x20;
                            else if(i == 4)
                               opCodeToStored[_code_line] = opCodeToStored[_code_line]|0x10;
                            else if(i == 3)
                               opCodeToStored[_code_line] = opCodeToStored[_code_line]|0x08;
                            else if(i == 2)
                              opCodeToStored[_code_line] = opCodeToStored[_code_line]|0x04;
                            else if(i == 1)
                              opCodeToStored[_code_line] = opCodeToStored[_code_line]|0x02;
                            else if(i == 0)
                              opCodeToStored[_code_line] = opCodeToStored[_code_line]|0x01;
                   }
                }
                printf("Exit Coding mode, entering save\n");
                choose_save_page(rv_code);
                printf("Exit Code saving screen!\n");
                //flushCanvas();
                //break;
            } else if (JOY_4_pressed()){
                printf("Coding workspace\n");
                /*for (int _code_line = 0; _code_line <_TOTAL_CODE_LINE; _code_line++) {
                   uint8_t _temp_page = _code_line/7;
                   uint8_t _temp_line = _code_line%7;
                   for(int i = 7; i >= 0; i--){
                       printf("%d, ",opCodeStorage[_temp_page][_temp_line][i]);
                   }
                   printf("\n");
                }*/

                toCodingSpace(currentPage);

                //flushCanvas();
            } else if (JOY_5_pressed()){
                printf("Run Result\n");
                rvCodeRun(1);
                toCodingSpace(currentPage);
            } else if (JOY_6_pressed()){
                printf("Simulation workspace\n");
                printf("Run Program\n");
                rvCodeRun(0);
                toCodingSpace(currentPage);
            } else if (JOY_7_pressed()) {
                // save paint
                appChosen = rv_code;
                printf("Clear\n");
                Delay_Ms(500);
                break;
            } else if (JOY_8_pressed()){
                for (int _code_line = 0; _code_line <7; _code_line++) {
                   for(int i = 7; i >= 0; i--){
                       opCodeStorage[currentPage-1][_code_line][i]=0;
                   }
                }
                for (int i = 8; i < NUM_LEDS; i++) {
                   canvas[i].layer = CLEARROUND_LAYER;
                   canvas[i].color = clearground;
                }
                flushCanvas();
            } else if (JOY_9_pressed()){
                appChosen = rv_paint;
                printf("Exit paint mode, entering coding\n");
                Delay_Ms(500);
                break;
            }
            continue;
        }
        printf("User input: %d\n",user_input);

        // user sets canvas color
        if(user_input > 7){
            if(canvas[user_input].layer == CLEARROUND_LAYER){
                uint8_t code_line = (7-user_input/8);
                uint8_t code_bit = (user_input%8);
                canvas[user_input].layer = FOREGROUND_LAYER;

                if(code_bit < 3){
                    canvas[user_input].color = valueColor;
                } else{
                    canvas[user_input].color = opcodeColor;
                }
                //programStored[user_input] = 1;
                opCodeStorage[currentPage-1][code_line][code_bit] = 1;
            }
            else {
                canvas[user_input].layer = CLEARROUND_LAYER;
                canvas[user_input].color = clearground;
                //programStored[user_input] = 0;
                uint8_t code_line = (7-user_input/8);
                uint8_t code_bit = (user_input%8);
                opCodeStorage[currentPage-1][code_line][code_bit] = 0;
            }
            printf("Canvas[%d] set to R:%d G:%d B:%d\n", user_input, canvas[user_input].color.r, canvas[user_input].color.g, canvas[user_input].color.b);
            flushCanvas();
        }
        else if(user_input<8 && user_input>3){
            currentPage = (8-user_input);
            printf("Show Page %d | ", currentPage);
            toCodingSpace(currentPage);
            flushCanvas();
        }
    }
}


void rvCodeRun(uint8_t direct_result){
    //line one
    if(direct_result){
        timeout_var_code =1;
        timeout_line_code =1;
    }
    uint8_t opCode_line_storage[_TOTAL_CODE_LINE] = {0};
    uint8_t var_line_storage[_TOTAL_CODE_LINE] = {0};
    uint8_t opGrp_line_storage[_TOTAL_CODE_LINE] = {0};
    int8_t currentDirection = 8;
    pointerLocation = 36;
    //turtleBody = 28;
    int8_t rVariable = 7,gVariable=7,bVariable=7,xVariable=4,yVariable=4,loopVariable=0;
    uint8_t turtStatus = 1;
    uint8_t penStatus = 0;
    uint8_t line_run = 0;
    uint8_t var_run = 0;
    uint8_t jump_variable = 0;
    //uint8_t jump_var_flag = 0;
    uint16_t sound_freq = 1000;
    uint16_t sound_dur = 100;
    uint32_t timeout_f = timeout_flash;
    uint32_t timeout_lc = timeout_line_code;
    uint32_t timeout_varc = timeout_var_code;
    //uint8_t y_pos = 0;
    //uint8_t x_pos =0;
    char * ptr;
    for (ptr = (char *)rv_coding_board; ptr < (char *)(rv_coding_board + 64);
         ptr += sizeof(rvCodeParts)) {
        *(rvCodeParts *)ptr = (rvCodeParts){'0', rvClearColor};
    }
    printf("run here\n");
    for (int _code_line = 0; _code_line <_TOTAL_CODE_LINE; _code_line++) {
        uint8_t _temp_page = _code_line/7;
        uint8_t _temp_line = _code_line%7;
        opCode_line_storage[_code_line] = opCodeExtraction(opCodeStorage[_temp_page][_temp_line]);
        opGrp_line_storage[_code_line] = opGroupExtraction(opCodeStorage[_temp_page][_temp_line]);
        var_line_storage[_code_line] = varExtraction(opCodeStorage[_temp_page][_temp_line]);
        //if(opCode_line_storage[_code_line] > 0)
        printf("OP: %d | Line: %d, code: %d, var: %d\n",opGrp_line_storage[_code_line], _code_line,opCode_line_storage[_code_line], var_line_storage[_code_line]);
    }
    printf("enter loop\n");

    while (1){
        --timeout_f;
        --timeout_lc;
        /* 63 62 61 60 59 58 57 56
         * 55 54 53 52 51 50 49 48
         * 47 46 45 44 43 42 41 40
         * 39 38 37 36 35 34 33 32
         * 31 30 29 28 27 26 25 24
         * 23 22 21 20 19 18 17 16
         * 15 14 13 12 11 10 09 08
         * 07 06 05 04 03 02 01 00
         */
        if(timeout_lc == 0 && line_run <_TOTAL_CODE_LINE){
            //printf("Check lines %d of opgrp: %d | opcode: %d | value: %d\n", line_run,opGrp_line_storage[line_run],opCode_line_storage[line_run],var_line_storage[line_run]);
            //check opcode group
            if(opGrp_line_storage[line_run] == _OPCODE_MOVE){
                switch(opCode_line_storage[line_run]){
                    case _RVCODE_OPCODE_FD0:
                        currentDirection = _DIR_FD0;
                        break;
                    case _RVCODE_OPCODE_FD45:
                        currentDirection = _DIR_FD45;
                        break;
                    case _RVCODE_OPCODE_FD90:
                        currentDirection = _DIR_FD90;
                        break;
                    case _RVCODE_OPCODE_FD135:
                        currentDirection = _DIR_FD135;
                        break;
                    case _RVCODE_OPCODE_FD180:
                        currentDirection = _DIR_FD180;
                        break;
                    case _RVCODE_OPCODE_FD225:
                        currentDirection = _DIR_FD225;
                        break;
                    case _RVCODE_OPCODE_FD270:
                        currentDirection = _DIR_FD270;
                        break;
                    case _RVCODE_OPCODE_FD315:
                        currentDirection = _DIR_FD315;
                        break;
                    default:
                        currentDirection = _DIR_STOP;
                        break;
                }
                if(var_run == 0){
                    var_run = var_line_storage[line_run];
                    if(var_line_storage[line_run] == 0){
                        //printf("Line %d Code Done, Next Line | Head code %d\n", line_run, pointerLocation);
                        line_run++;
                        timeout_lc = timeout_line_code;
                     }
                }
            }
            else if(opGrp_line_storage[line_run] == _OPCODE_PEN){
                //printf("entered Pen Stage 1\n");
                switch(opCode_line_storage[line_run]){
                    case _RVCODE_OPCODE_TURT:
                        if(var_line_storage[line_run] == 1){
                            turtStatus = 1;
                        }
                        else{
                            turtStatus = 0;
                        }
                        break;
                    case _RVCODE_OPCODE_PROSPEED:
                        if(!direct_result){
                            timeout_var_code = 150 - ((int8_t)var_line_storage[line_run]-4)*40;
                            timeout_line_code = 300 - ((int8_t)var_line_storage[line_run]-4)*40;
                        }

                        break;
                    case _RVCODE_OPCODE_SOUNDDUR:
                        sound_dur = 50+var_line_storage[line_run]*100;
                        break;
                    case _RVCODE_OPCODE_SOUNDFREQ:
                        static const uint16_t noteTable[8] = { NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5 };
                        sound_freq = noteTable[var_line_storage[line_run] & 0x07];
                        // SAME AS BELOW: 
                        // switch(var_line_storage[line_run]){
                        //     case 0:
                        //         sound_freq = NOTE_C4;
                        //         break;
                        //     case 1:
                        //         sound_freq = NOTE_D4;
                        //         break;
                        //     case 2:
                        //         sound_freq = NOTE_E4;
                        //         break;
                        //     case 3:
                        //         sound_freq = NOTE_F4;
                        //         break;
                        //     case 4:
                        //         sound_freq = NOTE_G4;
                        //         break;
                        //     case 5:
                        //         sound_freq = NOTE_A4;
                        //         break;
                        //     case 6:
                        //         sound_freq = NOTE_B4;
                        //         break;
                        //     case 7:
                        //         sound_freq = NOTE_C5;
                        //         break;
                        //     default:
                        //         sound_freq = NOTE_C4;
                        //         break;
                        // }
                        JOY_sound(sound_freq,sound_dur);
                        break;
                    case _RVCODE_OPCODE_PENRGB:
                        if(var_line_storage[line_run]== 0){
                            penStatus = 0;
                        }
                        else{
                            penStatus = 1; 
                            updatePendownColorFromBits(var_line_storage[line_run], rVariable, gVariable, bVariable);
                            
                            /* SAME AS BELOW HERE:
                            if((var_line_storage[line_run]&0x04)==0x04)
                                rvPendownColor.r = 36*rVariable;
                            else
                                rvPendownColor.r = 0;
                            if((var_line_storage[line_run]&0x02)==0x02)
                                rvPendownColor.g = 36*gVariable;
                            else
                                rvPendownColor.g = 0;
                            if((var_line_storage[line_run]&0x01)==0x01)
                                rvPendownColor.b = 36*bVariable;
                            else
                                rvPendownColor.b = 0; 
                            */

                            #ifdef DEBUG_VERBOSE
                            printf("Leave Color R: %d, G: %d, B:%d\n",rvPendownColor.r, rvPendownColor.g, rvPendownColor.b);
                            #endif
                        }
                        break;
                    case _RVCODE_OPCODE_TURT_POS:
                        if(turtStatus == 1){
                            if(penStatus == 1){
                                rv_coding_board[pointerLocation] = (rvCodeParts){'0', rvPendownColor};
                            }
                            else{
                                rv_coding_board[pointerLocation] = (rvCodeParts){'0', rvClearColor};
                            }


                        }
                        switch(var_line_storage[line_run]){
                            case 0:
                                pointerLocation = 36;
                                break;
                            case 1:
                                pointerLocation = 56;
                                break;
                            case 2:
                                pointerLocation = 0;
                                break;
                            case 3:
                                pointerLocation = 7;
                                break;
                            case 4:
                                pointerLocation = 63;
                                break;
                            case 7:
                                pointerLocation = (yVariable * 8 + (7-xVariable));
                                break;
                            default:
                                //pointerLocation = pointerLocation;
                                break;
                        }
                        if(turtStatus == 1){
                            rv_coding_board[pointerLocation] = (rvCodeParts){'P', rvPointerColor};
                            logoDisplay();
                        }

                        break;
                    case _RVCODE_OPCODE_CLRSCREEN:
                        if((var_line_storage[line_run]&0x04)==0x04)
                            rvPendownColor.r = 36*rVariable;
                        else
                            rvPendownColor.r = 0;
                        if((var_line_storage[line_run]&0x02)==0x02)
                            rvPendownColor.g = 36*gVariable;
                        else
                            rvPendownColor.g = 0;
                        if((var_line_storage[line_run]&0x01)==0x01)
                            rvPendownColor.b = 36*bVariable;
                        else
                            rvPendownColor.b = 0;
                        for (ptr = (char *)rv_coding_board; ptr < (char *)(rv_coding_board + 64);
                             ptr += sizeof(rvCodeParts)) {
                            *(rvCodeParts *)ptr = (rvCodeParts){'0', rvPendownColor};
                        }
                        break;
                    default:
                        //currentDirection = _DIR_STOP;
                        break;
                }
                //printf("Pen Line %d Code Done, Next Line | Head code %d\n", line_run, pointerLocation);
                line_run++;
                timeout_lc = timeout_line_code;
            }
            else if(opGrp_line_storage[line_run] == _OPCODE_OPTION){
                switch(opCode_line_storage[line_run]){
                    case _RVCODE_OPCODE_LOADCODE:
                        break;
                    case _RVCODE_OPCODE_LOADMUSIC:
                        break;
                    case _RVCODE_OPCODE_LOADPAINT:
                        printf("Load Paint %d\n", var_line_storage[line_run]);
                        load_paint(var_line_storage[line_run], led_array, 1);

                        for (int i = 0; i < NUM_LEDS; i++) {
                            rv_coding_board[i] = (rvCodeParts){'x', led_array[i]};
                            //canvas[i].color = led_array[i];
                        }
                        //flushCanvas();
                        break;
                    case _RVCODE_OPCODE_JUMPP1:
                        line_run = (var_line_storage[line_run]-1);
                        printf("-----Jump1 to line %d\n", line_run);
                        break;
                    case _RVCODE_OPCODE_JUMPP2:
                        line_run = 7+(var_line_storage[line_run]-1);
                        printf("-----Jump2 to line %d\n", line_run);
                        break;
                    case _RVCODE_OPCODE_JUMPP3:
                        line_run = 14+(var_line_storage[line_run]-1);
                        printf("-----Jump3 to line %d\n", line_run);
                        break;
                    case _RVCODE_OPCODE_JUMPP4:
                        line_run = 21+(var_line_storage[line_run]-1);
                        printf("-----Jump4 to line %d\n", line_run);
                        break;
                    case _RVCODE_OPCODE_END:
                        printf("END OpCode\n");
                        line_run = 29;
                        break;
                    default:
                        //currentDirection = _DIR_STOP;
                        break;
                }
                //printf("Option Line %d Code Done, Next Line | Head code %d\n", line_run, pointerLocation);
                line_run++;
                timeout_lc = timeout_line_code;
            }
            else if(opGrp_line_storage[line_run] == _OPCODE_VARLOOP){
                int8_t jp_temp = 0;
                switch(opCode_line_storage[line_run]){
                    case _RVCODE_OPCODE_RVAR:
                        rVariable = var_line_storage[line_run];
                        //if(jump_variable == 0)
                            jump_variable = 1;
                        break;
                    case _RVCODE_OPCODE_GVAR:
                        gVariable = var_line_storage[line_run];
                        //if(jump_variable == 0)
                            jump_variable = 2;
                        break;
                    case _RVCODE_OPCODE_BVAR:
                        bVariable = var_line_storage[line_run];
                        //if(jump_variable == 0)
                            jump_variable = 3;
                        break;
                    case _RVCODE_OPCODE_XVAR:
                        xVariable = var_line_storage[line_run];
                        //if(jump_variable == 0)
                            jump_variable = 4;
                        break;
                    case _RVCODE_OPCODE_YVAR:
                        yVariable = var_line_storage[line_run];
                        //if(jump_variable == 0)
                            jump_variable = 5;
                        break;
                    case _RVCODE_OPCODE_LOOPVAR:
                        loopVariable = var_line_storage[line_run];
                        //if(jump_variable == 0)
                            jump_variable = 6;
                        break;
                    case _RVCODE_OPCODE_MINUSSKIP:
                        switch(jump_variable){
                            case 1:
                                jp_temp = rVariable;
                                break;
                            case 2:
                                jp_temp = gVariable;
                               break;
                            case 3:
                                jp_temp = bVariable;
                                break;
                            case 4:
                                jp_temp = xVariable;
                               break;
                            case 5:
                                jp_temp = yVariable;
                               break;
                            case 6:
                                jp_temp = loopVariable;
                               break;
                            default:
                                break;
                        }
                        jp_temp-=var_line_storage[line_run];
                        //printf("LOOP minus at: %d\n",jp_temp);
                        if(jp_temp < 0){
                            line_run++;
                            //jump_var_flag = 0;
                            //printf("Skip the line %d\n",line_run);
                        }
                        switch(jump_variable){
                            case 1:
                                rVariable = jp_temp;
                                break;
                            case 2:
                                gVariable = jp_temp;
                               break;
                            case 3:
                                bVariable = jp_temp;
                                break;
                            case 4:
                                xVariable = jp_temp;
                               break;
                            case 5:
                                yVariable = jp_temp;
                               break;
                            case 6:
                                loopVariable = jp_temp;
                               break;
                            default:
                                break;
                        }
                        break;
                    case _RVCODE_OPCODE_ADDSKIP:
                        switch(jump_variable){
                            case 1:
                                jp_temp = rVariable;
                                break;
                            case 2:
                                jp_temp = gVariable;
                               break;
                            case 3:
                                jp_temp = bVariable;
                                break;
                            case 4:
                                jp_temp = xVariable;
                               break;
                            case 5:
                                jp_temp = yVariable;
                               break;
                            case 6:
                                jp_temp = loopVariable;
                               break;
                            default:
                                break;
                        }
                        jp_temp+=var_line_storage[line_run];
                        //printf("LOOP add ast: %d\n",jp_temp);
                        if(jp_temp>7){
                            line_run++;
                            //jump_var_flag = 0;
                            //printf("Skip the line %d\n",line_run);
                        }
                        switch(jump_variable){
                            case 1:
                                rVariable = jp_temp;
                                break;
                            case 2:
                                gVariable = jp_temp;
                               break;
                            case 3:
                                bVariable = jp_temp;
                                break;
                            case 4:
                                xVariable = jp_temp;
                               break;
                            case 5:
                                yVariable = jp_temp;
                               break;
                            case 6:
                                loopVariable = jp_temp;
                               break;
                            default:
                                break;
                        }
                        break;
                    default:
                        //currentDirection = _DIR_STOP;
                        break;
                }
                //printf("Loop Line %d Code Done, Next Line | Head code %d\n", line_run, pointerLocation);
                line_run++;
                timeout_lc = timeout_line_code;
            }
            /*else {
                //printf("Else Line %d Code Done, Next Line | Head code %d\n", line_run, pointerLocation);
                line_run++;
                timeout_lc = timeout_line_code;
            }*/

        }
        if(opGrp_line_storage[line_run] == _OPCODE_MOVE){
            //value round up check finished
            if(var_run>0){
                --timeout_varc;
                if(timeout_varc == 0){
                    if(penStatus == 1){
                       rv_coding_board[pointerLocation] = (rvCodeParts){'x', rvPendownColor};
                    }
                    else{
                        rv_coding_board[pointerLocation] = (rvCodeParts){'0', rvClearColor};
                    }
                    if((pointerLocation+currentDirection)<64 && pointerLocation+currentDirection>=0){
                        //printf("TH+: %d | TH %d\n", (pointerLocation+currentDirection)/8, (pointerLocation)/8);
                        if(currentDirection == _DIR_FD90 || currentDirection == _DIR_FD270){
                            if((pointerLocation+currentDirection)/8 == (pointerLocation)/8){
                                pointerLocation += currentDirection;
                            }
                        }
                        else if(currentDirection == _DIR_FD45 || currentDirection == _DIR_FD135){
                            if(pointerLocation%8!=0){
                                pointerLocation += currentDirection;
                            }
                        }
                        else if(currentDirection == _DIR_FD225 || currentDirection == _DIR_FD315){
                            if(pointerLocation%8!=7){
                                pointerLocation += currentDirection;
                            }
                        }
                        else{
                            pointerLocation += currentDirection;
                        }

                    }

                    var_run--;
                    timeout_varc = timeout_var_code;
                    //show map
                    /*for(int i = 63; i >0; i--){
                        printf("%c, ",rv_coding_board[i].part);
                        if(i%8==0){
                            printf("\n");
                        }
                    }*/
                    if(var_run == 0){
                        printf("Moving Group - Line code ran %d | Head num: %d\n", line_run, pointerLocation);
                        line_run++;
                        timeout_lc = timeout_line_code;
                    }
                }
            }
        }
        if(timeout_f == (timeout_flash/2)){
            if(turtStatus == 1){
                //printf("Print Pointer show\n");
                rv_coding_board[pointerLocation] = (rvCodeParts){'P', rvPointerColor};

            }
            logoDisplay();
        } else if(timeout_f == 0){
           /* if(turtStatus == 1){
                printf("Print Pointer hide\n");
                rv_coding_board[pointerLocation] = (rvCodeParts){'0', pointer_status};
                //logoDisplay();
            }*/
            if(penStatus == 1){
              rv_coding_board[pointerLocation] = (rvCodeParts){'x', rvPendownColor};
            }
            else{
              rv_coding_board[pointerLocation] = (rvCodeParts){'0', rvClearColor};
            }


            logoDisplay();
            timeout_f = timeout_flash;
        }


        if (JOY_4_pressed()) {
            printf("Break the loop\n");
            break;
        }

    }
}

/// @brief Do OR operation with each case such as `0x10 | 0x01 --> 0b11`
uint8_t opGroupExtraction(uint8_t received_message[8]){
    return (received_message[7] ? 0x02 : 0) | (received_message[6] ? 0x01 : 0);
    /* SAME AS BELOW: 
    uint8_t opcodeGroup = 0;
    for (int i = 7; i > 5; i--) {
        if(received_message[i]>0)
            if(i == 7){
                opcodeGroup = opcodeGroup|0x02;
            }
            else if(i == 6){
                opcodeGroup = opcodeGroup|0x01;
            }
    }
    return opcodeGroup; */
}

/// @brief Do OR operation with each case that will result 5-bits value
uint8_t opCodeExtraction(uint8_t received_message[8]){
    uint8_t code = 0;
    for (int i = 3; i <= 7; i++) code |= (received_message[i] ? (1 << (i - 3)) : 0);
    return code;
    /* SAME AS BELOW
        uint8_t extracted_code = 0;
        for (int i = 7; i > 2; i--) {
            if(received_message[i]>0)
                if(i == 7){
                    extracted_code = extracted_code|0x10;
                }
                else if(i == 6){
                    extracted_code = extracted_code|0x08;
                }
                else if(i == 5)
                    extracted_code = extracted_code|0x04;
                else if(i == 4)
                    extracted_code = extracted_code|0x02;
                else if(i == 3)
                    extracted_code = extracted_code|0x01;
        }
        return extracted_code;
    */
}
/// @brief Do OR operation with each case that will result 3-bits value
uint8_t varExtraction(uint8_t received_message[8]){
    uint8_t v = 0;
    for (int i = 0; i <= 2; i++) v |= (received_message[i] ? (1 << i) : 0);
    return v;
    /* SAME AS BELOW HERE: 
    uint8_t extracted_var = 0;
    for (int i = 2; i >= 0; i--) {
        if(received_message[i]>0)
            if(i == 2)
                extracted_var = extracted_var|0x04;
            else if(i == 1)
                extracted_var = extracted_var|0x02;
            else if(i == 0)
                extracted_var = extracted_var|0x01;
    }
    return extracted_var;
    */
}
void toCodingSpace(uint8_t curr_page){
   printf("Coding workspace Page %d\n", curr_page);
   /*for(int i = 63; i >0; i--){
       printf("%c, ",rv_coding_board[i].part);
       if(i%8==0){
           printf("\n");
       }
   }*/
   clear();
   for (int i = 0; i < 7; i++) {
       for (int j = 7; j >= 0; j--){
           if(opCodeStorage[curr_page-1][i][j] >0){
               canvas[(56-i*8+j)].layer = opCodeStorage[curr_page-1][i][j];
               if(j>2)
                   canvas[(56-i*8+j)].color = opcodeColor;
               else
                   canvas[(56-i*8+j)].color = valueColor;
           }
           else{
               canvas[(56-i*8+j)].layer = CLEARROUND_LAYER;
               canvas[(56-i*8+j)].color = clearground;
           }
       }
   }
   for (int i = 4; i <= 7; i++){
       canvas[i].layer = PAGEGROUND_LAYER;
       canvas[i].color = pageground;
       if((8-i)==currentPage){
           canvas[i].color.r = 200;
       }
   }
   flushCanvas();
}

/* 63 62 61 60 59 58 57 56
 * 55 54 53 52 51 50 49 48
 * 47 46 45 44 43 42 41 40
 * 39 38 37 36 35 34 33 32
 * 31 30 29 28 27 26 25 24
 * 23 22 21 20 19 18 17 16
 * 15 14 13 12 11 10 09 08
 * 07 06 05 04 03 02 01 00
 */

void logoDisplay(void){
    clear();
    for (int i = 0; i < 64; i++) {
        set_color(i, rv_coding_board[i].current_color, brightness_divisor);
    }
    /*for(int i = 63; i >0; i--){
        printf("%c, ",rv_coding_board[i].part);
        if(i%8==0){
            printf("\n");
        }
    }*/
    //printf("\n");
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
}

//////////////////////////////////////////////////
//**********************************************//
//****************  RV Paint    ****************//
//**********************************************//
//////////////////////////////////////////////////

void painting_routine(void) {
    for (int i = 0; i < NUM_LEDS; i++) {
        canvas[i].layer = CLEARROUND_LAYER;
        canvas[i].color = clearground;
    }
    flushCanvas();
    while (1) {
        Delay_Ms(200);
        int8_t user_input = matrix_pressed_two();
        if (user_input == no_button_pressed) {

            if (JOY_1_pressed()) {
                #ifdef DEBUG_VERBOSE
                printf("Enter paint loading screen!\n");
                #endif

                choose_load_page(rv_paint);
                Delay_Ms(1000);

                #ifdef DEBUG_VERBOSE
                printf("Exit paint loading screen!\n");
                #endif
            }
            else if (JOY_2_pressed()) {
                // save paint
                //printf("Enter Brightness mode\n");
                choose_led_brightness();
                 Delay_Ms(1000);
                // printf("Exit Brightness mode\n");
                //break;
            }
            else if (JOY_3_pressed()) {
                // save paint
                printf("Exit paint mode, entering save\n");
                choose_save_page(rv_paint);
                 Delay_Ms(1000);
                 printf("Exit paint Saving screen!\n");
                //break;
            }
            else if (JOY_4_pressed()) {
                colorPaletteSelection(&foreground);
            }
            else if (JOY_5_pressed()) {
                chooseGameMode(brightness_divisor);
                Delay_Ms(200);
                switch(playGameMode){
                    case ADDITION_GAME:
                        initAdditionGameHW(brightness_divisor);
                        break;
                    case BINARY_GAME:
                        initBinaryGameHW(brightness_divisor);
                        break;
                }
                // Reset the value
                playGameMode = CHOOSE_GAME;
            }
            else if (JOY_6_pressed()) {
                colorPaletteSelection(&background);
            }

            else if (JOY_7_pressed()) {
                // save paint
                appChosen = rv_code;
                printf("Exit paint mode, entering coding\n");
                Delay_Ms(500);
                break;
            }
            else if (JOY_8_pressed()) {
                bucketFill();
                /*for (int i = 0; i < NUM_LEDS; i++) {
                   canvas[i].layer = CLEARROUND_LAYER;
                   canvas[i].color = clearground;
                }
                flushCanvas();*/
            }
            else if (JOY_9_pressed()) {
                // save paint
                appChosen = rv_paint;
                printf("Clear\n");
                Delay_Ms(500);
                break;
            }

            continue;
        }
        printf("User input: %d\n",user_input);
        // user sets canvas color
        if(canvas[user_input].layer == CLEARROUND_LAYER){
            canvas[user_input].layer = FOREGROUND_LAYER;
            canvas[user_input].color = foreground;
        }
        else if (canvas[user_input].layer == FOREGROUND_LAYER) {
            canvas[user_input].layer = BACKGROUND_LAYER;
            canvas[user_input].color = background;
        }
        else {
            canvas[user_input].layer = CLEARROUND_LAYER;
            canvas[user_input].color = clearground;
        }
        printf("Canvas[%d] set to %s layer\n", user_input,
            canvas[user_input].layer == FOREGROUND_LAYER ? "FOREGROUND" : canvas[user_input].layer == BACKGROUND_LAYER ? "BACKGROUND":"CLEARGROUND");
        printf("Canvas color set to R:%d G:%d B:%d\n", canvas[user_input].color.r,
            canvas[user_input].color.g, canvas[user_input].color.b);
        /*for (int i = 0; i < NUM_LEDS; i++) {
            printf("Canva: %d\n",canvas[i].color);
        }*/
        flushCanvas();
    }
}

void iconShow(void){
    clear();
    int8_t current_display_icon = 0;
    uint16_t _icon_page_no = current_display_icon * sizeof_paint_data_aspage + app_icon_page_no;

    if (!is_page_used(_icon_page_no + page_status_addr_begin) || !is_page_used(_icon_page_no + page_status_addr_begin + 1) || !is_page_used(_icon_page_no + page_status_addr_begin + 2)) {
        printf("Icon %d not found\n", _icon_page_no / 3);
        fill_logo();
    }
    else {
        printf("Displaying icon %d\n", _icon_page_no);
        load_paint(_icon_page_no / sizeof_paint_data_aspage, led_array, 1);
        for (int i = 0; i < NUM_LEDS; i++){
            led_array[i].r = led_array[i].r / 10;
            led_array[i].g = led_array[i].g / 10;
            led_array[i].b = led_array[i].b / 10;
        }
    }
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
}










//////////////////////////////////////////////////
//**********************************************//
//*****************  Storage   *****************//
//**********************************************//
//////////////////////////////////////////////////

void init_storage(void) {
    if (!is_storage_initialized()) {
        reset_storage();
        printf("Storage initialized\n");
    }
    else {
        printf("Storage already initialized\n");
    }
}

uint8_t is_storage_initialized(void) {
    // Creates a temporary buffer to hold bytes read from EEPROM
    uint8_t data[init_status_reg_size];
    // Reads (retrieve information) init_status_reg_size bytes starting at init_status_addr_begin from the EEPROM into data
    i2c_read(EEPROM_ADDR, init_status_addr_begin, I2C_REGADDR_2B, data, init_status_reg_size);
    // Check if this EEPROM block has already been initialized
    for (uint8_t i = 0; i < init_status_reg_size; i++) {
        if (data[i] != *(init_status_data + i)) {
            return 0;
        }
    }
    // If success, this means EEPROM contents look valid and already initialized
    return 1;
}

void reset_storage(void) {
    // Writes init_status_data into the initialization-signature area, which marks the storage as “initialized.”
    i2c_write(EEPROM_ADDR, init_status_addr_begin, I2C_REGADDR_2B, init_status_data,
        init_status_reg_size);
    Delay_Ms(3);
    // Loops through every page-status byte and writes 0, make it uninitialized
    for (uint16_t addr = page_status_addr_begin;
         addr < page_status_addr_begin + page_status_reg_size; addr++) {
        i2c_write(EEPROM_ADDR, addr, I2C_REGADDR_2B, (uint8_t[]){0}, sizeof(uint8_t));
        Delay_Ms(3);
    }
    #ifdef DEBUG_VERBOSE
    printf("Storage reset\n");
    #endif
}

void print_status_storage(void) {
    #ifdef DEBUG_VERBOSE
    printf("Status storage data:\n");
    #endif
    for (uint16_t addr = init_status_addr_begin;
         addr < init_status_addr_begin + init_status_reg_size; addr++) {
        uint8_t data = 0;
        i2c_read(EEPROM_ADDR, addr, I2C_REGADDR_2B, &data, sizeof(data));
        // Prints the initialization status bytes, one by one
        #ifdef DEBUG_VERBOSE
        printf(" %d: ", addr);
        printf(init_status_format, data);
        #endif
    }
    printf("\n");
    for (uint16_t addr = page_status_addr_begin;
         addr < page_status_addr_begin + page_status_reg_size; addr++) {
        uint8_t data = 0;
        i2c_read(EEPROM_ADDR, addr, I2C_REGADDR_2B, &data, sizeof(data));
        
        #ifdef DEBUG_VERBOSE
        if (data) {
            printf("%d ", addr);
        }
        else {
            printf("    ");
        }
        if ((addr + 1) % matrix_hori == 0) {
            printf("\n");
        }
        #endif
    }
    printf("\n");
}

void set_page_status(uint16_t page_no, uint8_t status) {
    if (status > 1) {
        #ifdef DEBUG_VERBOSE
        printf("Invalid status %d\n", status);
        printf("DEBUG: %d\n", __LINE__);
        #endif

        while (1)
            ;
    }
    validatePageNo(page_no);
    
    i2c_write(EEPROM_ADDR, page_no, I2C_REGADDR_2B, &status, sizeof(status));
    Delay_Ms(3);
    //printf("Page %d status set to %d\n", page_no, status);
}

uint8_t is_page_used(uint16_t page_no) {
    validatePageNo(page_no);
    uint8_t data = 0;
    i2c_read(EEPROM_ADDR, page_no, I2C_REGADDR_2B, &data, sizeof(data));
    //printf("Page %d is %s\n", page_no, data ? "used" : "empty");
    return data;
}

uint16_t calculate_page_no(uint16_t paint_no, uint8_t is_icon) {
    if (is_icon==1) {
        return (paint_no + app_icon_page_no) * sizeof_paint_data_aspage +
               paint_addr_begin;
    }
    else {
        return paint_no * sizeof_opcode_data_aspage +
               opcode_addr_begin;
    }
}

static void save_data(uint16_t item_no, uint16_t max_no, uint8_t is_icon,
                       uint16_t page_no_start, uint16_t page_count,
                       uint8_t *data, uint16_t data_size) {
    if (item_no > max_no) {
        #ifdef DEBUG_VERBOSE
        printf("Invalid item number %d\n", item_no);
        #endif
        while (1);
    }
    for (uint16_t i = page_no_start; i < page_no_start + page_count; i++) {
        if (is_page_used(i)) {
            #ifdef DEBUG_VERBOSE
            printf("Item %d already used, overwriting\n", item_no);
            #endif
            Delay_Ms(500);
        }
        set_page_status(i, 1);
    }
    i2c_result_e err = i2c_write_pages(EEPROM_ADDR, page_no_start * page_size,
        I2C_REGADDR_2B, data, data_size);
    #ifdef DEBUG_VERBOSE
    printf("Save result: %d\n", err);
    #endif
    Delay_Ms(3);
}

static void save_paint(uint16_t paint_no, color_t * data, uint8_t is_icon) {
    uint16_t page_no_start = calculate_page_no(paint_no, is_icon);
    save_data(paint_no, paint_addr_end, is_icon, page_no_start, sizeof_paint_data_aspage,
              (uint8_t *)data, sizeof_paint_data);
    // if (paint_no < 0 || paint_no > paint_addr_end) {
    //     #ifdef DEBUG_VERBOSE
    //     printf("Invalid paint number %d\n", paint_no);
    //     printf("DEBUG: %d\n", __LINE__);
    //     #endif
    //     while (1)
    //         ;
    // }
    // uint16_t page_no_start = calculate_page_no(paint_no, is_icon);
    // for (uint16_t i = page_no_start; i < page_no_start + sizeof_paint_data_aspage; i++) {
    //     if (is_page_used(i)) {
    //         #ifdef DEBUG_VERBOSE
    //         printf("Paint %d already used, overwriting\n", paint_no);
    //         #endif
    //         Delay_Ms(500);
    //     }
    //     set_page_status(i, 1);
    // }
    // i2c_result_e err = i2c_write_pages(EEPROM_ADDR, page_no_start * page_size,
    //     I2C_REGADDR_2B, (uint8_t *)data, sizeof_paint_data);
    
    // #ifdef DEBUG_VERBOSE
    // printf("Save paint result: %d\n", err);
    // #endif
    // Delay_Ms(3);
    // #ifdef DEBUG_VERBOSE
    // printf("Paint %d saved\n", paint_no);
    // #endif
}


void save_opCode(uint16_t opcode_no, uint8_t * data) {
    uint16_t page_no_start = calculate_page_no(opcode_no, 0);
    save_data(opcode_no, page_status_addr_end, 0, page_no_start, sizeof_opcode_data_aspage,
              data, sizeof_opcode_data);
//     if (opcode_no < 0 || opcode_no > page_status_addr_end) {
//         #ifdef DEBUG_VERBOSE
//         printf("Invalid paint number %d\n", opcode_no);
//         printf("DEBUG: %d\n", __LINE__);
//         #endif
//         while (1);
//     }
//     uint16_t page_no_start = calculate_page_no(opcode_no, 0);
//     for (uint16_t i = page_no_start; i < page_no_start + sizeof_opcode_data_aspage; i++) {
//         if (is_page_used(i)) {

//             #ifdef DEBUG_VERBOSE
//             printf("Opcode %d already used, overwriting\n", opcode_no);
//             #endif

//             Delay_Ms(500);
//         }
//         set_page_status(i, 1);
//     }
//     i2c_result_e err = i2c_write_pages(EEPROM_ADDR, page_no_start * page_size,
//         I2C_REGADDR_2B, (uint8_t *)data, sizeof_opcode_data);
    
//     #ifdef DEBUG_VERBOSE
//     printf("Save Opcode result: %d\n", err);
//     #endif
//     Delay_Ms(3);
//     #ifdef DEBUG_VERBOSE
//     printf("Opcode %d saved\n", opcode_no);
//     #endif
}

void load_paint(uint16_t paint_no, color_t * data, uint8_t is_icon) {
    if (paint_no < 0 || paint_no > paint_addr_end) {
        #ifdef DEBUG_VERBOSE
        printf("Invalid paint number %d\n", paint_no);
        printf("DEBUG: %d\n", __LINE__);
        #endif
        while (1)
            ;
    }
    uint16_t page_no_start = calculate_page_no(paint_no, is_icon);
    #ifdef DEBUG_VERBOSE
    printf("Loading paint_no %d from page %d, is_icon: %d\n", paint_no, page_no_start,
        is_icon);
    #endif
    if (!is_page_used(page_no_start)) {
        #ifdef DEBUG_VERBOSE
        printf("Paint %d not found\n", paint_no);
        printf("DEBUG: %d\n", __LINE__);
        #endif
        while (1)
            ;
    }
    i2c_result_e err = i2c_read_pages(EEPROM_ADDR, page_no_start * page_size,
        I2C_REGADDR_2B, (uint8_t *)data, sizeof_paint_data);
    
    #ifdef DEBUG_VERBOSE
    printf("Load paint result: %d\n", err);
    #endif
    Delay_Ms(3);
    #ifdef DEBUG_VERBOSE
    printf("Paint %d loaded\n", paint_no);
    #endif
}

void load_opCode(uint16_t opcode_no, uint8_t * data) {
    if (opcode_no < 0 || opcode_no > page_status_addr_end) {
        #ifdef DEBUG_VERBOSE
        printf("Invalid paint number %d\n", opcode_no);
        printf("DEBUG: %d\n", __LINE__);
        #endif
        while (1)
            ;
    }
    uint16_t page_no_start = calculate_page_no(opcode_no, 0);
    #ifdef DEBUG_VERBOSE
    printf("Loading paint_no %d from page %d, is_icon: %d\n", opcode_no, page_no_start,0);
    #endif
    if (!is_page_used(page_no_start)) {
        #ifdef DEBUG_VERBOSE
        printf("Paint %d not found\n", opcode_no);
        printf("DEBUG: %d\n", __LINE__);
        #endif
        while (1)
            ;
    }
    i2c_result_e err = i2c_read_pages(EEPROM_ADDR, page_no_start * page_size,
        I2C_REGADDR_2B, (uint8_t *)data, sizeof_opcode_data);
    #ifdef DEBUG_VERBOSE
    printf("Load paint result: %d\n", err);
    #endif
    Delay_Ms(3);
    #ifdef DEBUG_VERBOSE
    printf("Paint %d loaded\n", opcode_no);
    #endif
}

void any_paint_exist(uint8_t * paint_exist) {
    for (uint16_t _paint_page_no = paint_page_no;
         _paint_page_no < paint_page_no_max + paint_page_no;
         _paint_page_no += sizeof_paint_data_aspage) {
        if (is_page_used(_paint_page_no + paint_addr_begin) &&
            is_page_used(_paint_page_no + paint_addr_begin + 1) &&
            is_page_used(_paint_page_no + paint_addr_begin + 2)) {
            *paint_exist = 1;
            return;
        }
    }
    *paint_exist = 0;
}

void any_opcode_exist(uint8_t * opcode_exist) {
    for (uint16_t _opcode_page_no = opcode_page_no;
         _opcode_page_no < opcode_page_no_max + opcode_page_no;
         _opcode_page_no += sizeof_paint_data_aspage) {
        if (is_page_used(_opcode_page_no + opcode_addr_begin)) {
            *opcode_exist = 1;
            return;
        }
    }
    *opcode_exist = 0;
}

void choose_load_page(app_selected app_current) {
    led_display_paint_page_status(app_current);
    int8_t button = no_button_pressed;
	uint8_t _sizeof_data_aspage = 24, _page_no = 24, _page_addr_begin = 8;
	if(app_current == rv_paint){
		_sizeof_data_aspage = sizeof_paint_data_aspage;
		_page_no = paint_page_no;
		_page_addr_begin = paint_addr_begin;
	} else if(app_current == rv_code){
		_sizeof_data_aspage = sizeof_opcode_data_aspage;
		_page_no = opcode_page_no;
		_page_addr_begin = opcode_addr_begin;
	}
    while (1) {
        button = matrix_pressed_two();
        if (button != no_button_pressed) {
            if (!is_page_used(button * _sizeof_data_aspage + _page_no +
                             _page_addr_begin)) {
                //printf("Page %d is not used\n", button);
                // Fill the screen with red to indicate error
                fill_color((color_t){.r = 100, .g = 0, .b = 0});
                WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
                Delay_Ms(1000);
                led_display_paint_page_status(app_current);
                continue;
            }

            #ifdef DEBUG_VERBOSE
            printf("Selected page %d\n", button);
            #endif

            /*if(appChosen == rv_paint)
                load_paint(button, led_array, 1);
            else if(appChosen == rv_code)
                load_opCode(button, opCodeToStored);*/

            // Put led_array to canvas
            if(app_current == rv_paint){
                load_paint(button, led_array, 1);
                for (int i = 0; i < NUM_LEDS; i++) {
                    canvas[i].color = led_array[i];
                }
                //flushCanvas();
            }
            else if(app_current == rv_code){
                load_opCode(button, opCodeToStored);
                for (int i = 0; i < sizeof(opCodeToStored); i++) {
                    opCodeStorage[i/7][i%7][0] = ((opCodeToStored[i]&0x01));
                    opCodeStorage[i/7][i%7][1] = ((opCodeToStored[i]&0x02)>>1);
                    opCodeStorage[i/7][i%7][2] = ((opCodeToStored[i]&0x04)>>2);
                    opCodeStorage[i/7][i%7][3] = ((opCodeToStored[i]&0x08)>>3);
                    opCodeStorage[i/7][i%7][4] = ((opCodeToStored[i]&0x10)>>4);
                    opCodeStorage[i/7][i%7][5] = ((opCodeToStored[i]&0x20)>>5);
                    opCodeStorage[i/7][i%7][6] = ((opCodeToStored[i]&0x40)>>6);
                    opCodeStorage[i/7][i%7][7] = ((opCodeToStored[i]&0x80)>>7);
                }
                currentPage = 1;
                toCodingSpace(currentPage);
            }

            #ifdef DEBUG_VERBOSE
            printf("Paint load\n");
            #endif

            Delay_Ms(1000);
            break;
        }
        else{
            if (JOY_9_pressed()){
                #ifdef DEBUG_VERBOSE
                printf("Exit Loading\n");
                #endif

                break;
            }
        }
        Delay_Ms(200);
    }
    flushCanvas();
    //flushCanvas();
    /*for (int i = 0; i < NUM_LEDS; i++) {
        set_color_no_div(i, canvas[i].color);
    }
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);*/
}


void choose_save_page(app_selected app_current) {
    led_display_paint_page_status(app_current);
    int8_t button = no_button_pressed;
	uint8_t _sizeof_data_aspage = 24, _page_no = 24, _page_addr_begin = 8;
	if(app_current == rv_paint){
		_sizeof_data_aspage = sizeof_paint_data_aspage;
		_page_no = paint_page_no;
		_page_addr_begin = paint_addr_begin;
	} else if(app_current == rv_code){
		_sizeof_data_aspage = sizeof_opcode_data_aspage;
		_page_no = opcode_page_no;
		_page_addr_begin = opcode_addr_begin;
	}
    while (1) {
        button = matrix_pressed_two();
        if (button != no_button_pressed) {
            if (is_page_used(button * _sizeof_data_aspage + _page_no +
                             _page_addr_begin)) {
                //printf("Page %d already used\n", button);
                // Overwrite save
            }
            //printf("Selected page %d\n", button);
            // Put canvas to led_array
            for (int i = 0; i < NUM_LEDS; i++) {
                set_color_no_div(i, canvas[i].color);
            }

            if(app_current == rv_paint)
                save_paint(button, led_array, 1);
            else if(app_current == rv_code)
                save_opCode(button, opCodeToStored);

            #ifdef DEBUG_VERBOSE
            printf("Paint saved\n");
            #endif

            Delay_Ms(1000);
            break;
        }
        else{
            if (JOY_9_pressed()){
                #ifdef DEBUG_VERBOSE
                printf("Exit Saving\n");
                #endif

                break;
            }
        }
        Delay_Ms(200);
    }
    flushCanvas();
    //clear();
    //WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
}

void led_display_paint_page_status(app_selected app_current) {
    clear();
    if(app_current == rv_paint){
        for (uint16_t _paint_page_no = paint_page_no;
             _paint_page_no < paint_page_no_max + paint_page_no;
             _paint_page_no += sizeof_paint_data_aspage) {
            if (is_page_used(_paint_page_no + paint_addr_begin) &&
                is_page_used(_paint_page_no + paint_addr_begin + 1) &&
                is_page_used(_paint_page_no + paint_addr_begin + 2)) {
                set_color((_paint_page_no - paint_page_no) / sizeof_paint_data_aspage,
                    color_savefile_exist,normal_brightness_divisor);
            }
            else {
                set_color((_paint_page_no - paint_page_no) / sizeof_paint_data_aspage,
                    color_savefile_empty,normal_brightness_divisor);
            }
            //printf("Paint page number: %d\n", _paint_page_no);
        }
    }
    if(app_current == rv_code){
        for (uint16_t _opcode_page_no = opcode_page_no;
             _opcode_page_no < opcode_page_no_max + opcode_page_no;
             _opcode_page_no += sizeof_opcode_data_aspage) {
            if (is_page_used(_opcode_page_no + opcode_addr_begin)) {
                set_color((_opcode_page_no - opcode_page_no) / sizeof_opcode_data_aspage,
                    color_savefile_exist,normal_brightness_divisor);
            }
            else {
                set_color((_opcode_page_no - opcode_page_no) / sizeof_opcode_data_aspage,
                    color_savefile_empty,normal_brightness_divisor);
            }
        }
    }


    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
}

void erase_all_paint_saves(void) {
    // Set status of paint pages to 0
    for (uint16_t _paint_page_no = paint_page_no + page_status_addr_begin;
         _paint_page_no < paint_page_no_max + paint_page_no; _paint_page_no++) {
        set_page_status(_paint_page_no, 0);
        //printf("Page is now status: %d\n", is_page_used(_paint_page_no));
        Delay_Ms(3);
    }

    #ifdef DEBUG_VERBOSE
    printf("All paint saves status erased\n");
    #endif

    // Erase existing data to 0
    for (uint16_t _paint_page_no = paint_page_no + page_status_addr_begin;
         _paint_page_no < paint_page_no_max + paint_page_no;
         _paint_page_no += sizeof(uint8_t)) {
        i2c_result_e err = i2c_write_pages(EEPROM_ADDR, _paint_page_no * page_size,
            I2C_REGADDR_2B, (uint8_t[]){0}, sizeof(uint8_t));
        
        #ifdef DEBUG_VERBOSE
        printf("Erase paint result: %d\n", err);
        #endif

        Delay_Ms(3);
    }
}

//////////////////////////////////////////////////
//**********************************************//
//**************  LED Setting   ****************//
//**********************************************//
//////////////////////////////////////////////////

void flushCanvas(void) {
    for (int i = 0; i < NUM_LEDS; i++) {
        if(appChosen == rv_code && (i>=4 && i<8)){
            set_color(i, canvas[i].color, normal_brightness_divisor);
        }
        else{
            set_color(i, canvas[i].color, brightness_divisor);
        }

    }
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
}

void displayColorPalette(void) {
    for (int i = 0; i < NUM_LEDS; i++) {
        set_color(i, colors[i], brightness_divisor);
    }
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
    //printf("Color palette displayed\n");
}

void bucketFill(void){
    displayColorPalette();
    int8_t button = no_button_pressed;
    while (1){
        button = matrix_pressed_two();
        // Check if any button is pressed
        if(button != no_button_pressed){
            for (int i = 0; i < NUM_LEDS; i++) {
                canvas[i].layer = PAGEGROUND_LAYER;
                canvas[i].color = colors[button];
                set_color(i, colors[button], brightness_divisor);
            }
            WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
            break;
        }
        else{
            if(JOY_9_pressed()){
                #ifdef DEBUG_VERBOSE
                printf("Exit Saving\n");
                #endif

                break;
            }
        }
        Delay_Ms(200);
    }
    //flushCanvas();
}

void choose_led_brightness(void){
    led_display_brightness_status();
    int8_t button = no_button_pressed;
    while (1){
        button = matrix_pressed_two();
        if(button != no_button_pressed){
            //printf("Selected button %d\n", button);
            if(button<8){
                if(button<=2){
                    brightness_divisor = (button*button)+2;
                }
                else{
                    brightness_divisor = (button*button)+1;
                }
                //printf("Brightness: %d\n", brightness_divisor);
                break;
            }
        }
        else{
            if(JOY_9_pressed()){
                #ifdef DEBUG_VERBOSE
                printf("Exit Saving\n");
                #endif
                break;
            }
        }
        Delay_Ms(200);
    }
    flushCanvas();

}

void led_display_brightness_status(void) {
    clear();
    for (int i = 0; i < 8; i++) {
        if(i<=2){
            set_color(i, colors[NUM_LEDS], ((i*i)+2));
        }
        else{
            set_color(i, colors[NUM_LEDS], ((i*i)+1));
        }

    }

    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
}


void colorPaletteSelection(color_t * selectedColor) {
    displayColorPalette();
    while (1) {
        int8_t button = matrix_pressed_two();
        if (button != no_button_pressed) {
            *selectedColor = colors[button];
            break;
        }
        Delay_Ms(200);
    }
    #ifdef DEBUG_VERBOSE
    printf("Selected color: R:%d G:%d B:%d\n", selectedColor->r, selectedColor->g,
        selectedColor->b);
    #endif
    flushCanvas();
}

/** 
 * @brief Update pen status with selected color from variable bits
 * @param bits Variable bits from a specific line run
 * @param rVariable Current red color value
 * @param gVariable Current green color value
 * @param bVariable Current blue value
 **/
static void updatePendownColorFromBits(uint8_t bits, uint8_t rVariable, uint8_t gVariable, uint8_t bVariable) {
    rvPendownColor.r = (bits & 0x04) ? 36 * rVariable : 0;
    rvPendownColor.g = (bits & 0x02) ? 36 * gVariable : 0;
    rvPendownColor.b = (bits & 0x01) ? 36 * bVariable : 0;
}

void red_screen(void) {
    fill_color((color_t){.r = 100, .g = 0, .b = 0});
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
}

void blue_screen(void) {
    fill_color((color_t){.r = 0, .g = 0, .b = 100});
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
}
