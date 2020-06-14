#include "font_types.h"
// #include "snake.h"

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


void draw_pixel(int x, int y, int color);

void draw_EndGame(unsigned short *fb1, int score, int retry, int quit);

void draw_Menu(unsigned short *fb1, int standard, int demo, int exit);

void draw_speed_ctrl(unsigned short *fb1, int slow, int medium, int fast);

void drawDesk(desk_t *desk, snake_t *snake, food_t *food, int sec, unsigned short *fb1);

void drawDesk_2_snakes(desk_t *desk, snake_t *snake, snake_t *snake2, food_t *food, int sec, unsigned short *fb1);