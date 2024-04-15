#ifndef ACTOR_H
#define ACTOR_H

#include "engine/COI.h"
#include "types.h"

typedef struct Actor {
  COISprite* sprite;
  Callback tick;
  bool invincible;
  int yMomentum;
  int xMomentum;
  Direction moving;
  int health;
  unsigned int counter;
  unsigned int spriteSheetCounter;
  double angle;
  int distanceToTarget[2];
  int mana;
  unsigned int timeLeft; // When 0, summoned actor disappears
  bool jumping;
  int sheetIndexY;
  int sheetIndexX;
} Actor;


Actor* dogCreate(COIBoard* board, int x, int y, bool standingStill);
void dogTick(Actor* dog, COIBoard* board, void* context);

Actor* fireballCreate(COIBoard* board, int x, int y, int targetX, int targetY);
void fireballTick(Actor* fireball, COIBoard* board, void* context);

Actor* angelCreate(COIBoard* board, int x, int y);
void angelTick(Actor* angel, COIBoard* board, void* context);

#define PLAYER_WALK_TICKS 8 // Ticks per walk frame
Actor* playerCreate(COIBoard* board, int x, int y);
void playerTick(Actor* player, COIBoard* board, void* context);

Actor* rockCreate(COIBoard* board, int x, int y);
void rockTick(Actor* rock, COIBoard* board, void* context);

void actorDestroy(Actor* actor, COIBoard* board);

#endif
