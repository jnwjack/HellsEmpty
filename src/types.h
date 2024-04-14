

#ifndef TYPES_H
#define TYPES_H


#include "engine/COI.h"

#define MAX_MOMENTUM 10
#define MAX_HEALTH 5
#define MAX_MANA 500

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

struct Actor;

typedef enum Direction {
  NONE,
  LEFT,
  RIGHT
} Direction;

typedef void (*Callback)(struct Actor* dog, COIBoard* board, void* context);

typedef struct TestContext {
  struct Actor* player;
  COISprite* square;
  int yMomentum;
  int xMomentum;
  Direction moving;
  LinkedList* actors;
  COISprite* hearts[MAX_HEALTH];
  SDL_Rect manaBar;
  IntList souls;
  int activeSoulIndex;
  unsigned int level;
} TestContext;

#endif