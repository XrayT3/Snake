
#include <stdio.h>
#include <stdlib.h>
// #include <unistd.h> 

#include "snake.h"
#include "gui.h"

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "font_types.h"

unsigned short *fb;
unsigned char *parlcd_mem_base;
unsigned char *mem_base;
uint32_t rgb_knobs_value;

int size_Cell = 20;
int size_GameOver = 5;
int size_retry = 4;
int size_quit = 4;
int size_standard = 5;
int size_demo = 5;
int size_speed = 5;
int size_exit = 4;

void draw_pixel(int x, int y, int color) {
  if (x>=0 && x<480 && y>=0 && y<320) {
    fb[x+480*y] = color;
  }
}

void draw_EndGame(unsigned short *fb1, int score, int retry, int quit){
  int ptr;
  fb = fb1;
  for (ptr = 0; ptr < 320*480 ; ptr++) {
        fb[ptr]=0u;
  }
  char str[] = "Game over"; // 9
  char *ch = str;
  int x = 54;
  for (int i=0; i<9; i++) {
      draw_char(x, 10, *ch, size_GameOver, 63519);
      x+=size_GameOver*char_width(*ch)+2;
      ch++;
  }
  x = 220;
  if (score==0){
      draw_char(x, 100, '0', 4, 63519); // size_score = 4;
  }
  char str1[3] = "0";
  int idx = 0;
  while (score!=0)
  {
      str1[idx] = score % 10 + '0';
      score /= 10;
      idx++;
  }
  for (int i = idx-1; i >= 0; i--){
      draw_char(x, 100, str1[i], 4, 63519); // size_score = 4;
      x+=4*char_width(str1[i])+2; // size_score = 4;
  }
  char RETRY[] = "RETRY"; // 5
  char *RE = RETRY;
  x = 142-(24*retry);
  for (int i=0; i<5; i++) {
      draw_char(x, 180, *RE, retry+size_retry, 63519+(2016*retry));
      x+=(size_retry+retry)*char_width(*RE)+2;
      RE++;
  }
  char QUIT[] = "QUIT"; // 4
  char *QU = QUIT;
  x = 173-(13*quit);
  for (int i=0; i<4; i++) {
      draw_char(x, 250, *QU, quit+size_quit, 63519+(2016*quit));
      x+=(size_quit+quit)*char_width(*QU)+2;
      QU++;
  }

  // draw LCD
    parlcd_write_cmd(parlcd_mem_base, 0x2c);
    for (ptr = 0; ptr < 480*320 ; ptr++) {
        parlcd_write_data(parlcd_mem_base, fb[ptr]);
    }
}

void draw_Menu(unsigned short *fb1, int standard, int demo, int exit){
    parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
    int ptr;
    fb = fb1;
    for (ptr = 0; ptr < 320*480 ; ptr++) {
        fb[ptr]=0u;
    }
    char str[] = "STANDARD"; // 8
    char *ch = str;
    int x = 20;
    // int x = 49-(standard*36);
    for (int i=0; i<8; i++) {
        draw_char(x, 20, *ch, size_standard+standard, 63519+(2016*standard));
        x+=(size_standard+standard)*char_width(*ch)+2;
        ch++;
    }
    char str1[] = "DEMO"; // 4
    char *ch1 = str1;
    x = 20;
    // x = 134-(23*demo);
    for (int i=0; i<4; i++) {
        draw_char(x, 130, *ch1, size_demo+demo, 63519+(2016*demo));
        x+=(size_demo+demo)*char_width(*ch1)+2;
        ch1++;
    }
    char str2[] = "EXIT"; // 4
    char *ch2 = str2;
    x = 20;
    // x = 177-(15*exit);
    for (int i=0; i<4; i++) {
        draw_char(x, 240, *ch2, size_exit+exit, 63519+(2016*exit));
        x+=(size_exit+exit)*char_width(*ch2)+2;
        ch2++;
    }
    // draw LCD
    parlcd_write_cmd(parlcd_mem_base, 0x2c);
    for (ptr = 0; ptr < 480*320 ; ptr++) {
        parlcd_write_data(parlcd_mem_base, fb[ptr]);
    }
}

void draw_speed_ctrl(unsigned short *fb1, int slow, int medium, int fast){
    int ptr;
    fb = fb1;
    for (ptr = 0; ptr < 320*480 ; ptr++) {
            fb[ptr]=0u;
    }
    char str[] = "Set speed"; // 9
    char *ch = str;
    int x = 49;
    for (int i=0; i<9; i++) {
        draw_char(x, 20, *ch, size_speed, 63519);
        x+=size_speed*char_width(*ch)+2;
        ch++;
    }
    char str1[] = "Slow"; // 4
    char *ch1 = str1;
    x = 49;
    for (int i=0; i<4; i++) {
        draw_char(x, 130, *ch1, 4, 63519+(2016*slow));
        x+=4*char_width(*ch1)+2;
        ch1++;
    }
    char str2[] = "Medium"; // 6
    char *ch2 = str2;
    x = 49;
    for (int i=0; i<6; i++) {
        draw_char(x, 190, *ch2, 4, 63519+(2016*medium));
        x+=4*char_width(*ch2)+2;
        ch2++;
    }
    char str3[] = "Fast"; // 4
    char *ch3 = str3;
    x = 49;
    for (int i=0; i<4; i++) {
        draw_char(x, 250, *ch3, 4, 63519+(2016*fast));
        x+=4*char_width(*ch3)+2;
        ch3++;
    }
    // draw LCD
    parlcd_write_cmd(parlcd_mem_base, 0x2c);
    for (ptr = 0; ptr < 480*320 ; ptr++) {
        parlcd_write_data(parlcd_mem_base, fb[ptr]);
    }

    // LED
    mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
    rgb_knobs_value = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
    rgb_knobs_value = 16711935; //pink

    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = rgb_knobs_value;
    *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o) = rgb_knobs_value;
}

void drawDesk(desk_t *desk, snake_t *snake, food_t *food, int sec, unsigned short *fb1) {
    int ptr;
    fb = fb1;
    for (ptr = 0; ptr < 480*320 ; ptr++) {
        fb[ptr] = 0;
    }
    draw_score(snake->score);
    draw_time(sec);
    draw_wall(16, 14);

    for (int i = desk->startY; i < desk->endY; i ++) {
        for (int j = desk->startX; j < desk->endX; j++) {
            if (
                (j == food->coord[0]) &&
                (i == food->coord[1])
            )
            draw_food(j*size_Cell, i*size_Cell); // food
            else{
                for (int k = 0; k < snake->length; k++) {

                    if (
                        j == (snake->snake_skeleton[k].coords[0]) &&
                        i == (snake->snake_skeleton[k].coords[1]) 
                    )
                    draw_snake(j*size_Cell, i*size_Cell); // snake
                }
            }
        }
    }

    // draw LCD
    parlcd_write_cmd(parlcd_mem_base, 0x2c);
    for (ptr = 0; ptr < 480*320 ; ptr++) {
        parlcd_write_data(parlcd_mem_base, fb[ptr]);
    }
}

void drawDesk_2_snakes(desk_t *desk, snake_t *snake, snake_t *snake2, food_t *food, int sec, unsigned short *fb1) {
    int ptr;
    fb = fb1;
    for (ptr = 0; ptr < 480*320 ; ptr++) {
        fb[ptr] = 0;
    }
    draw_score(snake->score);
    draw_score_2_snakes(snake2->score);
    draw_time(sec);
    draw_wall(16, 14); 
    for (int i = desk->startY; i < desk->endY; i ++) {
        for (int j = desk->startX; j < desk->endX; j++) {
            if (
                (j == food->coord[0]) &&
                (i == food->coord[1])
            )
            draw_food(j*size_Cell, i*size_Cell); // food
            else{
                for (int k = 0; k < snake->length; k++) {
                    if (
                        j == (snake->snake_skeleton[k].coords[0]) &&
                        i == (snake->snake_skeleton[k].coords[1]) 
                    )
                    draw_snake(j*size_Cell, i*size_Cell); // snake
                }
                for (int k = 0; k < snake2->length; k++) {
                    if (
                        j == (snake2->snake_skeleton[k].coords[0]) &&
                        i == (snake2->snake_skeleton[k].coords[1]) 
                    )
                    draw_snake(j*size_Cell, i*size_Cell); // snake2
                }
            }
        }
    }

    // draw LCD
    parlcd_write_cmd(parlcd_mem_base, 0x2c);
    for (ptr = 0; ptr < 480*320 ; ptr++) {
        parlcd_write_data(parlcd_mem_base, fb[ptr]);
    }
}
