#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <termios.h>

#include "snake.h"

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "font_types.h"
#include "font_prop14x16.c"

unsigned short *fb;
// font_descriptor_t* fdes = &font_winFreeSystem14x16;

int scale = 5;

int main() {

    unsigned char *mem_base;
    unsigned char *parlcd_mem_base;
    uint32_t val_line=5;
    uint32_t rgb_knobs_value;
    int i;
    int ptr;

    static struct termios oldt, newt;
    tcgetattr( STDIN_FILENO, &oldt); 
    newt = oldt; 
    newt.c_lflag &= ~(ICANON); 
    newt.c_cc[VMIN] = 0;
    newt.c_cc[VTIME] = 0;
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);

    fb  = (unsigned short *)malloc(320*480*2);
    mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
    /* If mapping fails exit with error code */
    if (mem_base == NULL)
        exit(1);

    rgb_knobs_value = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    rgb_knobs_value = 16711935; //pink

    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = rgb_knobs_value;
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o) = rgb_knobs_value;

    snake_t snake;
    desk_t desk;
    food_t food;

    desk = initDesk(16, 14, 1, 1);
    snake = initSnake(16, 14, 5, 5);
    food = initFood(10, 10);

    int start, now, sec, ns;
    start = clock();

    // struct timespec loop_delay = {.tv_sec = 0, .tv_nsec = 100 * 1000 * 1000};
    // clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    while (snake.life) {
        now = clock();
        ns = (now-start) / 1000;
        sec = ns / 1000;
        if (ns % 100 == 0){
            printf("%d\n", ns);
            drawDesk(&desk, &snake, &food, sec, fb);
            moveSnakeAI(&snake, &food, &desk);
            // moveSnakeManual(&snake, &food, &desk);

        }
    }

    rgb_knobs_value = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    rgb_knobs_value =16711680; //red

    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = rgb_knobs_value;
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o) = rgb_knobs_value;

    // LED Line
    val_line = 15;
    // val_line = 1227133513;
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = val_line;
    struct timespec loop_delay = {.tv_sec = 0, .tv_nsec = 50 * 1000 * 1000};
    for (i=0; i<30; i++) {
        *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = val_line;
        val_line<<=1;
        //printf("LED val 0x%x\n", val_line);
        clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    }
    
    parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
    if (parlcd_mem_base == NULL)
        exit(1);
    parlcd_hx8357_init(parlcd_mem_base);
    

    // draw Menu
    for (ptr = 0; ptr < 320*480 ; ptr++) {
        fb[ptr]=0u;
    }
    char str[] = "Game over"; // 9
    char *ch = str;
    int x = 20;
    for (int i=0; i<9; i++) {
        draw_char(x, 100, fdes, *ch, scale);
        x+=scale*char_width(fdes, *ch)+2;
        ch++;
    }

    parlcd_write_cmd(parlcd_mem_base, 0x2c);
        for (ptr = 0; ptr < 480*320 ; ptr++) {
            parlcd_write_data(parlcd_mem_base, fb[ptr]);
        }

    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
    return 0;

}
