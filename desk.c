#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <time.h>
#include <stdbool.h>
#include <termios.h>

#include "snake.h"

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
// #include "font_types.h"
// #include "font_prop14x16.c"

unsigned short *fb;
// font_descriptor_t* fdes = &font_winFreeSystem14x16;
unsigned char *parlcd_mem_base;
unsigned char *mem_base;
uint32_t rgb_knobs_value;

int size_cell_d = 20;
// int size_score = 4;
// int size_time = 4;
// int size_GameOver = 5;
// int size_retry = 4;
// int size_quit = 4;
// int size_standard = 5;
// int size_demo = 5;
// int size_speed = 5;

desk_t *initDesk(int width, int height, int startX, int startY) {

    desk_t *desk = (desk_t *)malloc(sizeof(desk_t));
    desk->width = width;
    desk->height = height;
    desk->startX = startX;
    desk->startY = startY;
    desk->endX = startX + width;
    desk->endY = startY + height;
    desk->gameOver = 0;
    return desk;
}

void freeDesk(desk_t *desk) {

    free(desk);
    desk = NULL;
}

void drawDesk(desk_t *desk, snake_t *snake, food_t *food, int sec, unsigned short *fb1) {

    int ptr;
    fb = fb1;
    for (ptr = 0; ptr < 480*320 ; ptr++) {
        fb[ptr] = 0;
    }
    draw_score(snake->score);
    draw_time(sec);
    draw_wall(16, 14); // dobavit parametry

    for (int i = desk->startY + 1; i < desk->endY; i ++) {
        for (int j = desk->startX; j < desk->endX + 1; j++) {
            if (
                (j == food->coord[0]) &&
                (i == food->coord[1])
            )
            draw_food(j*size_cell_d, i*size_cell_d); // food
            else{
                for (int k = 0; k < snake->length; k++) {

                    if (
                        j == (snake->snake_skeleton[k].coords[0]) &&
                        i == (snake->snake_skeleton[k].coords[1]) 
                    )
                    draw_snake(j*size_cell_d, i*size_cell_d); // snake
                }
            }
        }
    }

    // LED
    mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
    rgb_knobs_value = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    rgb_knobs_value = 65280; // green

    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = rgb_knobs_value;
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o) = rgb_knobs_value;

    // draw LCD
    parlcd_write_cmd(parlcd_mem_base, 0x2c);
    for (ptr = 0; ptr < 480*320 ; ptr++) {
        parlcd_write_data(parlcd_mem_base, fb[ptr]);
    }
}

int obstacleBeforeSnake(snake_t *snake, desk_t *desk) {

    int ret = 0;

    if (
        snake->direction == UP &&
        snake->snake_skeleton[0].coords[1] == (desk->startY + 1)
    ) {
        ret = 1;
        return ret;
    }
    else if (
        snake->direction == DOWN &&
        snake->snake_skeleton[0].coords[1] == (desk->endY - 1)
    ) {
        ret = 1;
        return ret;
    }
    else if (
        snake->direction == LEFT &&
        snake->snake_skeleton[0].coords[0] == (desk->startX + 1)
    ) {
        ret = 1;
        return ret;
    }
    else if (
        snake->direction == RIGHT &&
        snake->snake_skeleton[0].coords[0] == (desk->endX - 1)
    ) {
        ret = 1;
        return ret;
    }
    for (int k = 0; k < snake->length; k++) {

        if (
            snake->direction == UP                                                          &&
            snake->snake_skeleton[0].coords[1] == (snake->snake_skeleton[k].coords[1] + 1)  &&
            snake->snake_skeleton[0].coords[0] == snake->snake_skeleton[k].coords[0]
        ) {
            ret = 1;
            return ret;
        }
        else if (
            snake->direction == DOWN                                                        &&
            snake->snake_skeleton[0].coords[1] == (snake->snake_skeleton[k].coords[1] - 1)  &&
            snake->snake_skeleton[0].coords[0] == snake->snake_skeleton[k].coords[0]
        ) {
            ret = 1;
            return ret;
        }
        else if (
            snake->direction == LEFT                                                        &&
            snake->snake_skeleton[0].coords[0] == (snake->snake_skeleton[k].coords[0] + 1)  &&
            snake->snake_skeleton[0].coords[1] == snake->snake_skeleton[k].coords[1]  
        ) {
            ret = 1;
            return ret;
        }
        else if (
            snake->direction == RIGHT                                                       &&
            snake->snake_skeleton[0].coords[0] == (snake->snake_skeleton[k].coords[0] - 1)  &&
            snake->snake_skeleton[0].coords[1] == snake->snake_skeleton[k].coords[1]  
        ) {
            ret = 1;
            return ret;
        }
    }
    return ret;
}

int obstacleLeftOfSnake(snake_t *snake, desk_t *desk) {

    if (
        snake->direction == UP &&
        snake->snake_skeleton[0].coords[0] == (desk->startX + 1)
    )   return 1;
    
    else if (
        snake->direction == DOWN &&
        snake->snake_skeleton[0].coords[0] == (desk->endX - 1)
    )   return 1;
    
    else if (
        snake->direction == LEFT &&
        snake->snake_skeleton[0].coords[1] == (desk->endY - 1)
    )   return 1;
    
    else if (
        snake->direction == RIGHT &&
        snake->snake_skeleton[0].coords[1] == (desk->startY + 1)
    )   return 1;
    
    return 0;
}

int obstacleRightOfSnake(snake_t *snake, desk_t *desk) {

    if (
        snake->direction == UP &&
        snake->snake_skeleton[0].coords[0] == (desk->endX - 1)
    )   return 1;
    
    else if (
        snake->direction == DOWN &&
        snake->snake_skeleton[0].coords[0] == (desk->startX + 1)
    )   return 1;
    
    else if (
        snake->direction == LEFT &&
        snake->snake_skeleton[0].coords[1] == (desk->startY + 1)
    )   return 1;

    else if (
        snake->direction == RIGHT &&
        snake->snake_skeleton[0].coords[1] == (desk->endY - 1)
    )   return 1;

    return 0;
}

int snakeLeftOfSnake(snake_t *snake) {

    for (int k = 0; k < snake->length; k++) {

        if (
            snake->direction == UP                                                          &&
            snake->snake_skeleton[0].coords[0] == (snake->snake_skeleton[k].coords[0] + 1)  &&
            snake->snake_skeleton[0].coords[1] == snake->snake_skeleton[k].coords[1]
        )   return 1;
        else if (
            snake->direction == DOWN                                                        &&
            snake->snake_skeleton[0].coords[0] == (snake->snake_skeleton[k].coords[0] - 1)  &&
            snake->snake_skeleton[0].coords[1] == snake->snake_skeleton[k].coords[1]
        )   return 1;
        else if (
            snake->direction == LEFT                                                        &&
            snake->snake_skeleton[0].coords[1] == (snake->snake_skeleton[k].coords[1] - 1)  &&
            snake->snake_skeleton[0].coords[0] == snake->snake_skeleton[k].coords[0]
        )   return 1;
        else if (
            snake->direction == RIGHT                                                       &&
            snake->snake_skeleton[0].coords[1] == (snake->snake_skeleton[k].coords[1] + 1)  &&
            snake->snake_skeleton[0].coords[0] == snake->snake_skeleton[k].coords[0]
        )   return 1;
    }
    return 0;
}

int snakeRightOfSnake(snake_t *snake) {

    for (int k = 0; k < snake->length; k++) {

        if (
            snake->direction == UP                                                          &&
            snake->snake_skeleton[0].coords[0] == (snake->snake_skeleton[k].coords[0] - 1)  &&
            snake->snake_skeleton[0].coords[1] == snake->snake_skeleton[k].coords[1]
        )   return 1;
        else if (
            snake->direction == DOWN                                                        &&
            snake->snake_skeleton[0].coords[0] == (snake->snake_skeleton[k].coords[0] + 1)  &&
            snake->snake_skeleton[0].coords[1] == snake->snake_skeleton[k].coords[1]
        )   return 1;
        else if (
            snake->direction == LEFT                                                        &&
            snake->snake_skeleton[0].coords[1] == (snake->snake_skeleton[k].coords[1] + 1)  &&
            snake->snake_skeleton[0].coords[0] == snake->snake_skeleton[k].coords[0]
        )   return 1;
        else if (
            snake->direction == RIGHT                                                       &&
            snake->snake_skeleton[0].coords[1] == (snake->snake_skeleton[k].coords[1] - 1)  &&
            snake->snake_skeleton[0].coords[0] == snake->snake_skeleton[k].coords[0]
        )   return 1;
    }
    return 0;
}

int checkFieldOccupation(snake_t *snake, desk_t *desk) {
    
    int freeLeft = snake->max_length, freeRight = snake->max_length;

    for (int k = 1; k < snake->length; k++) {

        if (
            snake->direction == UP                                                      &&
            snake->snake_skeleton[k].coords[0] < snake->snake_skeleton[0].coords[0]     &&
            snake->snake_skeleton[k].coords[1] == snake->snake_skeleton[0].coords[1]    &&
            snake->snake_skeleton[0].coords[0] - snake->snake_skeleton[k].coords[0] < freeLeft
        )   freeLeft = snake->snake_skeleton[0].coords[0] - snake->snake_skeleton[k].coords[0];
        if (
            snake->direction == UP                                                      &&
            snake->snake_skeleton[k].coords[0] > snake->snake_skeleton[0].coords[0]     &&
            snake->snake_skeleton[k].coords[1] == snake->snake_skeleton[0].coords[1]    &&
            snake->snake_skeleton[k].coords[0] - snake->snake_skeleton[0].coords[0] < freeRight
        )   freeRight = snake->snake_skeleton[k].coords[0] - snake->snake_skeleton[0].coords[0];
        if (
            snake->direction == DOWN                                                    &&
            snake->snake_skeleton[k].coords[0] < snake->snake_skeleton[0].coords[0]     &&
            snake->snake_skeleton[k].coords[1] == snake->snake_skeleton[0].coords[1]    &&
            snake->snake_skeleton[0].coords[0] - snake->snake_skeleton[k].coords[0] < freeRight
        )   freeRight = snake->snake_skeleton[0].coords[0] - snake->snake_skeleton[k].coords[0];
        if (
            snake->direction == DOWN                                                    &&
            snake->snake_skeleton[k].coords[0] > snake->snake_skeleton[0].coords[0]     &&
            snake->snake_skeleton[k].coords[1] == snake->snake_skeleton[0].coords[1]    &&
            snake->snake_skeleton[k].coords[0] - snake->snake_skeleton[0].coords[0] < freeLeft
        )   freeLeft = snake->snake_skeleton[k].coords[0] - snake->snake_skeleton[0].coords[0];
        if (
            snake->direction == LEFT                                                    &&
            snake->snake_skeleton[k].coords[1] < snake->snake_skeleton[0].coords[1]     &&
            snake->snake_skeleton[k].coords[0] == snake->snake_skeleton[0].coords[0]    &&
            snake->snake_skeleton[0].coords[1] - snake->snake_skeleton[k].coords[1] < freeRight
        )   freeRight = snake->snake_skeleton[0].coords[1] - snake->snake_skeleton[k].coords[1];
        if (
            snake->direction == LEFT                                                    &&
            snake->snake_skeleton[k].coords[1] > snake->snake_skeleton[0].coords[1]     &&
            snake->snake_skeleton[k].coords[0] == snake->snake_skeleton[0].coords[0]    &&
            snake->snake_skeleton[k].coords[1] - snake->snake_skeleton[0].coords[1] < freeLeft
        )   freeLeft = snake->snake_skeleton[k].coords[1] - snake->snake_skeleton[0].coords[1];
        if (
            snake->direction == RIGHT                                                   &&
            snake->snake_skeleton[k].coords[1] < snake->snake_skeleton[0].coords[1]     &&
            snake->snake_skeleton[k].coords[0] == snake->snake_skeleton[0].coords[0]    &&
            snake->snake_skeleton[0].coords[1] - snake->snake_skeleton[k].coords[1] < freeLeft
        )   freeLeft = snake->snake_skeleton[0].coords[1] - snake->snake_skeleton[k].coords[1];
        if (
            snake->direction == RIGHT                                                   &&
            snake->snake_skeleton[k].coords[1] > snake->snake_skeleton[0].coords[1]     &&
            snake->snake_skeleton[k].coords[0] == snake->snake_skeleton[0].coords[0]    &&
            snake->snake_skeleton[k].coords[1] - snake->snake_skeleton[0].coords[1] < freeRight
        )   freeRight = snake->snake_skeleton[k].coords[1] - snake->snake_skeleton[0].coords[1];
    }
    if (freeLeft < freeRight) return RIGHT;
    else return LEFT;
}
