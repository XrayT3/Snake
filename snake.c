#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <time.h>
#include <termios.h>

#include "snake.h"

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "font_types.h"
#include "font_prop14x16.c"

unsigned short *fb;
unsigned char *parlcd_mem_base;
int scale = 25;
int size_cell = 20;

snake_t initSnake(int displayWidth, int displayHeight, int initialSnakeLength, int initSnakeX, int initSnakeY) {

    snake_t *snake = (snake_t *)malloc(sizeof(snake_t));
    snake->length = initialSnakeLength;
    snake->max_length = displayWidth * displayHeight;
    snake->score = 0;
    snake->snake_skeleton = (section_t *)malloc(sizeof(section_t) * snake->max_length);
    for (int i = 0; i < initialSnakeLength; i++) {
        snake->snake_skeleton[i].coords[0] = initSnakeX + i;
        snake->snake_skeleton[i].coords[1] = initSnakeY;
        printf("Coords - %d %d\n", snake->snake_skeleton[i].coords[0], snake->snake_skeleton[i].coords[1]);
    }
    getchar();
    return *snake;
}

food_t initFood(int coordX, int coordY) {

    food_t *food = (food_t *)malloc(sizeof(food_t));
    food->coord[0] = coordX;
    food->coord[1] = coordY;
    return *food;
}

void draw_pixel(int x, int y) {
  if (x>=0 && x<480 && y>=0 && y<320) {
    fb[x+480*y] = 0x1f<<11;
  }
}

void draw_pixel8(int x, int y) {
  int i, j;
  for (i = 0; i < scale; i++){
    for (j = 0; j < scale; j++){
      draw_pixel(x+i, y+j);
    }
  }
}

void draw_wall(int x, int y) {
  int i, j;
  for (i = 0; i < size_cell; i++){
    for (j = 0; j < size_cell; j++){
      draw_pixel(x+i, y+j);
    }
  }
}

void draw_food(int x, int y) {
  int i, j;
  for (i = 0; i < size_cell; i++){
    for (j = 0; j < size_cell; j++){
      draw_pixel(x+i, y+j);
    }
  }
}

void draw_snake(int x, int y) {
  int i, j;
  for (i = 0; i < size_cell; i++){
    for (j = 0; j < size_cell; j++){
      draw_pixel(x+i, y+j);
    }
  }
}

void moveSnake(snake_t *snake, food_t *food, desk_t *desk) {
    char ch;
    int r = read(0, &ch, 1);

    if (r==1)
    {
        int lastCoords[2] = {
        snake->snake_skeleton[snake->length].coords[0],
        snake->snake_skeleton[snake->length].coords[1], 
        };

        for (int k = snake->length; k > 0; k--) {
            snake->snake_skeleton[k].coords[0] = snake->snake_skeleton[k - 1].coords[0];
            snake->snake_skeleton[k].coords[1] = snake->snake_skeleton[k - 1].coords[1];
        }

        switch (ch) {
        
        case 'a':
            printf("left");
            snake->snake_skeleton->coords[0] -= 1;
            break;
        case 'd':
            printf("right");
            snake->snake_skeleton->coords[0] += 1;
            break;
        case 'w':
            printf("up");
            snake->snake_skeleton->coords[1] -= 1;
            break;
        case 's':
            printf("down");
            snake->snake_skeleton->coords[1] += 1;
            break;
        }
    }

    if (checkCollisions(snake)) {

        //printf("Gameover!\n");
        // exit(1);
        //change to gameover menu
    }

    snakeEats(food, snake, desk, lastCoords[0], lastCoords[1]);
}

void increaseSnake(snake_t *snake, int coordX, int coordY) {

    snake->length += 1;
    snake->snake_skeleton[snake->length].coords[0] = coordX;
    snake->snake_skeleton[snake->length].coords[1] = coordY;
}

void snakeEats(food_t *food, snake_t *snake, desk_t *desk, int lastCoordX, int lastCoordY) {

    if (
        snake->snake_skeleton[0].coords[0] == food->coord[0] &&
        snake->snake_skeleton[0].coords[1] == food->coord[1]
    ) {

        snake->score += 1;
        increaseSnake(snake, lastCoordX, lastCoordY);
        updateFood(desk, food);
        printf("\n\rsome\n\r");
    }
}

int checkCollisions(snake_t *snake) {

    int ret = 0;
    for (int k = 1; k < snake->length ; k++) {

        if (
            snake->snake_skeleton[k].coords[0] == snake->snake_skeleton[0].coords[0] &&
            snake->snake_skeleton[k].coords[1] == snake->snake_skeleton[0].coords[1] 

        ) {
            ret = 1;
            break;
        }
    }
    return ret;
}

desk_t initDesk(int width, int height, int startX, int startY) {

    desk_t *desk = (desk_t *)malloc(sizeof(desk_t));
    desk->width = width;
    desk->height = height;
    desk->startX = startX;
    desk->startY = startY;
    desk->endX = startX + width;
    desk->endY = startY + height;

    int ptr;
    fb  = (unsigned short *)malloc(320*480*2);
    parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
    if (parlcd_mem_base == NULL)
        exit(1);
    parlcd_hx8357_init(parlcd_mem_base);

    for (ptr = 0; ptr < 320*480 ; ptr++) {
        fb[ptr]=0u;
    }

    return *desk;
}

void drawDesk(desk_t *desk, snake_t *snake, food_t *food) {

    int ptr;
    fb  = (unsigned short *)malloc(320*480*2);

    for (int i = desk->startY; i < desk->endY; i ++) {
        for (int j = desk->startX; j < desk->endX; j++) {
            //draw_pixel(i, j);
            if (j == desk->startX) {
                draw_wall(j*size_cell, i*size_cell);
                continue;
            }
            else if (j == (desk->endX - 1)) {
                draw_wall(j*size_cell, i*size_cell);
                continue;
            }
            else if (i == desk->startY) {
                draw_wall(j*size_cell, i*size_cell);
                continue;
            }
            else if (i == (desk->endY - 1)) {
                draw_wall(j*size_cell, i*size_cell);
                continue;
            }
            else if (
                (j == food->coord[0]) &&
                (i == food->coord[1])
            )
            draw_food(j*size_cell, i*size_cell); // food
            else{
                for (int k = 0; k < snake->length; k++) {

                    if (
                        j == (snake->snake_skeleton[k].coords[0]) &&
                        i == (snake->snake_skeleton[k].coords[1]) 
                    )
                    {
                    printf("%d %d ----- %d\n", i, j, k);
                    draw_snake(j*size_cell, i*size_cell); 
                    }// snake
                }
            }
        }
    }

    // draw LCD
    parlcd_write_cmd(parlcd_mem_base, 0x2c);
        for (ptr = 0; ptr < 480*320 ; ptr++) {
            parlcd_write_data(parlcd_mem_base, fb[ptr]);
        }
}

void updateFood(desk_t *desk, food_t *food) {

    int newX, newY;
    int maxX = desk->endX, minX = desk->startX;
    int maxY = desk->endY, minY = desk->startY;
    srand(time(NULL));

    newX = rand() % (maxX - minX) + minX;
    newY = rand() % (maxY - minY) + minY;
    printf("New coords: %d %d\n", newX, newY);
    food->coord[0] = newX;
    food->coord[1] = newY;
}
