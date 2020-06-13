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
int slow = 1;
int medium = 0;
int fast = 0;
int a, b, c;

int main() {
    unsigned char *mem_base;
    unsigned char *parlcd_mem_base;
    uint32_t val_line=5;
    uint32_t rgb_knobs_value;
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

    snake_t *snake;
    desk_t *desk;
    food_t *food;

    desk = initDesk(16, 14, 1, 1);
    snake = initSnakeAI(16, 14, 5, 5);
    // snake2 = initSnake(16, 14, 10, 10);
    food = initFood(10, 10);

    goto Menu;
        
    start_game:
    snake = initSnakeAI(16, 14, 5, 5);
    draw_speed_ctrl(fb, slow, medium, fast);
    ch = '1';
    while (ch!=' ')
    {
        int r = read(0, &ch, 1);
        if (r==1)
        {   
            a = slow;
            b = medium;
            c = fast;
            if (ch == 's') {
                slow = c;
                medium = a;
                fast = b;
                draw_speed_ctrl(fb, slow, medium, fast);
            }
            else if (ch == 'w') {
                slow = b;
                medium = c;
                fast = a;
                draw_speed_ctrl(fb, slow, medium, fast);
            }
            else if (ch == ' ') {
                break;
            }
        }
    }
    speed = 100*fast + 250*medium + 500*slow;
    start = clock();
    while (desk->gameOver) {
        now = clock();
        ns = (now-start) / 1000;
        sec = ns / 1000;
        if (ns % speed == 0){
            printf("%d\n", ns);
            drawDesk(desk, snake, food, sec, fb);
            if (standard==1){
                moveSnakeManual(snake, food, desk);
            }
            else
            {
                moveSnakeAI(snake, food, desk);
            }
        }
    }
    sleep(1);
    draw_EndGame(fb, snake->score, retry, quit);
    ch = '1';
    while (ch!=' ')
    {
        int r = read(0, &ch, 1);
        if (r==1)
        {   
            if (ch == 'w') {
                retry = 1 - retry;
                quit = 1 - quit;
                draw_EndGame(fb, snake->score, retry, quit);
            }
            else if (ch == 's') {
                retry = 1 - retry;
                quit = 1 - quit;
                draw_EndGame(fb, snake->score, retry, quit);
            }
            else if (ch == ' ') {
                break;
            }
        }
    }

    if (retry==1){
        snake = initSnakeAI(16, 14, 5, 5);
        goto start_game;
    }
    Menu:
    draw_Menu(fb, standard, demo);
    //draw LED
    rgb_knobs_value = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    rgb_knobs_value = 16711935; //purple

    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = rgb_knobs_value;
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o) = rgb_knobs_value;
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
    goto start_game;

    rgb_knobs_value = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    rgb_knobs_value =16711935; //purple

    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = rgb_knobs_value;
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o) = rgb_knobs_value;

    // LED Line
    // val_line = 15;
    // val_line = 1227133513;
    // *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = val_line;
    // struct timespec loop_delay = {.tv_sec = 0, .tv_nsec = 50 * 1000 * 1000};
    // for (i=0; i<30; i++) {
    //     *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = val_line;
    //     val_line<<=1;
    //     //printf("LED val 0x%x\n", val_line);
    //     clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    // }
    
    // parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
    // if (parlcd_mem_base == NULL)
    //     exit(1);
    // parlcd_hx8357_init(parlcd_mem_base);

    // parlcd_write_cmd(parlcd_mem_base, 0x2c);
    // for (ptr = 0; ptr < 480*320 ; ptr++) {
    //     parlcd_write_data(parlcd_mem_base, fb[ptr]);
    // }

    //clean up after game is over
    freeDesk(desk);
    freeSnake(snake);
    freeFood(food);

    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
    return 0;
}
