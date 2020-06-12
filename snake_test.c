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

unsigned short *fb;
int scale = 5;
int speed = 100;
int standard = 0;
int demo = 1;
int retry = 1;
int quit = 0;

int main() {
    unsigned char *mem_base;
    unsigned char *parlcd_mem_base;
    uint32_t val_line=5;
    uint32_t rgb_knobs_value;
    int i;
    int start, now, sec, ns;
    char ch = '1';

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

    // rgb_knobs_value = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    // rgb_knobs_value = 16711935; //pink

    // *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = rgb_knobs_value;
    // *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o) = rgb_knobs_value;

    snake_t snake;
    desk_t desk;
    food_t food;

    desk = initDesk(16, 14, 1, 1);
    snake = initSnake(16, 14, 5, 5);
    food = initFood(10, 10);

    draw_Menu1(fb, standard, demo);
    goto start;

    Menu:
    draw_Menu(fb, standard, demo);
    start:
    ch = '1';
    while (ch!=' ')
    {
        int r = read(0, &ch, 1);
        if (r==1)
        {   
            if (ch == 'w') {
                standard = 1 - standard;
                demo = 1 - demo;
                draw_Menu(fb, standard, demo);
            }
            else if (ch == 's') {
                standard = 1 - standard;
                demo = 1 - demo;
                draw_Menu(fb, standard, demo);
            }
            else if (ch == ' ') {
                break;
            }
        }
    }
        
    snake = initSnake(16, 14, 5, 5);

    start_game:
    start = clock();
    while (snake.life) {
        now = clock();
        ns = (now-start) / 1000;
        sec = ns / 1000;
        if (ns % speed == 0){
            printf("%d\n", ns);
            drawDesk(&desk, &snake, &food, sec, fb);
            if (standard==1){
                moveSnakeManual(&snake, &food, &desk);
            }
            else
            {
                moveSnakeAI(&snake, &food, &desk);
            }
        }
    }
    sleep(1);
    draw_EndGame(fb, snake.score, retry, quit);
    while (ch!=' ')
    {
        int r = read(0, &ch, 1);
        if (r==1)
        {   
            if (ch == 'w') {
                retry = 1 - retry;
                quit = 1 - quit;
                draw_EndGame(fb, snake.score, retry, quit);
            }
            else if (ch == 's') {
                retry = 1 - retry;
                quit = 1 - quit;
                draw_EndGame(fb, snake.score, retry, quit);
            }
            else if (ch == ' ') {
                break;
            }
        }
    }

    if (retry==1){
        snake = initSnake(16, 14, 5, 5);
        goto start_game;
    }
    else {
        goto Menu;
    }
    

    rgb_knobs_value = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    rgb_knobs_value =16711935; //purple

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
    
    // parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
    // if (parlcd_mem_base == NULL)
    //     exit(1);
    // parlcd_hx8357_init(parlcd_mem_base);

    // parlcd_write_cmd(parlcd_mem_base, 0x2c);
    // for (ptr = 0; ptr < 480*320 ; ptr++) {
    //     parlcd_write_data(parlcd_mem_base, fb[ptr]);
    // }

    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
    return 0;

}
