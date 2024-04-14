#include "actor.h"
#include "collision.h"

#define DOG_MOMENTUM 3

Actor* fireballCreate(COIBoard* board, int x, int y, int targetX, int targetY) {
  COISprite* fireballSprite = COISpriteCreateFromAssetID(x, y, 16, 16,
                                                        COI_GLOBAL_LOADER,
                                                        ROCK,
                                                        COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COIBoardAddDynamicSprite(board, fireballSprite);
  Actor* fireball = malloc(sizeof(Actor));
  fireball->tick = fireballTick;
  fireball->sprite = fireballSprite;
  fireball->sprite->_visible = true;
  fireball->counter = 0;
  fireball->distanceToTarget[0] = targetX - x;
  fireball->distanceToTarget[1] = targetY - y;  
  if (fireball->distanceToTarget[0] == 0) {
    fireball->xMomentum = 0;
  } else {
    // int sign = fireball->distanceToTarget[0] < 0 ? -1 : 1;
    int temp = fireball->distanceToTarget[1];
    if (temp < 0) {
      temp *= -1; // Guarantee to be positive
    }
    if (temp != 0) {
      fireball->xMomentum = fireball->distanceToTarget[0] < 0 ? -1 : 1;
      fireball->xMomentum = (fireball->distanceToTarget[0] / temp);
    } else {
      fireball->xMomentum = 1;
    }
    printf("xMomentum: %i\n", fireball->xMomentum);

  }
  if (fireball->distanceToTarget[1] == 0) {
    fireball->yMomentum = 0;
  } else {
    fireball->yMomentum = fireball->distanceToTarget[1] < 0 ? -1 : 1;
  }
  fireball->sprite = fireballSprite;

  return fireball;                  
}

void fireballTick(Actor* fireball, COIBoard* board, void* context) {
  if (fireball->xMomentum < 0) {
    fireball->counter--;
  } else {
    fireball->counter++;
  }
  int yMove = 0;
  int xMove = 0;
  if (fireball->xMomentum != 0) {
    xMove = fireball->xMomentum < 0 ? -1 : 1;
  }
  if (fireball->counter == fireball->xMomentum) {
    yMove = fireball->yMomentum;
    fireball->counter = 0;
  }
  COIBoardMoveSprite(board, fireball->sprite, xMove, yMove);
  // COIBoardMoveSprite(board, fireball->sprite, fireball->xMomentum, fireball->yMomentum);
  if (!fireball->sprite->_visible) {
    // Out of frame, delete fireball
    TestContext* tc = (TestContext*)context;
    LinkedListRemove(tc->actors, fireball);
    actorDestroy(fireball, board);
  }
}

Actor* angelCreate(COIBoard* board, int x, int y) {
  COISprite* angelSprite = COISpriteCreateFromAssetID(x, y, 32, 32,
                                                      COI_GLOBAL_LOADER,
                                                      DOG,
                                                      COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COIBoardAddDynamicSprite(board, angelSprite);
  Actor* angel = malloc(sizeof(Actor));
  angel->moving = NONE;
  angel->tick = angelTick;
  angel->counter = 0;
  angel->sprite = angelSprite;

  return angel;                                                
}

void angelTick(Actor* angel, COIBoard* board, void* context) {
  TestContext* tc = (TestContext*)context;

  angel->counter++;
  if (angel->counter > 100) {
    LinkedListAdd(tc->actors, (void*)fireballCreate(board, angel->sprite->_x, angel->sprite->_y, 
                                                    tc->square->_x + tc->square->_width / 2,
                                                    tc->square->_y + tc->square->_height / 2));
    angel->counter = 0;
  }
}

Actor* dogCreate(COIBoard* board, int x, int y) {
  COISprite* dogSprite = COISpriteCreateFromAssetID(x, y, 32, 32,
                                                    COI_GLOBAL_LOADER,
                                                    DOG,
                                                    COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  
  COIBoardAddDynamicSprite(board, dogSprite);
  Actor* dog = malloc(sizeof(Actor));
  dog->moving = LEFT;
  dog->xMomentum = DOG_MOMENTUM;
  dog->tick = dogTick;
  dog->yMomentum = 5;
  dog->sprite = dogSprite;

  return dog;
}
void dogTick(Actor* dog, COIBoard* board, void* context) {
  dog->yMomentum = MIN(MAX_MOMENTUM, dog->yMomentum + 1);

  if (dog->yMomentum > 0) {
    int effectiveYMomentum = dog->yMomentum; 
    for (int y = 0; y <= dog->yMomentum; y++) {
      unsigned int collisionResult = collisionCheckOnTopOf(dog->sprite, board, 0, y);
      if (collisionResult & CORNER_COLLIDE_BL || collisionResult & CORNER_COLLIDE_BR) {
        effectiveYMomentum = y;
        break;
      }
    }

    if (effectiveYMomentum > 0) {
      COIBoardMoveSprite(board, dog->sprite, 0, effectiveYMomentum);
    }
  } else if (dog->yMomentum < 0) {
    COIBoardMoveSprite(board, dog->sprite, 0, dog->yMomentum);
  }

  if (dog->xMomentum > 0) {
    int effectiveXMomentum = dog->xMomentum;
    for (int x = 0; x <= dog->xMomentum; x++) {
      unsigned int collisionResult = collisionCheckLeftOf(dog->sprite, board, x, 0);
      if (collisionResult & CORNER_COLLIDE_BR || collisionResult & CORNER_COLLIDE_TR) {
        effectiveXMomentum = x;
        dog->xMomentum = -1 * DOG_MOMENTUM;
        break;
      }
    }

    if (effectiveXMomentum > 0) {
      COIBoardMoveSprite(board, dog->sprite, effectiveXMomentum, 0);
    }
  } else if (dog->xMomentum < 0) {
    // printf("neg. momentum\n");
    int effectiveXMomentum = dog->xMomentum;
    for (int x = 0; x >= dog->xMomentum; x--) {
      unsigned int collisionResult = collisionCheckRightOf(dog->sprite, board, x, 0);
      if (collisionResult & CORNER_COLLIDE_BL || collisionResult & CORNER_COLLIDE_TL) {
        effectiveXMomentum = x;
        dog->xMomentum = DOG_MOMENTUM;
        break;
      }
    }

    if (effectiveXMomentum < 0) {
      COIBoardMoveSprite(board, dog->sprite, effectiveXMomentum, 0);
    }
  }
}

void actorDestroy(Actor* actor, COIBoard* board) {
  COIBoardRemoveDynamicSprite(board, actor->sprite);
  COISpriteDestroy(actor->sprite);
  free(actor);
}