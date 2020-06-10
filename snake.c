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
#include "font_types.h"
#include "font_prop14x16.c"

unsigned short *fb;
font_descriptor_t* fdes = &font_winFreeSystem14x16;
unsigned char *parlcd_mem_base;
int size_cell = 20;
int size_score = 4;

snake_t initSnake(int displayWidth, int displayHeight, int initSnakeX, int initSnakeY) {

    snake_t *snake = (snake_t *)malloc(sizeof(snake_t));
    snake->life = true;
    snake->length = INIT_LEN;
    snake->max_length = displayWidth * displayHeight;
    snake->score = 0;
    snake->direction = DOWN;
    snake->snake_skeleton = (section_t *)malloc(sizeof(section_t) * snake->max_length);
    for (int i = 0; i < INIT_LEN; i++) {

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

void draw_wall_LU(int x, int y) {
  int i, j;
  for (i = 0; i < size_cell; i++){
    for (j = 0; j < size_cell; j++){
      draw_pixel(x-i, y-j, 0x1f<<11);
    }
  }
}

void draw_wall_Rirht(int x, int y) {
  int i, j;
  for (i = 0; i < size_cell; i++){
    for (j = 0; j < size_cell; j++){
      draw_pixel(x+i, y+j, 0x1f<<11);
    }
  }
}

void draw_wall_Down(int x, int y) {
  int i, j;
  for (i = 0; i < size_cell; i++){
    for (j = 0; j < size_cell; j++){
      draw_pixel(x-i, y+j, 0x1f<<11);
    }
  }
}

void draw_wall() {
  // up
  for (int i = 0; i < 18-1; i++){
    for (int x = 0; x < size_cell; x++){
      for(int y = 0; y < size_cell; y++){
        draw_pixel((i+x)*size_cell, (0+y)*size_cell, 0x1f<<11);
      }
    }
  }
  // left
  for (int i = 1; i < 16-1; i++){
    for (int x = 0; x < size_cell; x++){
      for(int y = 0; y < size_cell; y++){
        draw_pixel(0+x, i+y, 0x1f<<11);
      }
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
    int y = 20;
    int x = 370;
    if (score==0){
        draw_char(x, y, fdes, '0');
        return;
    }
    char str[3] = "0";
    int idx = 0;
    while (score!=0)
    {
        str[idx] = score % 10 + '0';
        score /= 10;
        idx++;
    }
    for (int i = idx-1; i >= 0; i--){
        draw_char(x, y, fdes, str[i]);
        x+=size_score*char_width(fdes, str[i])+2;
    }
}

void draw_time(int sec){
  int y = 127;
  int x = 370;
  if (sec==0){
      draw_char(x, y, fdes, '0');
      return;
  }
  char str[3] = "0";
  int idx = 0;
  while (sec!=0)
  {
      str[idx] = sec % 10 + '0';
      sec /= 10;
      idx++;
  }
  for (int i = idx-1; i >= 0; i--){
      draw_char(x, y, fdes, str[i]);
      x+=size_score*char_width(fdes, str[i])+2;
  }
}

void moveSnake(snake_t *snake, food_t *food, desk_t *desk) {
    char ch;
    int r = read(0, &ch, 1);

    int lastCoords[2] = {
        snake->snake_skeleton[snake->length].coords[0],
        snake->snake_skeleton[snake->length].coords[1], 
    };

    for (int k = snake->length; k > 0; k--) {
        snake->snake_skeleton[k].coords[0] = snake->snake_skeleton[k - 1].coords[0];
        snake->snake_skeleton[k].coords[1] = snake->snake_skeleton[k - 1].coords[1];
    }

    if (r==1)
    {   
        if (ch == 'a') {

          switch (snake->direction) {

              case LEFT:
                  snake->direction = DOWN;
                  break;
              case DOWN:
                  snake->direction = RIGHT;
                  break;              
              case RIGHT:
                  snake->direction = UP;
                  break;
              case UP:
                  snake->direction = LEFT;
                  break;
          }
        }
        else if (ch == 'd') {

            switch (snake->direction) {

                case LEFT:
                    snake->direction = UP;
                    break;
                case DOWN:
                    snake->direction = LEFT;
                    break;              
                case RIGHT:
                    snake->direction = DOWN;
                    break;
                case UP:
                    snake->direction = RIGHT;
                    break;
            }
        }
    }

    //logic part--------
    snakeStep(snake);
    if (checkCollisions(snake, desk)) {
        printf("Gameover!\n");
        snake->life = false;
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
        draw_score(snake->score);
        increaseSnake(snake, lastCoordX, lastCoordY);
        updateFood(desk, food);
    }
}

int checkCollisions(snake_t *snake, desk_t *desk) {
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
    if (
        snake->snake_skeleton[0].coords[0] == desk->startX  ||
        snake->snake_skeleton[0].coords[0] == desk->endX    ||
        snake->snake_skeleton[0].coords[1] == desk->startY  ||
        snake->snake_skeleton[0].coords[1] == desk->endY
    ) {
        ret = 1;
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

void drawDesk(desk_t *desk, snake_t *snake, food_t *food, int sec, unsigned short *fb1) {

    int ptr;
    fb = fb1;
    for (ptr = 0; ptr < 480*320 ; ptr++) {
        fb[ptr] = 0;
    }
    draw_score(snake->score);
    draw_time(sec);
    draw_wall();

    for (int i = desk->startY; i < desk->endY; i ++) {
        for (int j = desk->startX; j < desk->endX; j++) {
            if (j == desk->startX) {
                //draw_wall_LU(j*size_cell, i*size_cell);
                continue;
            }
            else if (j == (desk->endX - 1)) {
                //draw_wall_Down((j+1)*size_cell, (i-1)*size_cell);
                continue;
            }
            else if (i == desk->startY) {
                //draw_wall_LU(j*size_cell, i*size_cell);
                continue;
            }
            else if (i == (desk->endY - 1)) {
                //draw_wall_Rirht((j-1)*size_cell, (i+1)*size_cell);
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
    int maxX = desk->endX - 1, minX = desk->startX + 1;
    int maxY = desk->endY - 1, minY = desk->startY + 1;
    srand(time(NULL));

    newX = rand() % (maxX - minX) + minX;
    newY = rand() % (maxY - minY) + minY;
    printf("New coords: %d %d\n", newY, newX);
    food->coord[0] = newX;
    food->coord[1] = newY;
}

void snakeStep(snake_t *snake) {

    switch (snake->direction) {

        case LEFT:
            snake->snake_skeleton[0].coords[0] -= 1;
            break;
        case RIGHT:
            snake->snake_skeleton[0].coords[0] += 1;
            break;
        case UP:
            snake->snake_skeleton[0].coords[1] -= 1;
            break;
        case DOWN:
            snake->snake_skeleton[0].coords[1] += 1;
            break;
    }
}
