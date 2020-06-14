#include <stdio.h>
#include <stdlib.h>

#include "snake.h"

desk_t *initDesk(int width, int height, int startX, int startY) {
    desk_t *desk = (desk_t *)malloc(sizeof(desk_t));
    desk->width = width;
    desk->height = height;
    desk->startX = startX;
    desk->startY = startY;
    desk->endX = startX + width;
    desk->endY = startY + height;
    return desk;
}

void freeDesk(desk_t *desk) {
    free(desk);
    desk = NULL;
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
