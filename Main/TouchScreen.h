
#ifndef TouchScreen_h
#define TouchScreen_h

class TouchScreen {
  public:
    struct Position;
    struct Key;
    Key selectedKey(Position position);

};

#endif
