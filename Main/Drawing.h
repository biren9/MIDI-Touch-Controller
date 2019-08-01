#ifndef Drawing_h
#define Drawing_h

#include <stdint.h>
/* Global object for FT801 Implementation */

typedef struct Position {
  uint16_t x;
  uint16_t y;
} Position;

typedef struct {
  uint8_t tag;
  uint8_t line;
  uint8_t row;
  int oktave;
  double xValue;
  double yValue;
} Key;


typedef struct {
  uint8_t oktave;
} MenuInformation;


class Drawing {
  public:
    static void drawGrid(Key keys[5], MenuInformation information);
    static Key Drawing::selectedKey(Position position, int oktave);
};

#endif
