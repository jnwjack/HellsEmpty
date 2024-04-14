#include "actor.h"
#include "collision.h"
#include <math.h>

#define DOG_MOMENTUM 3
#define INVINCIBILITY_DURATION 100
#define LIFETIME 400

Actor* playerCreate(COIBoard* board, int x, int y) {
  COISprite* playerSprite = COISpriteCreateFromAssetID(x, y, 32, 32, COI_GLOBAL_LOADER, 0, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COIBoardAddDynamicSprite(board, playerSprite);

  Actor* player = malloc(sizeof(Actor));
  player->yMomentum = 5;
  player->xMomentum = 0;
  player->moving = NONE;
  player->tick = playerTick;
  player->sprite = playerSprite;
  player->health = MAX_HEALTH;
  player->mana = MAX_MANA;
  player->invincible = false;

  return player;
}

void playerTick(Actor* player, COIBoard* board, void* context) {
  TestContext* tc = (TestContext*)context;

  // Invincibility
  if (player->invincible) {
    player->counter++;
    if (player->counter > INVINCIBILITY_DURATION) {
      player->invincible = false;
      player->counter = 0;
      player->sprite->_autoHandle = true;
    }
    if (player->counter % 5 == 0) {
      player->sprite->_visible = !player->sprite->_visible;
    }
  }

  // Restore mana
  player->mana = MIN(MAX_MANA, player->mana + 1);


  if (player->moving == LEFT) {
    player->xMomentum = MAX(MAX_MOMENTUM * -1, player->xMomentum - 1);
  } else if (player->moving == RIGHT) {
    player->xMomentum = MIN(MAX_MOMENTUM, player->xMomentum + 1);
  } else if (player->moving == NONE && player->xMomentum != 0) {
    if (player->xMomentum > 0) {
      player->xMomentum--;
    } else {
      player->xMomentum++;
    }
  }

  // Apply gravity
  player->yMomentum = MIN(MAX_MOMENTUM, player->yMomentum + 1);

  if (player->yMomentum > 0) {
    int effectiveYMomentum = player->yMomentum; 
    for (int y = 0; y <= player->yMomentum; y++) {
      unsigned int collisionResult = collisionCheckOnTopOf(player->sprite, board, 0, y);
      if (collisionResult & CORNER_COLLIDE_BL || collisionResult & CORNER_COLLIDE_BR) {
        effectiveYMomentum = y;
        break;
      }
    }

    if (effectiveYMomentum > 0) {
      COIBoardMoveSprite(board, player->sprite, 0, effectiveYMomentum);
    }
  } else if (player->yMomentum < 0) {
    COIBoardMoveSprite(board, player->sprite, 0, player->yMomentum);
  }

  if (player->xMomentum > 0) {
    int effectiveXMomentum = player->xMomentum;
    for (int x = 0; x <= player->xMomentum; x++) {
      unsigned int collisionResult = collisionCheckLeftOf(player->sprite, board, x, 0);
      if (collisionResult & CORNER_COLLIDE_BR || collisionResult & CORNER_COLLIDE_TR) {
        effectiveXMomentum = x;
        break;
      }
    }

    if (effectiveXMomentum > 0) {
      COIBoardMoveSprite(board, player->sprite, effectiveXMomentum, 0);
    }

    player->xMomentum = effectiveXMomentum;
  } else if (player->xMomentum < 0) {
    // printf("neg. momentum\n");
    int effectiveXMomentum = player->xMomentum;
    for (int x = 0; x >= player->xMomentum; x--) {
      unsigned int collisionResult = collisionCheckRightOf(player->sprite, board, x, 0);
      if (collisionResult & CORNER_COLLIDE_BL || collisionResult & CORNER_COLLIDE_TL) {
        effectiveXMomentum = x;
        break;
      }
    }

    if (effectiveXMomentum < 0) {
      COIBoardMoveSprite(board, player->sprite, effectiveXMomentum, 0);
    }

    player->xMomentum = effectiveXMomentum;
  }
}

Actor* rockCreate(COIBoard* board, int x, int y) {
  COISprite* rockSprite = COISpriteCreateFromAssetID(x, y, 32, 32,
                                            COI_GLOBAL_LOADER,
                                            ROCK,
                                            COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COIBoardAddDynamicSprite(board, rockSprite);
  Actor* rock = malloc(sizeof(Actor));
  rock->tick = rockTick;
  rock->sprite = rockSprite;
  rock->timeLeft = LIFETIME;

  return rock;
}

void rockTick(Actor* rock, COIBoard* board, void* context) {
  TestContext* tc = (TestContext*)context;

  // Do we have this soul type?
  if (tc->souls.length > 0) {
    rock->timeLeft--;
    if (rock->timeLeft == 0) {
      LinkedListRemove(tc->actors, rock);
      actorDestroy(rock, board);
    }
  } else if (collisionWithPlayer(rock->sprite, tc->player->sprite)) {
    printf("got rock soul\n");
    IntListAdd(&tc->souls, ROCK);
    tc->activeSoulIndex = 0;

    LinkedListRemove(tc->actors, rock);
    actorDestroy(rock, board);
  }
}

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

  fireball->xMomentum = fireball->distanceToTarget[0] < 0 ? -1 : 1;
  fireball->yMomentum = fireball->distanceToTarget[1] < 0 ? -1 : 1;

  // fireball->angle = atan2((double)abs(fireball->distanceToTarget[0]), (double)abs(fireball->distanceToTarget[1]));
  fireball->angle = atan2((double)fireball->distanceToTarget[1], (double)fireball->distanceToTarget[0]);


  fireball->sprite = fireballSprite;

  return fireball;                  
}

void fireballTick(Actor* fireball, COIBoard* board, void* context) {
  int newX = fireball->sprite->_x + (5 * cos(fireball->angle)) * 1;
  int newY = fireball->sprite->_y + (5 * sin(fireball->angle)) * 1;
  COIBoardMoveSprite(board, fireball->sprite, newX - fireball->sprite->_x, newY - fireball->sprite->_y);

  TestContext* tc = (TestContext*)context;
  if (!fireball->sprite->_visible) {
    // Out of frame, delete fireball
    LinkedListRemove(tc->actors, fireball);
    actorDestroy(fireball, board);
  } else if (!tc->player->invincible && collisionWithPlayer(fireball->sprite, tc->player->sprite)) {
    tc->player->health = MAX(0, tc->player->health - 1);
    tc->player->invincible = true;
    tc->player->sprite->_autoHandle = false;
    tc->hearts[tc->player->health]->_visible = false;

    LinkedListRemove(tc->actors, fireball);
    actorDestroy(fireball, board);
  }
}

Actor* angelCreate(COIBoard* board, int x, int y) {
  COISprite* angelSprite = COISpriteCreateFromAssetID(x, y, 32, 32,
                                                      COI_GLOBAL_LOADER,
                                                      ANGEL,
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
                                                    tc->player->sprite->_x + tc->player->sprite->_width / 2,
                                                    tc->player->sprite->_y + tc->player->sprite->_height / 2));
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
  dog->timeLeft = LIFETIME;

  return dog;
}
void dogTick(Actor* dog, COIBoard* board, void* context) {
  TestContext* tc = (TestContext*)context;

  dog->timeLeft--;
  if (dog->timeLeft == 0) {
    LinkedListRemove(tc->actors, dog);
    actorDestroy(dog, board);
    return;
  }

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

  // Collision with enemies
  LinkedListNode* oldNode = tc->actors->cursor; // Save place, reset when done
  LinkedListResetCursor(tc->actors);
  Actor* currentActor = LinkedListNext(tc->actors);
  while (currentActor) {
    if (currentActor->sprite->_assetID == ANGEL && collisionWithPlayer(dog->sprite, currentActor->sprite) != 0) {
      LinkedListRemove(tc->actors, currentActor);
      actorDestroy(currentActor, board);
    }
    currentActor = LinkedListNext(tc->actors);
  }
  tc->actors->cursor = oldNode;
}

void actorDestroy(Actor* actor, COIBoard* board) {
  COIBoardRemoveDynamicSprite(board, actor->sprite);
  COISpriteDestroy(actor->sprite);
  free(actor);
  COIBoardQueueDraw(board);
}