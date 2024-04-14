#ifndef ACTOR_H
#define ACTOR_H

#include "engine/COI.h"

#define MAX_MOMENTUM 10
#define MAX_HEALTH 5
#define MAX_MANA 500

struct Actor;

typedef enum Direction {
  NONE,
  LEFT,
  RIGHT
} Direction;

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
} TestContext;

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
  double angle;
  int distanceToTarget[2];
  int mana;
  unsigned int timeLeft; // When 0, summoned actor disappears
} Actor;

Actor* dogCreate(COIBoard* board, int x, int y);
void dogTick(Actor* dog, COIBoard* board, void* context);

Actor* fireballCreate(COIBoard* board, int x, int y, int targetX, int targetY);
void fireballTick(Actor* fireball, COIBoard* board, void* context);

Actor* angelCreate(COIBoard* board, int x, int y);
void angelTick(Actor* angel, COIBoard* board, void* context);

Actor* playerCreate(COIBoard* board, int x, int y);
void playerTick(Actor* player, COIBoard* board, void* context);

Actor* rockCreate(COIBoard* board, int x, int y);
void rockTick(Actor* rock, COIBoard* board, void* context);

void actorDestroy(Actor* actor, COIBoard* board);

#endif
