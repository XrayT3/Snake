#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
// #include <time.h>
// #include <stdbool.h>
#include <termios.h>

#include "snake.h"
// #include "gui.h"

// #include "mzapo_parlcd.h"
// #include "mzapo_phys.h"
// #include "mzapo_regs.h"
// #include "font_types.h"
// #include "font_prop14x16.c"

// unsigned short *fb;
// font_descriptor_t* fdes = &font_winFreeSystem14x16;
// unsigned char *parlcd_mem_base;
// unsigned char *mem_base;
// uint32_t rgb_knobs_value;

// int size_Cell = 20;
// int size_GameOver = 5;
// int size_retry = 4;
// int size_quit = 4;
// int size_standard = 5;
// int size_demo = 5;
// int size_speed = 5;
// int size_exit = 4;

snake_t *initSnake(int displayWidth, int displayHeight, int initSnakeX, int initSnakeY, char turnLeft, char turnRight) {

    snake_t *snake = (snake_t *)malloc(sizeof(snake_t));
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

// void draw_pixel(int x, int y, int color) {
//   if (x>=0 && x<480 && y>=0 && y<320) {
//     fb[x+480*y] = color;
//   }
// }

// void draw_EndGame(unsigned short *fb1, int score, int retry, int quit){
//   int ptr;
//   fb = fb1;
//   for (ptr = 0; ptr < 320*480 ; ptr++) {
//         fb[ptr]=0u;
//   }
//   char str[] = "Game over"; // 9
//   char *ch = str;
//   int x = 54;
//   for (int i=0; i<9; i++) {
//       draw_char(x, 10, *ch, size_GameOver, 63519);
//       x+=size_GameOver*char_width(*ch)+2;
//       ch++;
//   }
//   x = 220;
//   if (score==0){
//       draw_char(x, 100, '0', 4, 63519); // size_score = 4;
//   }
//   char str1[3] = "0";
//   int idx = 0;
//   while (score!=0)
//   {
//       str1[idx] = score % 10 + '0';
//       score /= 10;
//       idx++;
//   }
//   for (int i = idx-1; i >= 0; i--){
//       draw_char(x, 100, str1[i], 4, 63519); // size_score = 4;
//       x+=4*char_width(str1[i])+2; // size_score = 4;
//   }
//   char RETRY[] = "RETRY"; // 5
//   char *RE = RETRY;
//   x = 142-(24*retry);
//   for (int i=0; i<5; i++) {
//       draw_char(x, 180, *RE, retry+size_retry, 63519+(2016*retry));
//       x+=(size_retry+retry)*char_width(*RE)+2;
//       RE++;
//   }
//   char QUIT[] = "QUIT"; // 4
//   char *QU = QUIT;
//   x = 173-(13*quit);
//   for (int i=0; i<4; i++) {
//       draw_char(x, 250, *QU, quit+size_quit, 63519+(2016*quit));
//       x+=(size_quit+quit)*char_width(*QU)+2;
//       QU++;
//   }

//   // draw LCD
//     parlcd_write_cmd(parlcd_mem_base, 0x2c);
//     for (ptr = 0; ptr < 480*320 ; ptr++) {
//         parlcd_write_data(parlcd_mem_base, fb[ptr]);
//     }
// }

// void draw_Menu(unsigned short *fb1, int standard, int demo, int exit){
//     parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
//     int ptr;
//     fb = fb1;
//     for (ptr = 0; ptr < 320*480 ; ptr++) {
//         fb[ptr]=0u;
//     }
//     char str[] = "STANDARD"; // 8
//     char *ch = str;
//     int x = 20;
//     // int x = 49-(standard*36);
//     for (int i=0; i<8; i++) {
//         draw_char(x, 20, *ch, size_standard+standard, 63519+(2016*standard));
//         x+=(size_standard+standard)*char_width(*ch)+2;
//         ch++;
//     }
//     char str1[] = "DEMO"; // 4
//     char *ch1 = str1;
//     x = 20;
//     // x = 134-(23*demo);
//     for (int i=0; i<4; i++) {
//         draw_char(x, 130, *ch1, size_demo+demo, 63519+(2016*demo));
//         x+=(size_demo+demo)*char_width(*ch1)+2;
//         ch1++;
//     }
//     char str2[] = "EXIT"; // 4
//     char *ch2 = str2;
//     x = 20;
//     // x = 177-(15*exit);
//     for (int i=0; i<4; i++) {
//         draw_char(x, 240, *ch2, size_exit+exit, 63519+(2016*exit));
//         x+=(size_exit+exit)*char_width(*ch2)+2;
//         ch2++;
//     }
//     // draw LCD
//     parlcd_write_cmd(parlcd_mem_base, 0x2c);
//     for (ptr = 0; ptr < 480*320 ; ptr++) {
//         parlcd_write_data(parlcd_mem_base, fb[ptr]);
//     }
// }

// void draw_speed_ctrl(unsigned short *fb1, int slow, int medium, int fast){
//     int ptr;
//     fb = fb1;
//     for (ptr = 0; ptr < 320*480 ; ptr++) {
//             fb[ptr]=0u;
//     }
//     char str[] = "Set speed"; // 9
//     char *ch = str;
//     int x = 49;
//     for (int i=0; i<9; i++) {
//         draw_char(x, 20, *ch, size_speed, 63519);
//         x+=size_speed*char_width(*ch)+2;
//         ch++;
//     }
//     char str1[] = "Slow"; // 4
//     char *ch1 = str1;
//     x = 49;
//     for (int i=0; i<4; i++) {
//         draw_char(x, 130, *ch1, 4, 63519+(2016*slow));
//         x+=4*char_width(*ch1)+2;
//         ch1++;
//     }
//     char str2[] = "Medium"; // 6
//     char *ch2 = str2;
//     x = 49;
//     for (int i=0; i<6; i++) {
//         draw_char(x, 190, *ch2, 4, 63519+(2016*medium));
//         x+=4*char_width(*ch2)+2;
//         ch2++;
//     }
//     char str3[] = "Fast"; // 4
//     char *ch3 = str3;
//     x = 49;
//     for (int i=0; i<4; i++) {
//         draw_char(x, 250, *ch3, 4, 63519+(2016*fast));
//         x+=4*char_width(*ch3)+2;
//         ch3++;
//     }
//     // draw LCD
//     parlcd_write_cmd(parlcd_mem_base, 0x2c);
//     for (ptr = 0; ptr < 480*320 ; ptr++) {
//         parlcd_write_data(parlcd_mem_base, fb[ptr]);
//     }

//     // LED
//     mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
//     rgb_knobs_value = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
//     rgb_knobs_value = 16711935; //pink

//     *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = rgb_knobs_value;
//     *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o) = rgb_knobs_value;
// }

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
    if (
        checkWallsCollisions(snake, desk)   ||
        checkItselfCollisions(snake)        
    ) {

        // printf("Gameover!\n");
        desk->gameOver = 1;
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

    if (
        checkWallsCollisions(snake, desk)   ||
        checkItselfCollisions(snake)        
    ) {

        // printf("Gameover!\n");
        desk->gameOver = 1;
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

// void drawDesk(desk_t *desk, snake_t *snake, food_t *food, int sec, unsigned short *fb1) {
//     int ptr;
//     fb = fb1;
//     for (ptr = 0; ptr < 480*320 ; ptr++) {
//         fb[ptr] = 0;
//     }
//     draw_score(snake->score);
//     draw_time(sec);
//     draw_wall(16, 14);

//     for (int i = desk->startY; i < desk->endY; i ++) {
//         for (int j = desk->startX; j < desk->endX; j++) {
//             if (
//                 (j == food->coord[0]) &&
//                 (i == food->coord[1])
//             )
//             draw_food(j*size_Cell, i*size_Cell); // food
//             else{
//                 for (int k = 0; k < snake->length; k++) {

//                     if (
//                         j == (snake->snake_skeleton[k].coords[0]) &&
//                         i == (snake->snake_skeleton[k].coords[1]) 
//                     )
//                     draw_snake(j*size_Cell, i*size_Cell); // snake
//                 }
//             }
//         }
//     }

//     // draw LCD
//     parlcd_write_cmd(parlcd_mem_base, 0x2c);
//     for (ptr = 0; ptr < 480*320 ; ptr++) {
//         parlcd_write_data(parlcd_mem_base, fb[ptr]);
//     }
// }

// void drawDesk_2_snakes(desk_t *desk, snake_t *snake, snake_t *snake2, food_t *food, int sec, unsigned short *fb1) {
//     int ptr;
//     fb = fb1;
//     for (ptr = 0; ptr < 480*320 ; ptr++) {
//         fb[ptr] = 0;
//     }
//     draw_score(snake->score);
//     draw_score_2_snakes(snake2->score);
//     draw_time(sec);
//     draw_wall(16, 14); 
//     for (int i = desk->startY; i < desk->endY; i ++) {
//         for (int j = desk->startX; j < desk->endX; j++) {
//             if (
//                 (j == food->coord[0]) &&
//                 (i == food->coord[1])
//             )
//             draw_food(j*size_Cell, i*size_Cell); // food
//             else{
//                 for (int k = 0; k < snake->length; k++) {
//                     if (
//                         j == (snake->snake_skeleton[k].coords[0]) &&
//                         i == (snake->snake_skeleton[k].coords[1]) 
//                     )
//                     draw_snake(j*size_Cell, i*size_Cell); // snake
//                 }
//                 for (int k = 0; k < snake2->length; k++) {
//                     if (
//                         j == (snake2->snake_skeleton[k].coords[0]) &&
//                         i == (snake2->snake_skeleton[k].coords[1]) 
//                     )
//                     draw_snake(j*size_Cell, i*size_Cell); // snake2
//                 }
//             }
//         }
//     }

//     // draw LCD
//     parlcd_write_cmd(parlcd_mem_base, 0x2c);
//     for (ptr = 0; ptr < 480*320 ; ptr++) {
//         parlcd_write_data(parlcd_mem_base, fb[ptr]);
//     }
// }

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

    // printf("Snake actual direction: %d\n\r", snake->direction);

    if (obstacleBeforeSnake(snake, desk) && obstacleLeftOfSnake(snake, desk)) {
            
        snakeTurnRight(snake);
        // printf("Obstacle front and left\n\r");
    }

    else if (obstacleBeforeSnake(snake, desk) && obstacleRightOfSnake(snake, desk)) {
        
        snakeTurnLeft(snake);
        // printf("Obstacle front and right\n\r");
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
