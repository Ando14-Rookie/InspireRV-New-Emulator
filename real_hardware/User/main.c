#define CH32V003_I2C_IMPLEMENTATION
#define WS2812BSIMPLE_IMPLEMENTATION
#include <stdbool.h>
#include <stdio.h>
#include "./ch32v003fun/ch32v003_i2c.h"
#include "./data/colors.h"
#include "./ch32v003fun/driver.h"
//#include "./data/fonts.h"
#include "./data/music.h"
#include "./ch32v003fun/ws2812b_simple.h"

//Storage defines
#define EEPROM_ADDR 0x53 // obtained from i2c_scan(), before shifting by 1 bit
#define page_size 64    // range of byte that stores status of page[x]
#define opcode_size 28    // range of byte that stores opcodes
#define init_status_addr_begin 0
#define init_status_addr_end 7
#define init_status_reg_size (init_status_addr_end - init_status_addr_begin + 1) // size  = 8
#define init_status_format "  %c "
#define init_status_data (uint8_t *)"IL000001"
#define page_status_addr_begin 8 // page 8
#define page_status_addr_end 511 // page 511
#define page_status_reg_size (page_status_addr_end - page_status_addr_begin + 1) // page size = 504
#define paint_addr_begin 8 //paint page start at 8
#define sizeof_paint_data (3 * NUM_LEDS) //paint page size = 192
#define sizeof_paint_data_aspage (sizeof_paint_data / page_size) // no. of paint page = 3
#define paint_addr_end (paint_addr_begin + 8 * sizeof_paint_data_aspage - 1) // paint page end at addr = 31
#define paint_page_no (0 * sizeof_paint_data_aspage) //no = 0
#define paint_page_no_max (8 * sizeof_paint_data_aspage) //size = 24
#define num_paint_saves (paint_page_no_max / sizeof_paint_data_aspage) //size = 8
#define opcode_addr_begin (paint_addr_end + paint_page_no_max - 1) //addr = 54
#define sizeof_opcode_data 64 //size = 64
#define sizeof_opcode_data_aspage (sizeof_opcode_data / page_size) // size = 1
#define opcode_addr_end (opcode_addr_begin + 8 * sizeof_paint_data_aspage - 1) //addr = 61
#define opcode_page_no (0 * sizeof_opcode_data_aspage) //no = 8
#define opcode_page_no_max (8 * sizeof_opcode_data_aspage) //size = 8
#define matrix_hori 16
#define app_icon_page_no (0 * sizeof_paint_data_aspage) //no = 0
#define app_icon_page_no_max (8 * sizeof_paint_data_aspage) //size = 24


#define delay 1000



// initialize file storage structure for 32kb/512pages
// first 8 pages are used for status
void init_storage(void);
void save_paint(uint16_t paint_no, color_t * data, uint8_t is_icon);    // save paint data to eeprom, paint 0 stored in page ?? (out of page 0 to 511)
void load_paint(uint16_t paint_no, color_t * data, uint8_t is_icon);    // load paint data from eeprom, paint 0 stored in page ?? (out of page 0 to 511)
void set_page_status(uint16_t page_no, uint8_t status); // set page status to 0 or 1
void reset_storage(void);   // reset to default storage status
void print_status_storage(void);    // print storage data to console
uint8_t is_page_used(uint16_t page_no); // check if page[x] is already used
uint8_t is_storage_initialized(void);   // check if already initialized data, aka init_status_data is set
// save opcode data to eeprom, paint 0 stored in page ?? (out of page 0 to 511)
void save_opCode(uint16_t opcode_no, uint8_t * data);
void load_opCode(uint16_t opcode_no, uint8_t * data);


uint16_t calculate_page_no(uint16_t paint_no, uint8_t is_icon);
void any_paint_exist(uint8_t * paint_exist);
void any_opcode_exist(uint8_t * opcode_exist);
void erase_all_paint_saves(void);

//App selection
void appRunningRoutine(void);
/** @brief Numbers are arranged by the order of icons
 * in the EEPROM!!!!! Read app_selection() for more info.
 */
typedef enum _app_selected {
    paint = 0,
    music = 1,       // not implemented
    rec = 2,         // not implemented
    risc_v_code = 3, // not implemented
    game_tic_tac_toe = 4,
    game_snake = 5,
    robot_car = 6,
    rv_code = 7,
	rv_music = 8,
    rv_paint = 9
} app_selected;
app_selected appChosen = rv_paint;

//RV Paints defines
void painting_routine(void);
void iconShow(void);
//void display_stored_paints(void);


void choose_save_page(app_selected app_current);
void choose_load_page(app_selected app_current);
void led_display_paint_page_status(app_selected app_current);

// RV Code defines
/******************************************/
/*InspireRV Commands
Coding Mode
Function: 5bits  value: 3bits

Drawing 0b00
00 000 xxx | Fill Screen - xxx:RGB ~ Clear(000)
00 001 00x | Hide/Show(x:0/x:1) Cursor
00 010 xxx | Draw(Pen down) with color - xxx:RGB, xxx:000(Pen Up)
00 011 xxx | Not yet
00 100 xxx | Sound Freq
00 101 xxx | Sound dur
00 110 xxx | Set Program simulation speed - xxx:(0~7) ~ default 4
00 111 xxx | Move Cursor to 0:home position, 1~4: four corners, 7: variable position

Moving 0b01
01 000 xxx | move north xxx:(0~7) steps
01 001 xxx | move ne    xxx:(0~7) steps
01 010 xxx | move east  xxx:(0~7) steps
01 011 xxx | move se    xxx:(0~7) steps
01 100 xxx | move south xxx:(0~7) steps
01 101 xxx | move sw    xxx:(0~7) steps
01 110 xxx | move west  xxx:(0~7) steps
01 111 xxx | move nw    xxx:(0~7) steps

Options 0b10
10 000 000 | End of Program, codes after will not be run
10 001 xxx | Jump to line xxx:(0~7) at Page 1
10 010 xxx | Jump to line xxx:(0~7) at Page 2
10 011 xxx | Jump to line xxx:(0~7) at Page 3
10 100 xxx | Jump to line xxx:(0~7) at Page 4
10 101 xxx | Load saved Program (0~7)
10 110 xxx | Load saved Music (0~7)
10 111 xxx | Load saved Drawing (0~7)


loop variables 0b11
11 000 xxx | skipifCarry(minus), decrease with xxx:(0~7) until < 0
11 001 xxx | set blue color variable level xxx:(0~7) - dark 0, bright 7
11 010 xxx | set greencolor variable level xxx:(0~7) - dark 0, bright 7
11 011 xxx | set y variable coord xxx:(0~7) - xy-coord for 8x8
11 100 xxx | set red color variable level xxx:(0~7) - dark 0, bright 7
11 101 xxx | set x variable coord xxx:(0~7) - xy-coord for 8x8
11 110 xxx | set looping variable xxx:(0~7), only for looping
11 111 xxx | skipifCarry(plus), increase with xxx:(0~7) until > 7

1: Load
3: save
4: Programming space
5: Result
6: Run step by step
9: Register a color in a color Panel

8-line: Page0, Page1, Page2, Page3, Current Pen Color
green: head
blue: tail
//////////////////////////////////////////*/
/******************************************/

#define _OPCODE_PEN                 0b00
#define _RVCODE_OPCODE_CLRSCREEN    0b00000
#define _RVCODE_OPCODE_TURT         0b00001
#define _RVCODE_OPCODE_PENRGB       0b00010
#define _RVCODE_OPCODE_SOUNDFREQ    0b00100
#define _RVCODE_OPCODE_SOUNDDUR     0b00101
#define _RVCODE_OPCODE_PROSPEED     0b00110
#define _RVCODE_OPCODE_TURT_POS     0b00111
#define _OPCODE_MOVE                0b01
#define _RVCODE_OPCODE_FD0          0b01000
#define _RVCODE_OPCODE_FD45         0b01001
#define _RVCODE_OPCODE_FD90         0b01010
#define _RVCODE_OPCODE_FD135        0b01011
#define _RVCODE_OPCODE_FD180        0b01100
#define _RVCODE_OPCODE_FD225        0b01101
#define _RVCODE_OPCODE_FD270        0b01110
#define _RVCODE_OPCODE_FD315        0b01111
#define _OPCODE_OPTION              0b10
#define _RVCODE_OPCODE_JUMPP1       0b10001
#define _RVCODE_OPCODE_JUMPP2       0b10010
#define _RVCODE_OPCODE_JUMPP3       0b10011
#define _RVCODE_OPCODE_JUMPP4       0b10100
#define _RVCODE_OPCODE_LOADCODE     0b10101
#define _RVCODE_OPCODE_LOADMUSIC    0b10110
#define _RVCODE_OPCODE_LOADPAINT    0b10111
#define _RVCODE_OPCODE_END          0b10000
#define _OPCODE_VARLOOP             0b11
#define _RVCODE_OPCODE_MINUSSKIP    0b11000
#define _RVCODE_OPCODE_LOOPVAR      0b11101
#define _RVCODE_OPCODE_ADDSKIP      0b11111
#define _RVCODE_OPCODE_RVAR         0b11100
#define _RVCODE_OPCODE_GVAR         0b11010
#define _RVCODE_OPCODE_BVAR         0b11001
#define _RVCODE_OPCODE_XVAR         0b11110
#define _RVCODE_OPCODE_YVAR         0b11011

#define _DIR_FD0                    8
#define _DIR_FD45                   7
#define _DIR_FD90                   -1
#define _DIR_FD135                  -9
#define _DIR_FD180                  -8
#define _DIR_FD225                  -7
#define _DIR_FD270                  1
#define _DIR_FD315                  9
#define _DIR_STOP                   0
#define _TOTAL_CODE_LINE            28

void rv_code_routine(void);
void rvCodeRun(uint8_t direct_result);
uint8_t opGroupExtraction(uint8_t received_message[8]);
uint8_t opCodeExtraction(uint8_t received_message[8]);
uint8_t varExtraction(uint8_t received_message[8]);
void toCodingSpace(uint8_t curr_page);
const uint32_t timeout_flash = 200;
uint32_t timeout_var_code = 150;
uint32_t timeout_line_code = 300;
uint8_t funcRun[8] = {0};
uint8_t numRun[8] = {0};
uint8_t programStored[64] = {0};
uint8_t opCodeStorage[4][7][8] = {0};
uint8_t opCodeToStored[28] ={0};
uint8_t currentPage = 1;
typedef struct rvCodeParts {
    char part;
    color_t current_color;
} rvCodeParts;
rvCodeParts rv_coding_board[64]={'0'}; // 8x8 gameboard
int8_t pointerLocation = 36;






// Color defines
void flushCanvas(void);
void displayColorPalette(void);
void colorPaletteSelection(color_t * selectedColor);
void logoDisplay(void);
void red_screen(void);
void bucketFill(void);
void choose_led_brightness(void);
void led_display_brightness_status(void);

typedef struct {
    enum { FOREGROUND_LAYER, BACKGROUND_LAYER, CLEARROUND_LAYER, PAGEGROUND_LAYER } layer;
    color_t color;
} canvas_t;
canvas_t canvas[NUM_LEDS] = {0};

const color_t color_savefile_exist = {.r = 0, .g = 0, .b = 100};
const color_t color_savefile_empty = {.r = 0, .g = 100, .b = 0};
const color_t opcodeColor = {100, 0, 0};
const color_t valueColor = {0, 0, 100};
color_t foreground = {100, 0, 0};
color_t background = {0, 0, 100};
color_t pointground = {100, 100, 100};
color_t clearground = {0, 0, 0};
color_t pageground = {0, 10, 0};
color_t rvPointerColor = {.r = 150, .g = 150, .b = 150};
color_t rvPendownColor = {.r = 255, .g = 0, .b = 0};
color_t rvClearColor = {.r = 0, .g = 0, .b = 0};

uint8_t brightness_divisor = 10;// >0
uint8_t normal_brightness_divisor = 10;// >0
#define LED_PINS GPIOA, 2



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
            printf("Paint reset\n");
            printf("DEBUG: %d\n", __LINE__);
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
            case rv_code:
                rv_code_routine();
                break;
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
                        switch(var_line_storage[line_run]){
                            case 0:
                                sound_freq = NOTE_C4;
                                break;
                            case 1:
                                sound_freq = NOTE_D4;
                                break;
                            case 2:
                                sound_freq = NOTE_E4;
                                break;
                            case 3:
                                sound_freq = NOTE_F4;
                                break;
                            case 4:
                                sound_freq = NOTE_G4;
                                break;
                            case 5:
                                sound_freq = NOTE_A4;
                                break;
                            case 6:
                                sound_freq = NOTE_B4;
                                break;
                            case 7:
                                sound_freq = NOTE_C5;
                                break;
                            default:
                                sound_freq = NOTE_C4;
                                break;
                        }
                        JOY_sound(sound_freq,sound_dur);
                        break;
                    case _RVCODE_OPCODE_PENRGB:
                        if(var_line_storage[line_run]== 0){
                            penStatus = 0;
                        }
                        else{
                            penStatus = 1;
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
                            //printf("Leave Color R: %d, G: %d, B:%d\n",rvPendownColor.r, rvPendownColor.g, rvPendownColor.b);
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

uint8_t opGroupExtraction(uint8_t received_message[8]){
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
    return opcodeGroup;
}

uint8_t opCodeExtraction(uint8_t received_message[8]){
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
}

uint8_t varExtraction(uint8_t received_message[8]){
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
                printf("Enter paint loading screen!\n");
                choose_load_page(rv_paint);
                Delay_Ms(1000);
                printf("Exit paint loading screen!\n");
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
    uint8_t data[init_status_reg_size];
    i2c_read(EEPROM_ADDR, init_status_addr_begin, I2C_REGADDR_2B, data, init_status_reg_size);
    for (uint8_t i = 0; i < init_status_reg_size; i++) {
        if (data[i] != *(init_status_data + i)) {
            return 0;
        }
    }
    return 1;
}

void reset_storage(void) {
    i2c_write(EEPROM_ADDR, init_status_addr_begin, I2C_REGADDR_2B, init_status_data,
        init_status_reg_size);
    Delay_Ms(3);
    for (uint16_t addr = page_status_addr_begin;
         addr < page_status_addr_begin + page_status_reg_size; addr++) {
        i2c_write(EEPROM_ADDR, addr, I2C_REGADDR_2B, (uint8_t[]){0}, sizeof(uint8_t));
        Delay_Ms(3);
    }
    printf("Storage reset\n");
}

void print_status_storage(void) {
    printf("Status storage data:\n");
    for (uint16_t addr = init_status_addr_begin;
         addr < init_status_addr_begin + init_status_reg_size; addr++) {
        uint8_t data = 0;
        i2c_read(EEPROM_ADDR, addr, I2C_REGADDR_2B, &data, sizeof(data));
        printf(" %d: ", addr);
        printf(init_status_format, data);
    }
    printf("\n");
    for (uint16_t addr = page_status_addr_begin;
         addr < page_status_addr_begin + page_status_reg_size; addr++) {
        uint8_t data = 0;
        i2c_read(EEPROM_ADDR, addr, I2C_REGADDR_2B, &data, sizeof(data));
        if (data) {
            printf("%d ", addr);
        }
        else {
            printf("    ");
        }
        if ((addr + 1) % matrix_hori == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

void set_page_status(uint16_t page_no, uint8_t status) {
    if (status > 1) {
        printf("Invalid status %d\n", status);
        printf("DEBUG: %d\n", __LINE__);
        while (1)
            ;
    }
    if (page_no < page_status_addr_begin || page_no > page_status_addr_end) {
        printf("Invalid page number %d\n", page_no);
        printf("DEBUG: %d\n", __LINE__);
        while (1)
            ;
    }
    i2c_write(EEPROM_ADDR, page_no, I2C_REGADDR_2B, &status, sizeof(status));
    Delay_Ms(3);
    //printf("Page %d status set to %d\n", page_no, status);
}

uint8_t is_page_used(uint16_t page_no) {
    if (page_no < page_status_addr_begin || page_no > page_status_addr_end) {
        printf("Invalid page number %d\n", page_no);
        printf("DEBUG: %d\n", __LINE__);
        while (1);
    }
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

void save_paint(uint16_t paint_no, color_t * data, uint8_t is_icon) {
    if (paint_no < 0 || paint_no > paint_addr_end) {
        printf("Invalid paint number %d\n", paint_no);
        printf("DEBUG: %d\n", __LINE__);
        while (1)
            ;
    }
    uint16_t page_no_start = calculate_page_no(paint_no, is_icon);
    for (uint16_t i = page_no_start; i < page_no_start + sizeof_paint_data_aspage; i++) {
        if (is_page_used(i)) {
            printf("Paint %d already used, overwriting\n", paint_no);
            Delay_Ms(500);
        }
        set_page_status(i, 1);
    }
    i2c_result_e err = i2c_write_pages(EEPROM_ADDR, page_no_start * page_size,
        I2C_REGADDR_2B, (uint8_t *)data, sizeof_paint_data);
    printf("Save paint result: %d\n", err);
    Delay_Ms(3);
    printf("Paint %d saved\n", paint_no);
}


void save_opCode(uint16_t opcode_no, uint8_t * data) {
    if (opcode_no < 0 || opcode_no > page_status_addr_end) {
        printf("Invalid paint number %d\n", opcode_no);
        printf("DEBUG: %d\n", __LINE__);
        while (1);
    }
    uint16_t page_no_start = calculate_page_no(opcode_no, 0);
    for (uint16_t i = page_no_start; i < page_no_start + sizeof_opcode_data_aspage; i++) {
        if (is_page_used(i)) {
            printf("Opcode %d already used, overwriting\n", opcode_no);
            Delay_Ms(500);
        }
        set_page_status(i, 1);
    }
    i2c_result_e err = i2c_write_pages(EEPROM_ADDR, page_no_start * page_size,
        I2C_REGADDR_2B, (uint8_t *)data, sizeof_opcode_data);
    printf("Save Opcode result: %d\n", err);
    Delay_Ms(3);
    printf("Opcode %d saved\n", opcode_no);
}

void load_paint(uint16_t paint_no, color_t * data, uint8_t is_icon) {
    if (paint_no < 0 || paint_no > paint_addr_end) {
        printf("Invalid paint number %d\n", paint_no);
        printf("DEBUG: %d\n", __LINE__);
        while (1)
            ;
    }
    uint16_t page_no_start = calculate_page_no(paint_no, is_icon);
    printf("Loading paint_no %d from page %d, is_icon: %d\n", paint_no, page_no_start,
        is_icon);
    if (!is_page_used(page_no_start)) {
        printf("Paint %d not found\n", paint_no);
        printf("DEBUG: %d\n", __LINE__);
        while (1)
            ;
    }
    i2c_result_e err = i2c_read_pages(EEPROM_ADDR, page_no_start * page_size,
        I2C_REGADDR_2B, (uint8_t *)data, sizeof_paint_data);
    printf("Load paint result: %d\n", err);
    Delay_Ms(3);
    printf("Paint %d loaded\n", paint_no);
}

void load_opCode(uint16_t opcode_no, uint8_t * data) {
    if (opcode_no < 0 || opcode_no > page_status_addr_end) {
        printf("Invalid paint number %d\n", opcode_no);
        printf("DEBUG: %d\n", __LINE__);
        while (1)
            ;
    }
    uint16_t page_no_start = calculate_page_no(opcode_no, 0);
    printf("Loading paint_no %d from page %d, is_icon: %d\n", opcode_no, page_no_start,0);
    if (!is_page_used(page_no_start)) {
        printf("Paint %d not found\n", opcode_no);
        printf("DEBUG: %d\n", __LINE__);
        while (1)
            ;
    }
    i2c_result_e err = i2c_read_pages(EEPROM_ADDR, page_no_start * page_size,
        I2C_REGADDR_2B, (uint8_t *)data, sizeof_opcode_data);
    printf("Load paint result: %d\n", err);
    Delay_Ms(3);
    printf("Paint %d loaded\n", opcode_no);
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
            printf("Selected page %d\n", button);

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

            printf("Paint load\n");
            Delay_Ms(1000);
            break;
        }
        else{
            if (JOY_9_pressed()){
                printf("Exit Loading\n");
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

            printf("Paint saved\n");
            Delay_Ms(1000);
            break;
        }
        else{
            if (JOY_9_pressed()){
                printf("Exit Saving\n");
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
    printf("All paint saves status erased\n");
    // Erase existing data to 0
    for (uint16_t _paint_page_no = paint_page_no + page_status_addr_begin;
         _paint_page_no < paint_page_no_max + paint_page_no;
         _paint_page_no += sizeof(uint8_t)) {
        i2c_result_e err = i2c_write_pages(EEPROM_ADDR, _paint_page_no * page_size,
            I2C_REGADDR_2B, (uint8_t[]){0}, sizeof(uint8_t));
        printf("Erase paint result: %d\n", err);
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
                printf("Exit Saving\n");
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
                printf("Exit Saving\n");
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
    printf("Selected color: R:%d G:%d B:%d\n", selectedColor->r, selectedColor->g,
        selectedColor->b);
    flushCanvas();
}


void red_screen(void) {
    fill_color((color_t){.r = 100, .g = 0, .b = 0});
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
}

void blue_screen(void) {
    fill_color((color_t){.r = 0, .g = 0, .b = 100});
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
}
