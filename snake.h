#define UP 1222
#define DOWN 2122
#define LEFT 2212
#define RIGHT 2221
#define INIT_LEN 2

#include <stdbool.h>

#include "font_types.h"

typedef struct {
    
    int coord[2];
} food_t;

typedef struct {

    int coords[2];
    int validity;
} section_t;

typedef struct {
    
    int length;
    int max_length;
    int score;
    int direction;
    bool life;
    section_t *snake_skeleton;
} snake_t;

typedef struct {

    int width;
    int height;
    int startX;
    int startY;
    int endX;
    int endY;
} desk_t;

snake_t initSnake(int displayWidth, int displayHeight, int initSnakeX, int initSnakeY);

food_t initFood(int coordX, int coordY);

void draw_pixel(int x, int y, int color);

void draw_pixel_size(int x, int y, int size);

void draw_EndGame(unsigned short *fb1);

void moveSnakeManual(snake_t *snake, food_t *food, desk_t *desk);

void moveSnakeAI(snake_t *snake, food_t *food, desk_t *desk);

void increaseSnake(snake_t *snake, int coordX, int coordY);

void snakeEats(food_t *food, snake_t *snake, desk_t *desk, int lastCoordX, int lastCoordY);

int checkCollisions(snake_t *snake, desk_t *desk);

desk_t initDesk(int width, int height, int startX, int startY);

void drawDesk(desk_t *desk, snake_t *snake, food_t *food, int sec, unsigned short *fb1);

void updateFood(desk_t *desk, food_t *food);

void snakeStep(snake_t *snake);

void snakeController(snake_t *snake, desk_t *desk, food_t *food);

int obstacleBeforeSnake(snake_t *snake, desk_t *desk);

int obstacleLeftOfSnake(snake_t *snake, desk_t *desk);

int obstacleRightOfSnake(snake_t *snake, desk_t *desk);

void snakeTurnLeft(snake_t *snake);

void snakeTurnRight(snake_t *snake);