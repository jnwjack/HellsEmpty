#include "actor.h"
#include "collision.h"
#include "levels.h"
#include <math.h>

#define DOG_MOMENTUM 3
#define INVINCIBILITY_DURATION 100
#define LIFETIME 400

Actor* playerCreate(COIBoard* board, int x, int y) {
  COISprite* playerSprite = COISpriteCreateFromAssetID(x, y, 32, 32, COI_GLOBAL_LOADER, 0, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COISpriteSetSheetIndex(playerSprite, 0, 0);
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
  player->timeLeft = LIFETIME;
  player->jumping = false;
  player->sheetIndexX = 0;
  player->sheetIndexY = 0;
  player->spriteSheetCounter = 0;

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
  if (player->mana < MAX_MANA) {
    player->mana++;
    COIBoardQueueDraw(board);
  }


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
        player->jumping = false;
        break;
      }
    }

    if (effectiveYMomentum > 0) {
      COIBoardMoveSprite(board, player->sprite, 0, effectiveYMomentum);
    } else {
      player->jumping = false;
    }

    player->yMomentum = effectiveYMomentum;
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

  // Update spritesheet position
  bool inFirstFrame = player->spriteSheetCounter < PLAYER_WALK_TICKS / 2;
  if (player->yMomentum < 0) {
    player->sheetIndexX = 4;
  } else if (player->yMomentum > 0) {
    player->sheetIndexX = 5;
  } else {
    if (player->xMomentum != 0) {
      player->sheetIndexX = inFirstFrame ? 2 : 3;
    } else {
      player->sheetIndexX = 0;
    }
  }
  if (player->xMomentum > 0) {
    player->sheetIndexY = 0;
  } else if (player->xMomentum < 0) {
    player->sheetIndexY = 1;
  }

  player->spriteSheetCounter++;
  if (player->spriteSheetCounter >= PLAYER_WALK_TICKS) {
    player->spriteSheetCounter = 0;
  }

  COISpriteSetSheetIndex(player->sprite, player->sheetIndexY, player->sheetIndexX);

  if (player->sprite->_y == 0 && tc->level <= 9) {
    tc->level++;
    loadLevel(tc, board, tc->level);

    player->sprite->_y = SCREEN_HEIGHT - player->sprite->_height;
    player->yMomentum = MAX(-15, tc->player->yMomentum - 30);
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
    // if (rock->timeLeft == 0) {
    //   LinkedListRemove(tc->actors, rock);
    //   actorDestroy(rock, board);
    // }
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
                                                        FIREBALL,
                                                        COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COIBoardAddDynamicSprite(board, fireballSprite);
  Actor* fireball = malloc(sizeof(Actor));
  fireball->tick = fireballTick;
  fireball->sprite = fireballSprite;
  fireball->sprite->_visible = true;
  fireball->counter = 0;
  fireball->distanceToTarget[0] = targetX - x;
  fireball->distanceToTarget[1] = targetY - y;
  fireball->timeLeft = LIFETIME;

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
  if (!fireball->sprite->_visible || fireball->sprite->_x < 32) {
    // Out of frame, delete fireball
    LinkedListRemove(tc->actors, fireball);
    actorDestroy(fireball, board);
  } else if (!tc->player->invincible && collisionWithPlayer(fireball->sprite, tc->player->sprite)) {
    tc->player->health = MAX(0, tc->player->health - 1);

    LinkedListRemove(tc->actors, fireball);
    actorDestroy(fireball, board);

    if (tc->player->health == 0) {
      // Player death. Load checkpoint
      tc->player->sprite->_x = 64;
      tc->player->sprite->_y = SCREEN_HEIGHT - 64;
      tc->player->health = MAX_HEALTH;
      tc->player->mana = MAX_MANA;
      for (int i = 0; i < MAX_HEALTH; i++) {
        tc->hearts[i]->_visible = true;
      }
      if (tc->level >= 7) {
        tc->level = 7;
      } else if (tc->level >= 4) {
        tc->level = 4;
      } else {
        tc->level = 1;
      }
      loadLevel(tc, board, tc->level);

    } else {
      tc->player->invincible = true;
      tc->player->sprite->_autoHandle = false;
      tc->hearts[tc->player->health]->_visible = false;
    }
  }
}

Actor* angelCreate(COIBoard* board, int x, int y) {
  COISprite* angelSprite = COISpriteCreateFromAssetID(x, y, 32, 32,
                                                      COI_GLOBAL_LOADER,
                                                      ANGEL,
                                                      COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COISpriteSetSheetIndex(angelSprite, 0, 0);
  COIBoardAddDynamicSprite(board, angelSprite);
  Actor* angel = malloc(sizeof(Actor));
  angel->moving = NONE;
  angel->tick = angelTick;
  angel->counter = 0;
  angel->sprite = angelSprite;
  angel->timeLeft = LIFETIME;

  return angel;                                                
}

void angelTick(Actor* angel, COIBoard* board, void* context) {
  TestContext* tc = (TestContext*)context;

  // Spritesheet stuff
  if (tc->player->sprite->_x > angel->sprite->_x) {
    COISpriteSetSheetIndex(angel->sprite, 0 ,0);
  } else if (tc->player->sprite->_x < angel->sprite->_x) {
    COISpriteSetSheetIndex(angel->sprite, 0, 1);
  }

  angel->counter++;
  if (angel->counter > 100) {
    LinkedListAdd(tc->actors, (void*)fireballCreate(board, angel->sprite->_x, angel->sprite->_y, 
                                                    tc->player->sprite->_x + tc->player->sprite->_width / 2,
                                                    tc->player->sprite->_y + tc->player->sprite->_height / 2));
    angel->counter = 0;
  }
}

Actor* dogCreate(COIBoard* board, int x, int y, bool standingStill) {
  COISprite* dogSprite = COISpriteCreateFromAssetID(x, y, 32, 32,
                                                    COI_GLOBAL_LOADER,
                                                    DOG,
                                                    COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COISpriteSetSheetIndex(dogSprite, 0, 0);
  
  COIBoardAddDynamicSprite(board, dogSprite);
  Actor* dog = malloc(sizeof(Actor));
  if (standingStill) {
    dog->moving = NONE;
    dog->xMomentum = 0;
    dog->yMomentum = 0;
  } else {
    dog->moving = LEFT;
    dog->xMomentum = DOG_MOMENTUM;
    dog->yMomentum = 5;
  }
  dog->tick = dogTick;
  dog->sprite = dogSprite;
  dog->timeLeft = LIFETIME;
  dog->spriteSheetCounter = 0;

  return dog;
}
void dogTick(Actor* dog, COIBoard* board, void* context) {
  TestContext* tc = (TestContext*)context;

  // Special behavior for pickup object, not player-spawned object
  if (tc->souls.length >= 2) {
    dog->timeLeft--;

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


    // Spritesheet stuff
    bool inFirstFrame = dog->spriteSheetCounter < PLAYER_WALK_TICKS / 2;
    if (dog->xMomentum > 0) {
      dog->sheetIndexY = 0;
      dog->sheetIndexX = inFirstFrame ? 1 : 2;
    } else if (dog->xMomentum < 0) {
      dog->sheetIndexY = 1;
      dog->sheetIndexX = inFirstFrame ? 1 : 2;
    } else {
      dog->sheetIndexX = 0;
    }
    COISpriteSetSheetIndex(dog->sprite, dog->sheetIndexY, dog->sheetIndexX);

    dog->spriteSheetCounter++;
    if (dog->spriteSheetCounter >= PLAYER_WALK_TICKS) {
      dog->spriteSheetCounter = 0;
    }

    // Collision with enemies
    LinkedListNode* oldNode = tc->actors->cursor; // Save place, reset when done
    LinkedListResetCursor(tc->actors);
    void* nextData = LinkedListNext(tc->actors);
    Actor* currentActor = (Actor*)nextData;
    while (currentActor) {
      if (currentActor->sprite->_assetID == ANGEL && collisionWithPlayer(dog->sprite, currentActor->sprite) != 0) {
        currentActor->timeLeft = 0;
      }
      currentActor = LinkedListNext(tc->actors);
    }
    tc->actors->cursor = oldNode;
  } else if (collisionWithPlayer(dog->sprite, tc->player->sprite)) {
    printf("got dog soul\n");
    IntListAdd(&tc->souls, DOG);
    tc->activeSoulIndex = tc->souls.length - 1;

    LinkedListRemove(tc->actors, dog);
    actorDestroy(dog, board);
  }
}

void actorDestroy(Actor* actor, COIBoard* board) {
  COIBoardRemoveDynamicSprite(board, actor->sprite);
  COISpriteDestroy(actor->sprite);
  free(actor);
  COIBoardQueueDraw(board);
}