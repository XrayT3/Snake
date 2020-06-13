#define UP 1222
#define DOWN 2122
#define LEFT 2212
#define RIGHT 2221
#define INIT_LEN 2

#include <stdbool.h>
#include "font_types.h"

//Snake food data type
typedef struct {
    
    int coord[2];
} food_t;

//Snake section data type
typedef struct {

    int coords[2];
    int validity;
} section_t;

//Snake data type
typedef struct {
    bool life;
    int length;
    int max_length;
    int score;
    int direction;
    char turnLeft;
    char turnRight;
    section_t *snake_skeleton;
} snake_t;

//Game field data type
typedef struct {

    int width;
    int height;
    int startX;
    int startY;
    int endX;
    int endY;
    int gameOver;
} desk_t;

void draw_score(int score);

void draw_time(int sec);

void draw_wall(int c, int r);

void draw_food(int x, int y);

void draw_snake(int x, int y);

void draw_pixel(int x, int y, int color);

void draw_pixel_size(int x, int y, int size, int color);

void draw_EndGame(unsigned short *fb1, int score, int retry, int quit);

unsigned short * draw_Menu(unsigned short *fb1, int standard, int demo);

void draw_speed_ctrl(unsigned short *fb1, int slow, int medium, int fast);

//Initialize snake that will be controlled manually
snake_t *initSnake(int displayWidth, int displayHeight, int initSnakeX, int initSnakeY, char turnLeft, char turnRight);

//Initialize snake that will be controlled by computer
snake_t *initSnakeAI(int displayWidth, int displayHeight, int initSnakeX, int initSnakeY);

//Initialize snake food
food_t *initFood(int coordX, int coordY);

//Initialize game field
desk_t *initDesk(int width, int height, int startX, int startY);

//Delete snake when game is finished
void freeSnake(snake_t *snake);

//Delete snake food when game is finished
void freeFood(food_t *food);

//Delete game field when game is finished
void freeDesk(desk_t *desk);

//Snake makes its move per one frame according to player's commands
void moveSnakeManual(snake_t *snake, food_t *food, desk_t *desk);

//Snake makes its move per one frame according to AI's commands
void moveSnakeAI(snake_t *snake, food_t *food, desk_t *desk);

//Increase snake when it's got point
void increaseSnake(snake_t *snake, int coordX, int coordY);

//Check if snake can get a point during one move
void snakeEats(food_t *food, snake_t *snake, desk_t *desk, int lastCoordX, int lastCoordY);

//Check if snake didn't collide with walls or with itself during one move
int checkCollisions(snake_t *snake, desk_t *desk);

void drawDesk(desk_t *desk, snake_t *snake, food_t *food, int sec, unsigned short *fb1);

//Update snake food's location when snake's got a point
void updateFood(desk_t *desk, food_t *food, snake_t *snake);

//Snake makes one step forward 
void snakeStep(snake_t *snake);

//Snake's AI
void snakeController(snake_t *snake, desk_t *desk, food_t *food);

//Check if there is an obstacle before snake
int obstacleBeforeSnake(snake_t *snake, desk_t *desk);

//Check if there is an obstacle left of snake
int obstacleLeftOfSnake(snake_t *snake, desk_t *desk);

//Check if there is an obstacle right of snake
int obstacleRightOfSnake(snake_t *snake, desk_t *desk);

//Change snake's direction counterclockwise
void snakeTurnLeft(snake_t *snake);

//Change snake's direction clockwise
void snakeTurnRight(snake_t *snake);

int snakeLeftOfSnake(snake_t *snake);

int snakeRightOfSnake(snake_t *snake);

int checkFieldOccupation(snake_t *snake, desk_t *desk);