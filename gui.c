#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <termios.h>

#include "snake.h"

#include "font_types.h"
#include "font_prop14x16.c"

font_descriptor_t* fdes = &font_winFreeSystem14x16;

int size_cell = 20;
int size_score = 4;
int size_time = 4;
int size_GameOver = 5;
int size_retry = 4;
int size_quit = 4;
int size_standard = 5;
int size_demo = 5;
int size_speed = 5;
int size_exit = 4;

void draw_pixel_size(int x, int y, int size, int color) {
  int i, j;
  for (i = 0; i < size; i++){
    for (j = 0; j < size; j++){
      draw_pixel(x-i, y-j, color);
    }
  }
}

void draw_wall(int c, int r) {
  // up
  for (int i = 0; i < c+1; i++){
    for (int x = 0; x < size_cell; x++){
      for(int y = size_cell/2; y < size_cell; y++){
        draw_pixel(i*size_cell+x+(size_cell/4), 0+y, 65535);
      }
    }
  }
  // down
  for (int i = 0; i < c+1; i++){
    for (int x = 0; x < size_cell; x++){
      for(int y = size_cell/2; y < size_cell; y++){
        draw_pixel(i*size_cell+x+(size_cell/4), 300+y-(size_cell/2), 65535);
      }
    }
  }
  // left
  for (int i = 1; i < r+1; i++){
    for (int x = size_cell/2; x < size_cell; x++){
      for(int y = 0; y < size_cell; y++){
        draw_pixel(0+x, i*size_cell+y+(size_cell/4), 65535);
      }
    }
  }
  // right
  for (int i = 1; i < r+1; i++){
    for (int x = size_cell/2; x < size_cell; x++){
      for(int y = 0; y < size_cell; y++){
        draw_pixel(340+x-(size_cell/2), i*size_cell+y+(size_cell/4), 65535);
      }
    }
  }
}

void draw_food(int x, int y) {
  int i, j;
  for (i = 0; i < size_cell; i++){
    for (j = 0; j < size_cell; j++){
      draw_pixel(x+i, y+j, 2016);
    }
  }
}

void draw_snake(int x, int y) {
  int i, j;
  for (i = 0; i < size_cell; i++){
    for (j = 0; j < size_cell; j++){
      draw_pixel(x+i, y+j, 31);
    }
  }
}

int char_width(font_descriptor_t* fdes, int ch) {
  int width = 0;
  if ((ch >= fdes->firstchar) && (ch-fdes->firstchar < fdes->size)) {
    ch -= fdes->firstchar;
    if (!fdes->width) {
      width = fdes->maxwidth;
    } else {
      width = fdes->width[ch];
    }
  }
  return width;
}

void draw_char(int x, int y, font_descriptor_t* fdes, char ch, int size, int color) {
  int w = char_width(fdes, ch);
  if (w > 0) {
    const font_bits_t *ptr;
    if (fdes->offset) {
      ptr = &fdes->bits[fdes->offset[ch-fdes->firstchar]];
      ptr = fdes->bits + fdes->offset[ch-fdes->firstchar];
    } else {
      int bw = (fdes->maxwidth+15)/16;
      ptr = fdes->bits + (ch-fdes->firstchar)*bw*fdes->height;
    }
    // printf("Znak %c na %i, %i, sirka %i\n", ch, x, y, w);
    int i, j;
    for (i = 0; i < fdes->height; i++){
      font_bits_t val = *ptr;
      for (j = 0; j < w; j++){
        if ((val&0x8000) != 0) {
          draw_pixel_size(x+size*j, y+size*i, size, color);
        }
        val<<=1;
      }
      ptr++;
    }
  }
}

void draw_score(int score){
    int y = 20;
    int x = 380;
    if (score==0){
        draw_char(x, y, fdes, '0', size_score+1, 63519);
        return;
    }
    char str[3] = "0";
    int idx = 0;
    while (score!=0)
    {
        str[idx] = score % 10 + '0';
        score /= 10;
        idx++;
    }
    for (int i = idx-1; i >= 0; i--){
        draw_char(x, y, fdes, str[i], size_score+1, 63519);
        x+=size_score*char_width(fdes, str[i])+2;
    }
}

void draw_score_2_snakes(int score){
    int y = 240;
    int x = 380;
    if (score==0){
        draw_char(x, y, fdes, '0', size_score+1, 63519);
        return;
    }
    char str[3] = "0";
    int idx = 0;
    while (score!=0)
    {
        str[idx] = score % 10 + '0';
        score /= 10;
        idx++;
    }
    for (int i = idx-1; i >= 0; i--){
        draw_char(x, y, fdes, str[i], size_score+1, 63519);
        x+=size_score*char_width(fdes, str[i])+2;
    }
}

void draw_time(int sec){
  int y = 127;
  int x = 380;
  if (sec==0){
      draw_char(x, y, fdes, '0', size_time, 63519);
      return;
  }
  char str[3] = "0";
  int idx = 0;
  while (sec!=0)
  {
      str[idx] = sec % 10 + '0';
      sec /= 10;
      idx++;
  }
  for (int i = idx-1; i >= 0; i--){
      draw_char(x, y, fdes, str[i], size_time, 63519);
      x+=size_time*char_width(fdes, str[i])+2;
  }
}
