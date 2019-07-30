
/* Global object for FT801 Implementation */

typedef struct Position {
  uint16_t x;
  uint16_t y;
} Position;

typedef struct {
  uint8_t tag;
  double xValue;
  double yValue;
} Key;


class Drawing {  
  public:
    static void drawGrid(Key keys[5]);
    static Key selectedKey(Position position);
};
