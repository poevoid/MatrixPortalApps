#pragma once
#include "arrays.h"
//----------------RGB Matrix settings------------------//
#define HEIGHT 32   // Matrix height (pixels) - SET TO 64 FOR 64x64 MATRIX!
#define WIDTH 64    // Matrix width (pixels)
#define MAX_FPS 40  // Maximum redraw rate, frames/second
// Color definitions
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define AMBER 0x9465
#define WHITE    0xFFFF

#define BACK_BUTTON 2
#define NEXT_BUTTON 3
#if defined(_VARIANT_MATRIXPORTAL_M4_)  // MatrixPortal M4
uint8_t rgbPins[] = { 7, 8, 9, 10, 11, 12 };
uint8_t addrPins[] = { 17, 18, 19, 20, 21 };
uint8_t clockPin = 14;
uint8_t latchPin = 15;
uint8_t oePin = 16;
#else  // MatrixPortal ESP32-S3
uint8_t rgbPins[] = { 42, 41, 40, 38, 39, 37 };
uint8_t addrPins[] = { 35, 36, 48, 45, 21 };
uint8_t clockPin = 2;
uint8_t latchPin = 47;
uint8_t oePin = 14;
#endif

#define NUM_ADDR_PINS 4
//------------------------Game & control parameters---------------//
#define N_COLORS 1
#define BOX_HEIGHT 7
#define BOX_WIDTH 1
#define BALL_SIZE 2
#define CLICKTHRESHHOLD 30

struct Ball {
  int8_t x;
  int8_t y;
  bool right;
  bool down;
  bool up;
  uint16_t color;
};

struct Player {
  uint8_t x;
  uint8_t y;
  uint8_t score;
};

struct Opponent {
  uint8_t x;
  uint8_t y;
  uint8_t score;
  
};
uint32_t prevTime = 0;  // Used for frames-per-second throttle
uint8_t buzzer = A4;
uint8_t gamestate = 0;

uint8_t legth = 16;  // leg length
uint8_t zeroLength = 60;
uint8_t zeroX = 2;
uint8_t zeroY = 16;

int counter = 0;

enum class Application : uint8_t {
  Pong,
  Environs,
  Cistercia
};
enum class CurrentScreen : uint8_t {
  Title,
  Game,
  Gameover,
  Win
};

Ball ball = {WIDTH / 2 - BALL_SIZE, HEIGHT / 2 - BALL_SIZE, true, true, false, MAGENTA };
CurrentScreen screen = { CurrentScreen::Title };
Player player {0, 0, 0};
Opponent oppo {WIDTH-BOX_WIDTH, 0, 0 };
Application app = {Application::Pong};
