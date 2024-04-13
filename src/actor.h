#ifndef ACTOR_H
#define ACTOR_H

#include "engine/COI.h"

#define MAX_MOMENTUM 10

typedef enum Direction {
  NONE,
  LEFT,
  RIGHT
} Direction;

typedef struct TestContext {
  COISprite* square;
  int yMomentum;
  int xMomentum;
  Direction moving;
  LinkedList* actors;
} TestContext;

struct Actor;
typedef void (*Callback)(struct Actor* dog, COIBoard* board, void* context);

typedef struct Actor {
  COISprite* sprite;
  Callback tick;
  bool invincible;
  int yMomentum;
  int xMomentum;
  Direction moving;
  int health;
  unsigned int counter;
  int distanceToTarget[2];
} Actor;

Actor* dogCreate(COIBoard* board, int x, int y);
void dogTick(Actor* dog, COIBoard* board, void* context);

Actor* fireballCreate(COIBoard* board, int x, int y, int targetX, int targetY);
void fireballTick(Actor* fireball, COIBoard* board, void* context);

Actor* angelCreate(COIBoard* board, int x, int y);
void angelTick(Actor* angel, COIBoard* board, void* context);

#endif
