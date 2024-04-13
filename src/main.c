#include "engine/COI.h"

#define MAX_MOMENTUM 10
#define COLLISION_PLATFORM 2

#define CORNER_COLLIDE_TL 1
#define CORNER_COLLIDE_BL 2
#define CORNER_COLLIDE_TR 4
#define CORNER_COLLIDE_BR 8

typedef struct TestContext {
    COISprite* square;
    int momentum;
} TestContext;

unsigned int collision(COISprite* sprite, COIBoard* board) {
  // only try bottom left for now
  int lX = sprite->_x;
  int rX = sprite->_x + sprite->_width;
  int tY = sprite->_y;
  int bY = sprite->_y + sprite->_height;
  
    
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
  }

  return result;
}

void testLoop(COIBoard* board, SDL_Event* event, void* context) {
  TestContext* tc = (TestContext*)context;



    if (event->type == SDL_KEYDOWN) {
        if (event->key.keysym.sym == SDLK_SPACE) {
            tc->momentum = MAX(-15, tc->momentum - 30);
        } else if (event->key.keysym.sym == SDLK_LEFT) {
	  COIBoardMoveSprite(board, tc->square, -30, 0);
	} else if (event->key.keysym.sym == SDLK_RIGHT) {
	  COIBoardMoveSprite(board, tc->square, 30, 0);
	}
    }

    tc->momentum = MIN(MAX_MOMENTUM, tc->momentum + 1);
    unsigned int collisionResult = collision(tc->square, board);
    if (!(collisionResult & CORNER_COLLIDE_BL || collisionResult & CORNER_COLLIDE_BR) || tc->momentum < 0) {
      COIBoardMoveSprite(board, tc->square, 0, tc->momentum);
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
    tc->square = COISpriteCreateFromAssetID(640 / 2, 0, 50, 50, COI_GLOBAL_LOADER, 0, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
    tc->momentum = 5;
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
