#ifdef __WINDOWS__
#define SDL_MAIN_HANDLED
#endif

#include "collision.h"
#include "actor.h"

#define COLLISION_PLATFORM 2

void testLoop(COIBoard* board, SDL_Event* event, void* context) {
  TestContext* tc = (TestContext*)context;

  LinkedListResetCursor(tc->actors);
  Actor* currentActor = LinkedListNext(tc->actors);
  while (currentActor) {
    currentActor->tick(currentActor, board, context);
    currentActor = LinkedListNext(tc->actors);
  }

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
  } else if (event->type == SDL_MOUSEBUTTONUP) {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    // Add rock sprite
    COISprite* rock = COISpriteCreateFromAssetID(mouseX - 32 / 2, mouseY - 32 / 2, 32, 32,
                                                COI_GLOBAL_LOADER,
                                                ROCK,
                                                COIWindowGetRenderer(COI_GLOBAL_WINDOW));
    // rock->_visible = true;
    // rock->_autoHandle = false;
    COIBoardAddDynamicSprite(board, rock);
    // printf("HEYO x: %i y: %i\n", mouseX, mouseY);
    COIBoardQueueDraw(board);
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
      unsigned int collisionResult = collisionCheckOnTopOf(tc->square, board, 0, y);
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

  if (tc->xMomentum > 0) {
    int effectiveXMomentum = tc->xMomentum;
    for (int x = 0; x <= tc->xMomentum; x++) {
      unsigned int collisionResult = collisionCheckLeftOf(tc->square, board, x, 0);
      if (collisionResult & CORNER_COLLIDE_BR || collisionResult & CORNER_COLLIDE_TR) {
        effectiveXMomentum = x;
        break;
      }
    }

    if (effectiveXMomentum > 0) {
      COIBoardMoveSprite(board, tc->square, effectiveXMomentum, 0);
    }

    tc->xMomentum = effectiveXMomentum;
  } else if (tc->xMomentum < 0) {
    // printf("neg. momentum\n");
    int effectiveXMomentum = tc->xMomentum;
    for (int x = 0; x >= tc->xMomentum; x--) {
      unsigned int collisionResult = collisionCheckRightOf(tc->square, board, x, 0);
      if (collisionResult & CORNER_COLLIDE_BL || collisionResult & CORNER_COLLIDE_TL) {
        effectiveXMomentum = x;
        break;
      }
    }

    if (effectiveXMomentum < 0) {
      COIBoardMoveSprite(board, tc->square, effectiveXMomentum, 0);
    }

    tc->xMomentum = effectiveXMomentum;
  }
}

int main(int argc, char** argv) {
  SDL_SetMainReady();

  COIWindowInit();
  COIAssetLoaderInit();

  COIBoard* testBoard = COIBoardCreate(25, 25, 180, 0, COI_GLOBAL_WINDOW->_width, COI_GLOBAL_WINDOW->_height * 3, COI_GLOBAL_LOADER);
  TestContext* tc = malloc(sizeof(TestContext));
  tc->square = COISpriteCreateFromAssetID(640 / 2, 0, 32, 32, COI_GLOBAL_LOADER, 0, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  tc->yMomentum = 5;
  tc->xMomentum = 0;
  tc->moving = NONE;
  tc->actors = LinkedListCreate();
  LinkedListAdd(tc->actors, (void*)dogCreate(testBoard, 640 / 3, 0));
  LinkedListAdd(tc->actors, (void*)angelCreate(testBoard, 500, 250));
  COIBoardSetContext(testBoard, (void*)tc);
  COIBoardLoadSpriteMap(testBoard, COIWindowGetRenderer(COI_GLOBAL_WINDOW), "spritemap.dat");
  COIBoardAddDynamicSprite(testBoard, tc->square);
  COIWindowSetBoard(COI_GLOBAL_WINDOW, testBoard, &testLoop);

  printf("beforeloop\n");
  COIWindowLoop(COI_GLOBAL_WINDOW, true);


  COIBoardRemoveDynamicSprite(testBoard, tc->square);
  COISpriteDestroy(tc->square);
  free(tc);
  COIBoardDestroy(testBoard);
  COIAssetLoaderDestroy(COI_GLOBAL_LOADER);
  COIWindowDestroy(COI_GLOBAL_WINDOW);


  return 0;
}
