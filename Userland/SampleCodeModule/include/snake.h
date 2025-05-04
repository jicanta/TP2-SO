#ifndef SNAKE_H
#define SNAKE_H

#include <iolib.h>
#include <syscalls.h>
#include <random.h>
#include <stringutils.h>
#include <lib.h>

// Screen Dimensions
#define SIDEBAR_WIDTH  192
#define SCREEN_WIDTH 1024 - SIDEBAR_WIDTH 
#define SCREEN_HEIGHT 768

#define SIDEBAR_HEIGHT SCREEN_HEIGHT
#define SIDEBAR_MARGIN 10

// Game Dimensions
#define CELL_SIZE 16
#define MAP_WIDTH (SCREEN_WIDTH) / CELL_SIZE   
#define MAP_HEIGHT (SCREEN_HEIGHT) / CELL_SIZE 

// Colors and Text
#define SNAKE_1_COLOR 0x11C125
#define SNAKE_2_COLOR 0xFF33DE
#define BACKGROUND_COLOR 0xADD8E6
#define FOOD_COLOR 0xFF0000
#define TEXT_COLOR 0x000000
#define SIDEBAR_TEXT_COLOR 0xFFFFFFFF
#define SIDEBAR_BG_COLOR 0x000000
#define SPACE_BETWEEN 2
#define FONT_WIDTH 8

// Start values
#define INITIAL_SNAKE_LENGTH 4
#define INITIAL_SNAKE_1_X 20
#define INITIAL_SNAKE_1_Y 20
#define INITIAL_SNAKE_2_X MAP_WIDTH - INITIAL_SNAKE_1_X
#define INITIAL_SNAKE_2_Y INITIAL_SNAKE_1_Y
#define INITIAL_SNAKE_DIRECTION UP

typedef enum {
  UP, 
  LEFT, 
  DOWN, 
  RIGHT 
} Direction;

#define EASY_DIFFICULTY 80
#define MEDIUM_DIFFICULTY 50
#define HARD_DIFFICULTY 20

typedef enum {
  FALSE,
  TRUE
} Bool;

// 0 <= x <= 103 ; 0 <= y <= 95
typedef struct {
  uint8_t x;
  uint8_t y;
} Point;

typedef struct {
  Point body[MAP_WIDTH * MAP_HEIGHT];
  int length;
  int direction;
  Point lastTail;
  int alive;
  int dieAnimationFired;
} Snake;

typedef struct {
  Point position;
} Food;

uint32_t play_snake();

#endif
