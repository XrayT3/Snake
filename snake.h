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

snake_t initSnake(int displayWidth, int displayHeight, int initialSnakeLength, int initSnakeX, int initSnakeY);

food_t initFood(int coordX, int coordY);

void drawSnake(int displayWidth, int displayHeight, snake_t *snake, int sec);

void moveSnake(snake_t *snake, food_t *food, desk_t *desk);

void increaseSnake(snake_t *snake, int coordX, int coordY);

void snakeEats(food_t *food, snake_t *snake, desk_t *desk, int lastCoordX, int lastCoordY);

int checkCollisions(snake_t *snake);

desk_t initDesk(int width, int height, int startX, int startY);

void drawDesk(desk_t *desk, snake_t *snake, food_t *food);

void updateFood(desk_t *desk, food_t *food);