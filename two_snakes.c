#include "snake.h"

void moveSnakeManualTwoSnakes(snake_t *snake, snake_t *secondSnake, food_t *food, desk_t *desk) {

    //inputs part-------
    int input;
    input = getch();

    int lastCoords[2] = {
        snake->snake_skeleton[snake->length].coords[0],
        snake->snake_skeleton[snake->length].coords[1], 
    };

    for (int k = snake->length; k > 0; k--) {

        snake->snake_skeleton[k].coords[0] = snake->snake_skeleton[k - 1].coords[0];
        snake->snake_skeleton[k].coords[1] = snake->snake_skeleton[k - 1].coords[1];
    }

    //CHANGE!!!!!!!!!!
    // if (input == snake->turnLeft)
    //     snakeTurnLeft(snake);
    // else if (input == snake->turnRight)
    //     snakeTurnRight(snake);

    if (input == KEY_LEFT) 
        snakeTurnLeft(snake);
    else if (input == KEY_RIGHT)
        snakeTurnRight(snake);

    //logic part--------
    snakeStep(snake);

    

    if (
        checkWallsCollisions(snake, desk)   ||
        checkItselfCollisions(snake)        ||
        checkOtherSnakeCollisions(snake, secondSnake)
    ) {

        printf("Gameover!\n");
        desk->gameOver = 1;
        //change to gameover menu
    }

    snakeEats(food, snake, desk, lastCoords[0], lastCoords[1]);
}

//CHANGED
void moveSnakeAITwoSnakes(snake_t *snake, snake_t *secondSnake, food_t *food, desk_t *desk) {

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
        checkItselfCollisions(snake)        ||
        checkOtherSnakeCollisions(snake, secondSnake)
    ) {

        printf("Gameover!\n");
        desk->gameOver = 1;
        //change to gameover menu
    }

    snakeEats(food, snake, desk, lastCoords[0], lastCoords[1]);
}

//ADDED
int checkWallsCollisions(snake_t *snake, desk_t *desk) {

    if (
        snake->snake_skeleton[0].coords[0] == desk->startX  ||
        snake->snake_skeleton[0].coords[0] == desk->endX    ||
        snake->snake_skeleton[0].coords[1] == desk->startY  ||
        snake->snake_skeleton[0].coords[1] == desk->endY
    )   return 1;
    return 0;
}

//ADDED
int checkItselfCollisions(snake_t *snake) {

    for (int k = 1; k < snake->length ; k++) {

        if (
            snake->snake_skeleton[k].coords[0] == snake->snake_skeleton[0].coords[0] &&
            snake->snake_skeleton[k].coords[1] == snake->snake_skeleton[0].coords[1] 
        )   return 1;
    }
    return 0;
}

//ADDED
int checkOtherSnakeCollisions(snake_t *snake, snake_t *secondSnake) {

    for (int k = 0; k < snake->length ; k++) {

        if (
            secondSnake->snake_skeleton[k].coords[0] == snake->snake_skeleton[0].coords[0] &&
            secondSnake->snake_skeleton[k].coords[1] == snake->snake_skeleton[0].coords[1] 
        )   return 1;
    }
    return 0;
}

int obstacleBeforeSnakeTwoSnakes(snake_t *snake, snake_t *otherSnake, desk_t *desk) {

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
    for (int k = 0; k < snake->length; k++) {

        if (
            snake->direction == UP                                                          &&
            snake->snake_skeleton[0].coords[1] == (otherSnake->snake_skeleton[k].coords[1] + 1)  &&
            snake->snake_skeleton[0].coords[0] == otherSnake->snake_skeleton[k].coords[0]    
        ) {
            ret = 1;
            return ret;
        }
        else if (
            snake->direction == DOWN                                                        &&
            snake->snake_skeleton[0].coords[1] == (otherSnake->snake_skeleton[k].coords[1] - 1)  &&
            snake->snake_skeleton[0].coords[0] == otherSnake->snake_skeleton[k].coords[0]
        ) {
            ret = 1;
            return ret;
        }
        else if (
            snake->direction == LEFT                                                        &&
            snake->snake_skeleton[0].coords[0] == (otherSnake->snake_skeleton[k].coords[0] + 1)  &&
            snake->snake_skeleton[0].coords[1] == otherSnake->snake_skeleton[k].coords[1]  
        ) {
            ret = 1;
            return ret;
        }
        else if (
            snake->direction == RIGHT                                                       &&
            snake->snake_skeleton[0].coords[0] == (otherSnake->snake_skeleton[k].coords[0] - 1)  &&
            snake->snake_skeleton[0].coords[1] == otherSnake->snake_skeleton[k].coords[1]  
        ) {
            ret = 1;
            return ret;
        }
    }
    return ret;
}

//ADDED
int snakeLeftOfSnakeTwoSnakes(snake_t *snake, snake_t *otherSnake) {

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
    for (int k = 0; k < snake->length; k++) {

        if (
            snake->direction == UP                                                          &&
            snake->snake_skeleton[0].coords[0] == (otherSnake->snake_skeleton[k].coords[0] + 1)  &&
            snake->snake_skeleton[0].coords[1] == otherSnake->snake_skeleton[k].coords[1]
        )   return 1;
        else if (
            snake->direction == DOWN                                                        &&
            snake->snake_skeleton[0].coords[0] == (otherSnake->snake_skeleton[k].coords[0] - 1)  &&
            snake->snake_skeleton[0].coords[1] == otherSnake->snake_skeleton[k].coords[1]
        )   return 1;
        else if (
            snake->direction == LEFT                                                        &&
            snake->snake_skeleton[0].coords[1] == (otherSnake->snake_skeleton[k].coords[1] - 1)  &&
            snake->snake_skeleton[0].coords[0] == otherSnake->snake_skeleton[k].coords[0]
        )   return 1;
        else if (
            snake->direction == RIGHT                                                       &&
            snake->snake_skeleton[0].coords[1] == (otherSnake->snake_skeleton[k].coords[1] + 1)  &&
            snake->snake_skeleton[0].coords[0] == otherSnake->snake_skeleton[k].coords[0]
        )   return 1;
    }
    return 0;
}

//ADDED
int snakeRightOfSnakeTwoSnakes(snake_t *snake, snake_t *otherSnake) {

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
    for (int k = 0; k < snake->length; k++) {

        if (
            snake->direction == UP                                                          &&
            snake->snake_skeleton[0].coords[0] == (otherSnake->snake_skeleton[k].coords[0] - 1)  &&
            snake->snake_skeleton[0].coords[1] == otherSnake->snake_skeleton[k].coords[1]
        )   return 1;
        else if (
            snake->direction == DOWN                                                        &&
            snake->snake_skeleton[0].coords[0] == (otherSnake->snake_skeleton[k].coords[0] + 1)  &&
            snake->snake_skeleton[0].coords[1] == otherSnake->snake_skeleton[k].coords[1]
        )   return 1;
        else if (
            snake->direction == LEFT                                                        &&
            snake->snake_skeleton[0].coords[1] == (otherSnake->snake_skeleton[k].coords[1] + 1)  &&
            snake->snake_skeleton[0].coords[0] == otherSnake->snake_skeleton[k].coords[0]
        )   return 1;
        else if (
            snake->direction == RIGHT                                                       &&
            snake->snake_skeleton[0].coords[1] == (otherSnake->snake_skeleton[k].coords[1] - 1)  &&
            snake->snake_skeleton[0].coords[0] == otherSnake->snake_skeleton[k].coords[0]
        )   return 1;
    }
    return 0;
}

void snakeControllerTwoSnakes(snake_t *snake, snake_t *otherSnake, desk_t *desk, food_t *food) {

    int dx, dy;

    dx = food->coord[0] - snake->snake_skeleton[0].coords[0];
    dy = food->coord[1] - snake->snake_skeleton[0].coords[1];

    // printf("Snake actual direction: %d\n\r", snake->direction);

    if (obstacleBeforeSnakeTwoSnakes(snake, otherSnake, desk) && obstacleLeftOfSnake(snake, desk)) {
            
        snakeTurnRight(snake);
        // printf("Obstacle front and left\n\r");
    }

    else if (obstacleBeforeSnakeTwoSnakes(snake, otherSnake, desk) && obstacleRightOfSnake(snake, desk)) {
        
        snakeTurnLeft(snake);
        // printf("Obstacle front and right\n\r");
    }

    else if (obstacleBeforeSnakeTwoSnakes(snake, otherSnake, desk)) {
     
        if (checkFieldOccupation(snake, desk) == LEFT) snakeTurnLeft(snake);
        else if (checkFieldOccupation(snake, desk) == RIGHT) snakeTurnRight(snake);
    }

    else if (snakeLeftOfSnakeTwoSnakes(snake, otherSnake))
        ;

    else if (snakeRightOfSnakeTwoSnakes(snake, otherSnake))
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