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
unsigned char *mem_base;
uint32_t rgb_knobs_value;

int size_cell = 20;
int size_score = 4;
int size_time = 4;
int size_GameOver = 5;
int size_retry = 4;
int size_quit = 4;
int size_standard = 5;
int size_demo = 5;
int size_speed = 5;

snake_t *initSnake(int displayWidth, int displayHeight, int initSnakeX, int initSnakeY, char turnLeft, char turnRight) {

    snake_t *snake = (snake_t *)malloc(sizeof(snake_t));
    snake->life = true;
    snake->length = INIT_LEN;
    snake->max_length = displayWidth * displayHeight;
    snake->score = 0;
    snake->direction = LEFT;

    //TODO
    //snake->turnLeft = turnLeft
    //snake->turnRight = turnRight

    snake->snake_skeleton = (section_t *)malloc(sizeof(section_t) * snake->max_length);
    for (int i = 0; i < INIT_LEN; i++) {

        snake->snake_skeleton[i].coords[0] = initSnakeX + i;
        snake->snake_skeleton[i].coords[1] = initSnakeY;
    }
    return snake;
}

snake_t *initSnakeAI(int displayWidth, int displayHeight, int initSnakeX, int initSnakeY) {

    snake_t *snake = (snake_t *)malloc(sizeof(snake_t));
    snake->life = true;
    snake->length = INIT_LEN;
    snake->max_length = displayWidth * displayHeight;
    snake->score = 0;
    snake->direction = LEFT;
    snake->snake_skeleton = (section_t *)malloc(sizeof(section_t) * snake->max_length);
    for (int i = 0; i < INIT_LEN; i++) {

        snake->snake_skeleton[i].coords[0] = initSnakeX + i;
        snake->snake_skeleton[i].coords[1] = initSnakeY;
    }
    return snake;
}

void draw_pixel(int x, int y, int color) {
  if (x>=0 && x<480 && y>=0 && y<320) {
    fb[x+480*y] = color;
  }
}

void draw_pixel_size(int x, int y, int size, int color) {
  int i, j;
  for (i = 0; i < size; i++){
    for (j = 0; j < size; j++){
      draw_pixel(x-i, y-j, color);
    }
  }
}

void draw_wall(int c, int r) {
  // up
  for (int i = 0; i < c+1; i++){
    for (int x = 0; x < size_cell; x++){
      for(int y = size_cell/2; y < size_cell; y++){
        draw_pixel(i*size_cell+x+(size_cell/4), 0+y, 65535);
      }
    }
  }
  // down
  for (int i = 0; i < c+1; i++){
    for (int x = 0; x < size_cell; x++){
      for(int y = size_cell/2; y < size_cell; y++){
        draw_pixel(i*size_cell+x+(size_cell/4), 300+y-(size_cell/2), 65535);
      }
    }
  }
  // left
  for (int i = 1; i < r+1; i++){
    for (int x = size_cell/2; x < size_cell; x++){
      for(int y = 0; y < size_cell; y++){
        draw_pixel(0+x, i*size_cell+y+(size_cell/4), 65535);
      }
    }
  }
  // right
  for (int i = 1; i < r+1; i++){
    for (int x = size_cell/2; x < size_cell; x++){
      for(int y = 0; y < size_cell; y++){
        draw_pixel(340+x-(size_cell/2), i*size_cell+y+(size_cell/4), 65535);
      }
    }
  }
}

void draw_food(int x, int y) {
  int i, j;
  for (i = 0; i < size_cell; i++){
    for (j = 0; j < size_cell; j++){
      draw_pixel(x+i, y+j, 2016);
    }
  }
}

void draw_snake(int x, int y) {
  int i, j;
  for (i = 0; i < size_cell; i++){
    for (j = 0; j < size_cell; j++){
      draw_pixel(x+i, y+j, 31);
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

void draw_char(int x, int y, font_descriptor_t* fdes, char ch, int size, int color) {
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
    printf("Znak %c na %i, %i, sirka %i\n", ch, x, y, w);
    int i, j;
    for (i = 0; i < fdes->height; i++){
      font_bits_t val = *ptr;
      for (j = 0; j < w; j++){
        if ((val&0x8000) != 0) {
          draw_pixel_size(x+size*j, y+size*i, size, color);
        }
        val<<=1;
      }
      ptr++;
    }
  }
}

void draw_score(int score){
    int y = 20;
    int x = 380;
    if (score==0){
        draw_char(x, y, fdes, '0', size_score+1, 63519);
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
        draw_char(x, y, fdes, str[i], size_score+1, 63519);
        x+=size_score*char_width(fdes, str[i])+2;
    }
}

void draw_time(int sec){
  int y = 127;
  int x = 380;
  if (sec==0){
      draw_char(x, y, fdes, '0', size_time, 63519);
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
      draw_char(x, y, fdes, str[i], size_time, 63519);
      x+=size_time*char_width(fdes, str[i])+2;
  }
}

void draw_EndGame(unsigned short *fb1, int score, int retry, int quit){
  parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);

  int ptr;
  fb = fb1;
  for (ptr = 0; ptr < 320*480 ; ptr++) {
        fb[ptr]=0u;
  }
  char str[] = "Game over"; // 9
  char *ch = str;
  int x = 54;
  for (int i=0; i<9; i++) {
      draw_char(x, 10, fdes, *ch, size_GameOver, 63519);
      x+=size_GameOver*char_width(fdes, *ch)+2;
      ch++;
  }
  x = 220;
  if (score==0){
      draw_char(x, 100, fdes, '0', size_score, 63519);
  }
  char str1[3] = "0";
  int idx = 0;
  while (score!=0)
  {
      str1[idx] = score % 10 + '0';
      score /= 10;
      idx++;
  }
  for (int i = idx-1; i >= 0; i--){
      draw_char(x, 100, fdes, str1[i], size_score, 63519);
      x+=size_score*char_width(fdes, str1[i])+2;
  }
  char RETRY[] = "RETRY"; // 5
  char *RE = RETRY;
  x = 142-(24*retry);
  for (int i=0; i<5; i++) {
      draw_char(x, 180, fdes, *RE, retry+size_retry, 63519+(2016*retry));
      x+=(size_retry+retry)*char_width(fdes, *RE)+2;
      RE++;
  }
  char QUIT[] = "QUIT"; // 4
  char *QU = QUIT;
  x = 173-(13*quit);
  for (int i=0; i<4; i++) {
      draw_char(x, 250, fdes, *QU, quit+size_quit, 63519+(2016*quit));
      x+=(size_quit+quit)*char_width(fdes, *QU)+2;
      QU++;
  }

  // draw LCD
    parlcd_write_cmd(parlcd_mem_base, 0x2c);
    for (ptr = 0; ptr < 480*320 ; ptr++) {
        parlcd_write_data(parlcd_mem_base, fb[ptr]);
    }

    // draw LED
    rgb_knobs_value = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    rgb_knobs_value =16711680; //red

    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = rgb_knobs_value;
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o) = rgb_knobs_value;
}

void draw_Menu(unsigned short *fb1, int standard, int demo){
    parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
    int ptr;
    fb = fb1;
    for (ptr = 0; ptr < 320*480 ; ptr++) {
        fb[ptr]=0u;
    }
    char str[] = "STANDARD"; // 8
    char *ch = str;
    int x = 49-(standard*36);
    for (int i=0; i<8; i++) {
        draw_char(x, 50, fdes, *ch, size_standard+standard, 63519+(2016*standard));
        x+=(size_standard+standard)*char_width(fdes, *ch)+2;
        ch++;
    }
    char str1[] = "DEMO"; // 4
    char *ch1 = str1;
    x = 134-(23*demo);
    for (int i=0; i<4; i++) {
        draw_char(x, 170, fdes, *ch1, size_demo+demo, 63519+(2016*demo));
        x+=(size_demo+demo)*char_width(fdes, *ch1)+2;
        ch1++;
    }
    // draw LCD
    parlcd_write_cmd(parlcd_mem_base, 0x2c);
    for (ptr = 0; ptr < 480*320 ; ptr++) {
        parlcd_write_data(parlcd_mem_base, fb[ptr]);
    }
}

void draw_speed_ctrl(unsigned short *fb1, int slow, int medium, int fast){
    parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
    int ptr;
    fb = fb1;
    for (ptr = 0; ptr < 320*480 ; ptr++) {
            fb[ptr]=0u;
    }
    char str[] = "Set speed"; // 9
    char *ch = str;
    int x = 49;
    for (int i=0; i<9; i++) {
        draw_char(x, 20, fdes, *ch, size_speed, 63519);
        x+=size_speed*char_width(fdes, *ch)+2;
        ch++;
    }
    char str1[] = "Slow"; // 4
    char *ch1 = str1;
    x = 49;
    for (int i=0; i<4; i++) {
        draw_char(x, 130, fdes, *ch1, 4, 63519+(2016*slow));
        x+=4*char_width(fdes, *ch1)+2;
        ch1++;
    }
    char str2[] = "Medium"; // 6
    char *ch2 = str2;
    x = 49;
    for (int i=0; i<6; i++) {
        draw_char(x, 190, fdes, *ch2, 4, 63519+(2016*medium));
        x+=4*char_width(fdes, *ch2)+2;
        ch2++;
    }
    char str3[] = "Fast"; // 4
    char *ch3 = str3;
    x = 49;
    for (int i=0; i<4; i++) {
        draw_char(x, 250, fdes, *ch3, 4, 63519+(2016*fast));
        x+=4*char_width(fdes, *ch3)+2;
        ch3++;
    }
    // draw LCD
    parlcd_write_cmd(parlcd_mem_base, 0x2c);
    for (ptr = 0; ptr < 480*320 ; ptr++) {
        parlcd_write_data(parlcd_mem_base, fb[ptr]);
    }
}

void freeSnake(snake_t *snake) {

    free(snake->snake_skeleton);
    snake->snake_skeleton = NULL;
    free(snake);
    snake = NULL;
}

void moveSnakeManual(snake_t *snake, food_t *food, desk_t *desk) {
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
          snakeTurnLeft(snake);
        }
        else if (ch == 'd') {
          snakeTurnRight(snake);
        }
    }

    //logic part--------
    snakeStep(snake);
    if (checkCollisions(snake, desk)) {
        printf("Game over!\n");
        snake->life = false;
        //change to gameover menu
    }

    snakeEats(food, snake, desk, lastCoords[0], lastCoords[1]);

}

void moveSnakeAI(snake_t *snake, food_t *food, desk_t *desk) {

    //inputs part-------
    int lastCoords[2] = {
        snake->snake_skeleton[snake->length].coords[0],
        snake->snake_skeleton[snake->length].coords[1], 
    };

    for (int k = snake->length; k > 0; k--) {

        snake->snake_skeleton[k].coords[0] = snake->snake_skeleton[k - 1].coords[0];
        snake->snake_skeleton[k].coords[1] = snake->snake_skeleton[k - 1].coords[1];
    }

    snakeController(snake, desk, food);

    //logic part--------
    snakeStep(snake);

    if (checkCollisions(snake, desk)) {

        printf("Game over!\n");
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
        mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
        rgb_knobs_value = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
        rgb_knobs_value = 255; // blue

        *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = rgb_knobs_value;
        *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o) = rgb_knobs_value;
        increaseSnake(snake, lastCoordX, lastCoordY);
        updateFood(desk, food, snake);
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

void drawDesk(desk_t *desk, snake_t *snake, food_t *food, int sec, unsigned short *fb1) {
    // parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
    
    int ptr;
    fb = fb1;
    for (ptr = 0; ptr < 480*320 ; ptr++) {
        fb[ptr] = 0;
    }
    draw_score(snake->score);
    draw_time(sec);
    draw_wall(16, 14); // dobavit parametry

    for (int i = desk->startY; i < desk->endY; i ++) {
        for (int j = desk->startX; j < desk->endX; j++) {
            if (
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

void snakeController(snake_t *snake, desk_t *desk, food_t *food) {

    int dx, dy;

    dx = food->coord[0] - snake->snake_skeleton[0].coords[0];
    dy = food->coord[1] - snake->snake_skeleton[0].coords[1];

    printf("Snake actual direction: %d\n\r", snake->direction);

    if (obstacleBeforeSnake(snake, desk) && obstacleLeftOfSnake(snake, desk)) {
            
        snakeTurnRight(snake);
        printf("Obstacle front and left\n\r");
    }

    else if (obstacleBeforeSnake(snake, desk) && obstacleRightOfSnake(snake, desk)) {
        
        snakeTurnLeft(snake);
        printf("Obstacle front and right\n\r");
    }

    else if (obstacleBeforeSnake(snake, desk)) {
     
        if (checkFieldOccupation(snake, desk) == LEFT) snakeTurnLeft(snake);
        else if (checkFieldOccupation(snake, desk) == RIGHT) snakeTurnRight(snake);
    }

    else if (snakeLeftOfSnake(snake))
        ;

    else if (snakeRightOfSnake(snake))
        ;

    else {
        switch (snake->direction) {

            case LEFT:
                if (dy < 0 && dx == 0) snakeTurnRight(snake);
                else if (dy > 0 && dx == 0) snakeTurnLeft(snake);
                break;
            case RIGHT:
                if (dy > 0 && dx == 0) snakeTurnRight(snake);
                else if (dy < 0 && dx == 0) snakeTurnLeft(snake);
                break;
            case UP:
                if (dy == 0 && dx > 0) snakeTurnRight(snake);
                else if (dy == 0 && dx < 0) snakeTurnLeft(snake);
                break;
            case DOWN:
                if (dy == 0 && dx < 0) snakeTurnRight(snake);
                else if (dy == 0 && dx > 0) snakeTurnLeft(snake);
                break;
        }
    }
}

void snakeTurnLeft(snake_t *snake) {

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

void snakeTurnRight(snake_t *snake) {

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
