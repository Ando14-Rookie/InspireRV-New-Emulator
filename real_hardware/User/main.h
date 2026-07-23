#define CH32V003_I2C_IMPLEMENTATION
#define WS2812BSIMPLE_IMPLEMENTATION
#include <stdbool.h>
#include "funconfig.h"
#include "./ch32v003fun/ch32v003_i2c.h"
#include "./data/colors.h"
#include "./ch32v003fun/driver.h"
//#include "./data/fonts.h"
#include "./data/music.h"
#include "./ch32v003fun/ws2812b_simple.h"
// #ifdef abs
// #undef abs
// #endif
#include "./ch32v003fun/ch32v003fun.h"
#include "./hardware_binary_game/binary_game.h"

//Storage defines
#define EEPROM_ADDR 0x53 // obtained from i2c_scan(), before shifting by 1 bit
#define page_size 64    // range of byte that stores status of page[x]
#define opcode_size 28    // range of byte that stores opcodes
#define NUM_LEDS 64
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

// Ensure HSI value has been defined
#ifndef HSI_VALUE
#define HSI_VALUE 24000000
#endif

/// @brief Initialize file storage structure for 32kb/512pages. First 8 pages are used for status.
void init_storage(void);

/// @brief Save paint data to eeprom, paint 0 stored in page ?? (out of page 0 to 511)
void save_paint(uint16_t paint_no, color_t * data, uint8_t is_icon);    
void load_paint(uint16_t paint_no, color_t * data, uint8_t is_icon);    // load paint data from eeprom, paint 0 stored in page ?? (out of page 0 to 511)
/** 
 * 
 * 
 **/
void set_page_status(uint16_t page_no, uint8_t status); 
/// @brief Reset to default storage status
void reset_storage(void);   
/// @brief Reads back and prints the current EEPROM status to console
void print_status_storage(void);    

uint8_t is_page_used(uint16_t page_no); // check if page[x] is already used

/** 
 * @brief Checks whether a block of data stored in EEPROM matches an expected “initialization signature.”
 * It check if already initialized data, i.e., init_status_data is set.
 * @return If every bit matches, it returns `1`; Otherwise, it is 0
 **/
uint8_t is_storage_initialized(void);   
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
    rv_paint = 9,
    binary_game = 10,
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
static void updatePendownColorFromBits(uint8_t bits, uint8_t rVariable, uint8_t gVariable, uint8_t bVariable);
static const uint32_t timeout_flash = 200;
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
// Put it in the header file
// void flushCanvas(void);
// void displayColorPalette(void);
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

static const color_t color_savefile_exist = {.r = 0, .g = 0, .b = 100};
static const color_t color_savefile_empty = {.r = 0, .g = 100, .b = 0};
static const color_t opcodeColor = {100, 0, 0};
static const color_t valueColor = {0, 0, 100};
color_t foreground = {100, 0, 0};
color_t background = {0, 0, 100};
// color_t pointground = {100, 100, 100};
static const color_t clearground = {0, 0, 0};
static const color_t pageground = {0, 10, 0};
color_t rvPointerColor = {.r = 150, .g = 150, .b = 150};
color_t rvPendownColor = {.r = 255, .g = 0, .b = 0};
static const color_t rvClearColor = {.r = 0, .g = 0, .b = 0};

uint8_t brightness_divisor = 10;// >0
uint8_t normal_brightness_divisor = 10;// >0
#define LED_PINS GPIOA, 2

