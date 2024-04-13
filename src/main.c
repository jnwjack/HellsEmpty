#include "engine/COI.h"

#define MAX_MOMENTUM 10
#define COLLISION_PLATFORM 2

#define CORNER_COLLIDE_TL 1
#define CORNER_COLLIDE_BL 2
#define CORNER_COLLIDE_TR 4
#define CORNER_COLLIDE_BR 8

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
} TestContext;

unsigned int collision(COISprite* sprite, COIBoard* board, int deltaX, int deltaY) {
  // only try bottom left for now
  int lX = sprite->_x + deltaX;
  int rX = lX + sprite->_width;
  int tY = sprite->_y + deltaY;
  int bY = tY + sprite->_height;
  
    
  unsigned int result = 0;
  for (int i = 0; i < board->_spriteCount; i++) {
    if (COISpriteContainsPoint(board->_sprites[i], lX, bY)) {
      result |= CORNER_COLLIDE_BL;
    }
    if (COISpriteContainsPoint(board->_sprites[i], rX, bY)) {
      result |= CORNER_COLLIDE_BR;
    }
    if (COISpriteContainsPoint(board->_sprites[i], lX, tY)) {
      result |= CORNER_COLLIDE_TL;
    }
    if (COISpriteContainsPoint(board->_sprites[i], rX, tY)) {
      result |= CORNER_COLLIDE_TR;
    }

    if (result != 0) {
      return result;
    }
  }

  return 0;
}

void testLoop(COIBoard* board, SDL_Event* event, void* context) {
  TestContext* tc = (TestContext*)context;



  if (event->type == SDL_KEYDOWN) {
    if (event->key.keysym.sym == SDLK_SPACE) {
      tc->yMomentum = MAX(-15, tc->yMomentum - 30);
    } else if (event->key.keysym.sym == SDLK_LEFT) {
      tc->moving = LEFT;
	    // COIBoardMoveSprite(board, tc->square, -30, 0);
    } else if (event->key.keysym.sym == SDLK_RIGHT) {
      tc->moving = RIGHT;
      // COIBoardMoveSprite(board, tc->square, 30, 0);
    }
  } else if (event->type == SDL_KEYUP) {
    if ((event->key.keysym.sym == SDLK_LEFT && tc->moving == LEFT) ||
        (event->key.keysym.sym == SDLK_RIGHT && tc->moving == RIGHT)) {
      tc->moving = NONE;
    }
  }

  if (tc->moving == LEFT) {
    tc->xMomentum = MAX(MAX_MOMENTUM * -1, tc->xMomentum - 1);
  } else if (tc->moving == RIGHT) {
    tc->xMomentum = MIN(MAX_MOMENTUM, tc->xMomentum + 1);
  } else if (tc->moving == NONE && tc->xMomentum != 0) {
    if (tc->xMomentum > 0) {
      tc->xMomentum--;
    } else {
      tc->xMomentum++;
    }
  }

  // Apply gravity
  tc->yMomentum = MIN(MAX_MOMENTUM, tc->yMomentum + 1);

  if (tc->yMomentum > 0) {
    int effectiveYMomentum = tc->yMomentum; 
    for (int y = 0; y <= tc->yMomentum; y++) {
      unsigned int collisionResult = collision(tc->square, board, 0, y);
      if (collisionResult & CORNER_COLLIDE_BL || collisionResult & CORNER_COLLIDE_BR) {
        effectiveYMomentum = y;
        break;
      }
    }

    if (effectiveYMomentum > 0) {
      COIBoardMoveSprite(board, tc->square, 0, effectiveYMomentum);
    }
  } else if (tc->yMomentum < 0) {
    COIBoardMoveSprite(board, tc->square, 0, tc->yMomentum);
  }

  // unsigned int collisionResult = collision(tc->square, board, tc->xMomentum, tc->yMomentum);
  // if (!(collisionResult & CORNER_COLLIDE_BL || collisionResult & CORNER_COLLIDE_BR) || tc->yMomentum < 0) {
  //   COIBoardMoveSprite(board, tc->square, 0, tc->yMomentum);
  // }

  unsigned int collisionResult = collision(tc->square, board, tc->xMomentum, 0);
  if (tc->xMomentum < 0 && !(collisionResult & CORNER_COLLIDE_BL && collisionResult & CORNER_COLLIDE_TL)) {
    COIBoardMoveSprite(board, tc->square, tc->xMomentum, 0);
  } else if (tc->xMomentum > 0 && !(collisionResult & CORNER_COLLIDE_BR && collisionResult & CORNER_COLLIDE_TR)) {
    COIBoardMoveSprite(board, tc->square, tc->xMomentum, 0);
  }

  
  // if (tc->square->_y + tc->square->_height < board->_height) {
  //     int movementVal = MIN(board->_height, tc->square->_y + tc->square->_height + tc->momentum);
  //     COIBoardMoveSprite(board, tc->square, 0, tc->momentum);
  // }
}

int main(int argc, char** argv) {
  COIWindowInit();
  COIAssetLoaderInit();

  COIBoard* testBoard = COIBoardCreate(25, 25, 180, 0, COI_GLOBAL_WINDOW->_width, COI_GLOBAL_WINDOW->_height, COI_GLOBAL_LOADER);
  TestContext* tc = malloc(sizeof(TestContext));
  tc->square = COISpriteCreateFromAssetID(640 / 2, 0, 32, 32, COI_GLOBAL_LOADER, 0, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  tc->yMomentum = 5;
  tc->xMomentum = 0;
  tc->moving = NONE;
  COIBoardSetContext(testBoard, (void*)tc);
  COIBoardLoadSpriteMap(testBoard, COIWindowGetRenderer(COI_GLOBAL_WINDOW), "spritemap.dat");
  COIBoardAddDynamicSprite(testBoard, tc->square);
  COIWindowSetBoard(COI_GLOBAL_WINDOW, testBoard, &testLoop);

  COIWindowLoop(COI_GLOBAL_WINDOW, true);


  COIBoardRemoveDynamicSprite(testBoard, tc->square);
  COISpriteDestroy(tc->square);
  free(tc);
  COIBoardDestroy(testBoard);
  COIAssetLoaderDestroy(COI_GLOBAL_LOADER);
  COIWindowDestroy(COI_GLOBAL_WINDOW);


  return 0;
}
