
#define UP 1222
#define DOWN 2122
#define LEFT 2212
#define RIGHT 2221
#define INIT_LEN 2

#include "font_types.h"
#include "menu.h"

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

void moveSnakeAITwoSnakes(snake_t *snake, snake_t *secondSnake, food_t *food, desk_t *desk);

//Snake makes its move per one frame according to AI's commands
void moveSnakeAI(snake_t *snake, food_t *food, desk_t *desk);

//Increase snake when it's got point
void increaseSnake(snake_t *snake, int coordX, int coordY);

//Check if snake can get a point during one move
void snakeEats(food_t *food, snake_t *snake, desk_t *desk, int lastCoordX, int lastCoordY);

int checkWallsCollisions(snake_t *snake, desk_t *desk);

int checkItselfCollisions(snake_t *snake);

int checkOtherSnakeCollisions(snake_t *snake, snake_t *secondSnake);

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

int obstacleBeforeSnakeTwoSnakes(snake_t *snake, snake_t *otherSnake, desk_t *desk);

int snakeLeftOfSnakeTwoSnakes(snake_t *snake, snake_t *otherSnake);

int snakeRightOfSnakeTwoSnakes(snake_t *snake, snake_t *otherSnake);

void snakeControllerTwoSnakes(snake_t *snake, snake_t *otherSnake, desk_t *desk, food_t *food);

int get_record(int a, int b, int c, int d);