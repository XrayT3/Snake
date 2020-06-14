#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <termios.h>

#include "snake.h"
#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"

unsigned short *fb;
int speed = 100;
int standard = 0;
int demo = 1;
int Exit = 0;
int retry = 1;
int quit = 0;
int slow = 1;
int medium = 0;
int fast = 0;
int a, b, c, ptr, i;
uint32_t rgb_knobs_value;
unsigned char *mem_base;



void rgb_LED(int color){
    rgb_knobs_value = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    rgb_knobs_value = color;
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = rgb_knobs_value;
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o) = rgb_knobs_value;
}

int main() {
    unsigned char *parlcd_mem_base;
    uint32_t val_line=5;
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

    parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
    if (parlcd_mem_base == NULL)
        exit(1);
    parlcd_hx8357_init(parlcd_mem_base);

    val_line = 4;
    double p = 5;
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = val_line;
    struct timespec loop_delay = {.tv_sec = 0, .tv_nsec = 50 * 1000 * 1000};
    for (i=0; i<10; i++) {
        *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = val_line;
        val_line += pow(2, p);
        p+=3;
        clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    }

    snake_t *snake;
    snake_t *snake2;
    snake_t *snakeAI;
    snake_t *snakeAI2;
    desk_t *desk;
    food_t *food;
    desk = initDesk(16, 14, 1, 1);
    food = initFood(4, 4);
    snake = initSnake(16, 14, 5, 5, 'a', 'd');
    snake2 = initSnake(16, 14, 10, 10, 'j', 'l');
    snakeAI = initSnakeAI(16, 14, 5, 5);
    snakeAI2 = initSnakeAI(16, 14, 10, 10);

    goto Menu;
    start_game:
    food = initFood(4, 4);
    snake = initSnake(16, 14, 5, 5, 'a', 'd');
    snake2 = initSnake(16, 14, 10, 10, 'j', 'l');
    snakeAI = initSnakeAI(16, 14, 5, 5);
    snakeAI2 = initSnakeAI(16, 14, 10, 10);
    snake->gameOver = 0;
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
    // draw_set_players();
    speed = 100*fast + 250*medium + 500*slow;
    start = clock();
    rgb_LED(65280); //green
    while ((1-snake->gameOver) & (1-snake2->gameOver)) {
        now = clock();
        ns = (now-start) / 1000;
        sec = ns / 1000;
        if (ns % speed == 0){
            if (standard==1){
                moveSnakeManual(snake, food, desk);
                // moveSnakeManual(snake2, food, desk);
                drawDesk(desk, snake, food, sec, fb);
            }
            else
            {
                moveSnakeAITwoSnakes(snakeAI, snakeAI2, food, desk);
                moveSnakeAITwoSnakes(snakeAI2, snakeAI, food, desk);
                drawDesk_2_snakes(desk, snakeAI, snakeAI2, food, sec, fb);
                if (snakeAI->gameOver | snakeAI2->gameOver){
                    break;
                }
            }
        }
    }
    sleep(1);
    int record = get_record(snake->score, snake2->score, snakeAI->score, snakeAI2->score);
    draw_EndGame(fb, record, retry, quit);
    rgb_LED(16711680); //red
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
        goto start_game;
    }
    Menu:
    draw_Menu(fb, standard, demo, Exit);
    rgb_LED(16711935); //purple
    ch = '1';
    while (ch!=' ')
    {
        int r = read(0, &ch, 1);
        if (r==1)
        {   
            a = standard;
            b = demo;
            c = Exit;
            if (ch == 's') {
                standard = c;
                demo = a;
                Exit = b;
                draw_Menu(fb, standard, demo, Exit);
            }
            else if (ch == 'w') {
                standard = b;
                demo = c;
                Exit = a;
                draw_Menu(fb, standard, demo, Exit);
            }
            else if (ch == ' ') {
                break;
            }
        }
    }
    if (1-Exit){
        goto start_game;
    }

    // exit:
    rgb_LED(0); // off

    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = 1227133513;
    clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = 153391689;
    clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = 19173961;
    clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = 2396745;
    clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = 299593;
    clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = 37449;
    clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = 18724;
    clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = 2340;
    clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = 292;
    clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = 36;
    clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = 4;
    clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = 0;
    parlcd_write_cmd(parlcd_mem_base, 0x2c);
    for (ptr = 0; ptr < 480*320 ; ptr++) {
        parlcd_write_data(parlcd_mem_base, 0);
    }
    //clean up after game is over
    freeDesk(desk);
    freeSnake(snake);
    freeSnake(snake2);
    freeSnake(snakeAI);
    freeSnake(snakeAI2);
    freeFood(food);
    free(fb);

    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
    return 0;
}

