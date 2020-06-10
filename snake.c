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
font_descriptor_t* fdes = &font_winFreeSystem14x16;
unsigned char *parlcd_mem_base;
int size_cell = 20;
int size_score = 4;

snake_t initSnake(int displayWidth, int displayHeight, int initialSnakeLength, int initSnakeX, int initSnakeY) {

    snake_t *snake = (snake_t *)malloc(sizeof(snake_t));
    snake->length = initialSnakeLength;
    snake->max_length = displayWidth * displayHeight;
    snake->score = 0;
    snake->snake_skeleton = (section_t *)malloc(sizeof(section_t) * snake->max_length);
    for (int i = 0; i < initialSnakeLength; i++) {
        snake->snake_skeleton[i].coords[0] = initSnakeX + i;
        snake->snake_skeleton[i].coords[1] = initSnakeY;
    }
    return *snake;
}

food_t initFood(int coordX, int coordY) {

    food_t *food = (food_t *)malloc(sizeof(food_t));
    food->coord[0] = coordX;
    food->coord[1] = coordY;
    return *food;
}

void draw_pixel(int x, int y, int color) {
  if (x>=0 && x<480 && y>=0 && y<320) {
    fb[x+480*y] = color;
  }
}

void draw_pixel8(int x, int y) {
  int i, j;
  for (i = 0; i < size_score; i++){
    for (j = 0; j < size_score; j++){
      draw_pixel(x-i, y-j, 0x1f<<11);
    }
  }
}

void draw_wall(int x, int y) {
  int i, j;
  for (i = 0; i < size_cell; i++){
    for (j = 0; j < size_cell; j++){
      draw_pixel(x-i, y-j, 0x1f<<11);
    }
  }
}

void draw_food(int x, int y) {
  int i, j;
  for (i = 0; i < size_cell; i++){
    for (j = 0; j < size_cell; j++){
      draw_pixel(x+i, y+j, 0x1f<<5);
    }
  }
}

void draw_snake(int x, int y) {
  int i, j;
  for (i = 0; i < size_cell; i++){
    for (j = 0; j < size_cell; j++){
      draw_pixel(x+i, y+j, 0x1f<<0);
    }
  }
}

int char_width(font_descriptor_t* fdes, int ch) {
  int width = 0;
  if ((ch >= fdes->firstchar) && (ch-fdes->firstchar < fdes->size)) {
    ch -= fdes->firstchar;
    if (!fdes->width) {
      width = fdes->maxwidth;
    } else {
      width = fdes->width[ch];
    }
  }
  return width;
}

void draw_char(int x, int y, font_descriptor_t* fdes, char ch) {
  int w = char_width(fdes, ch);
  if (w > 0) {
    const font_bits_t *ptr;
    if (fdes->offset) {
      ptr = &fdes->bits[fdes->offset[ch-fdes->firstchar]];
      ptr = fdes->bits + fdes->offset[ch-fdes->firstchar];
    } else {
      int bw = (fdes->maxwidth+15)/16;
      ptr = fdes->bits + (ch-fdes->firstchar)*bw*fdes->height;
    }
    //printf("Znak %c na %i, %i, sirka %i\n", ch, x, y, w);
    int i, j;
    for (i = 0; i < fdes->height; i++){
      font_bits_t val = *ptr;
      for (j = 0; j < w; j++){
        if ((val&0x8000) != 0) {
          draw_pixel8(x+size_score*j, y+size_score*i);
        }
        val<<=1;
      }
      ptr++;
    }
  }
}

void draw_score(int score){
    char str[5] = "0";
    int idx = 0;
    int x = 10;
    while (score!=0)
    {
        str[idx] = score % 10 + '0';
        score /= 10;
        idx++;
    }
    for (int i = idx-1; i >= 0; i--){
        draw_char(370, x, fdes, str[i]);
        x+=size_score*char_width(fdes, str[i])+2;
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
        snakeEats(food, snake, desk, lastCoords[0], lastCoords[1]);
    }

    if (checkCollisions(snake)) {

        //printf("Gameover!\n");
        // exit(1);
        //change to gameover menu
    }

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
        draw_score(snake->score);
        increaseSnake(snake, lastCoordX, lastCoordY);
        updateFood(desk, food);
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
    draw_score(snake->score);

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
                    draw_snake(j*size_cell, i*size_cell); // snake
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
