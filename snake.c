#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
// #include <termios.h>

#include "snake.h"

snake_t *initSnake(int displayWidth, int displayHeight, int initSnakeX, int initSnakeY, char turnLeft, char turnRight) {

    snake_t *snake = (snake_t *)malloc(sizeof(snake_t));
    snake->length = INIT_LEN;
    snake->max_length = displayWidth * displayHeight;
    snake->score = 0;
    snake->direction = LEFT;
    snake->turnLeft = turnLeft;
    snake->turnRight = turnRight;
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
        if (ch == snake->turnLeft) {
          snakeTurnLeft(snake);
        }
        else if (ch == snake->turnRight) {
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
        snake->gameOver = 1;
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
        snake->gameOver = 1;
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
