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
    int gameOver;
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
} desk_t;
