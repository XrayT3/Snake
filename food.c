#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "snake.h"

food_t *initFood(int coordX, int coordY) {

    food_t *food = (food_t *)malloc(sizeof(food_t));
    food->coord[0] = coordX;
    food->coord[1] = coordY;
    return food;
}

void freeFood(food_t *food) {

    free(food);
    food = NULL;
}

void updateFood(desk_t *desk, food_t *food, snake_t *snake) {

    int newX, newY;
    int maxX = desk->endX - 1, minX = desk->startX + 1;
    int maxY = desk->endY - 1, minY = desk->startY + 1;
    srand(time(NULL));

    calculate:
    newX = rand() % (maxX - minX) + minX;
    newY = rand() % (maxY - minY) + minY;

    for (int k = 0; k < snake->length; k++) {
    
        if (
            newX == snake->snake_skeleton[k].coords[0] &&
            newY == snake->snake_skeleton[k].coords[1]
        )   goto calculate;
    }

    // printf("New coords: %d %d\n", newX, newY);
    food->coord[0] = newX;
    food->coord[1] = newY;
}
