#ifndef COLLISION_H
#define COLLISION_H

#include "engine/COI.h"

// Asset IDs
#define PLATFORM_BASE 1
#define PLATFORM_LEFT 2
#define PLATFORM_RIGHT 3
#define PLATFORM_MIDDLE 4
#define ROCK 5
#define DOG 6
#define ANGEL 7
#define HEART 8

#define CORNER_COLLIDE_TL 1
#define CORNER_COLLIDE_BL 2
#define CORNER_COLLIDE_TR 4
#define CORNER_COLLIDE_BR 8

unsigned int collisionCheckOnTopOf(COISprite* sprite, COIBoard* board, int deltaX, int deltaY);
unsigned int collisionCheckLeftOf(COISprite* sprite, COIBoard* board, int deltaX, int deltaY);
unsigned int collisionCheckRightOf(COISprite* sprite, COIBoard* board, int deltaX, int deltaY);
unsigned int collision(COISprite* sprite, COIBoard* board, int deltaX, int deltaY);
unsigned int collisionWithPlayer(COISprite* sprite, COISprite* playerSprite);
unsigned int sdlRectCollisionReturnOnFirst(SDL_Rect* rect, COIBoard* board);

#endif