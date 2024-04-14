#ifdef __WINDOWS__
#define SDL_MAIN_HANDLED
#endif

#include "collision.h"
#include "actor.h"

#define COLLISION_PLATFORM 2
#define MANABAR_HEIGHT 224 // 32 * 7


// Second arg is always NULL. Don't use it.
void extraDraw(COIBoard* board, SDL_Event* _, void* context) {
  TestContext* tc = (TestContext*)context;

  float portion = ((float)tc->player->mana / (float)MAX_MANA);

  tc->manaBar.h = MANABAR_HEIGHT * portion;

  // Old color values. Save for later when we're done drawing.
  Uint8 r, g, b, a;
  if (SDL_GetRenderDrawColor(COIWindowGetRenderer(COI_GLOBAL_WINDOW), &r, &g, &b, &a) != 0) {
    printf("Error when retrieving old renderer color values.\n");
  }

  // Auxilliary drawing outside of COIWindow loop
  SDL_SetRenderDrawColor(COIWindowGetRenderer(COI_GLOBAL_WINDOW), 0, 0, 255, 255);
  SDL_RenderFillRect(COIWindowGetRenderer(COI_GLOBAL_WINDOW), &tc->manaBar);

  // Reset old values
  SDL_SetRenderDrawColor(COIWindowGetRenderer(COI_GLOBAL_WINDOW), r, g, b, a);
}

void testLoop(COIBoard* board, SDL_Event* event, void* context) {
  TestContext* tc = (TestContext*)context;


  // Accept input
  if (event->type == SDL_KEYDOWN) {
    if (event->key.keysym.sym == SDLK_SPACE) {
      tc->player->yMomentum = MAX(-15, tc->player->yMomentum - 30);
    } else if (event->key.keysym.sym == SDLK_LEFT) {
      tc->player->moving = LEFT;
	    // COIBoardMoveSprite(board, tc->square, -30, 0);
    } else if (event->key.keysym.sym == SDLK_RIGHT) {
      tc->player->moving = RIGHT;
      // COIBoardMoveSprite(board, tc->square, 30, 0);
    }
  } else if (event->type == SDL_KEYUP) {
    if ((event->key.keysym.sym == SDLK_LEFT && tc->player->moving == LEFT) ||
        (event->key.keysym.sym == SDLK_RIGHT && tc->player->moving == RIGHT)) {
      tc->player->moving = NONE;
    }
  } else if (event->type == SDL_MOUSEBUTTONUP) {
    if (tc->player->mana >= 200) {
      int mouseX, mouseY;
      SDL_GetMouseState(&mouseX, &mouseY);
      LinkedListAdd(tc->actors, rockCreate(board, mouseX - 32 / 2, mouseY - 32 / 2));
      
      COIBoardQueueDraw(board);

      tc->player->mana -= 200;
    }
  }

  // Update game state
  playerTick(tc->player, board, context);
  LinkedListResetCursor(tc->actors);
  Actor* currentActor = LinkedListNext(tc->actors);
  while (currentActor) {
    currentActor->tick(currentActor, board, context);
    currentActor = LinkedListNext(tc->actors);
  }
}

int main(int argc, char** argv) {
  SDL_SetMainReady();

  COIWindowInit();
  COIAssetLoaderInit();

  COIBoard* testBoard = COIBoardCreate(25, 25, 180, 0, COI_GLOBAL_WINDOW->_width, COI_GLOBAL_WINDOW->_height * 3, COI_GLOBAL_LOADER);
  TestContext* tc = malloc(sizeof(TestContext));
  // tc->square = COISpriteCreateFromAssetID(640 / 2, 0, 32, 32, COI_GLOBAL_LOADER, 0, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  // tc->yMomentum = 5;
  // tc->xMomentum = 0;
  // tc->moving = NONE;
  tc->actors = LinkedListCreate();
  tc->player = playerCreate(testBoard, 640 / 2, 0);
  LinkedListAdd(tc->actors, (void*)dogCreate(testBoard, 640 / 3, 0));
  // LinkedListAdd(tc->actors, (void*)angelCreate(testBoard, 500, 250));
  // LinkedListAdd(tc->actors, (void*)angelCreate(testBoard, 400, 150));
  // LinkedListAdd(tc->actors, (void*)angelCreate(testBoard, 200, 360));
  // LinkedListAdd(tc->actors, (void*)angelCreate(testBoard, 300, 400));

  for (int i = 0; i < MAX_HEALTH; i++) {
    COISprite* heart = COISpriteCreateFromAssetID(0, i * 32, 32, 32, COI_GLOBAL_LOADER, HEART, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
    heart->_visible = true;
    heart->_autoHandle = false;
    COIBoardAddDynamicSprite(testBoard, heart);
    tc->hearts[i] = heart;
  }

  tc->manaBar.x = 640 - 32;
  tc->manaBar.y = 0;
  tc->manaBar.w = 32;
  tc->manaBar.h = MANABAR_HEIGHT;
  COIBoardSetContext(testBoard, (void*)tc);
  COIBoardLoadSpriteMap(testBoard, COIWindowGetRenderer(COI_GLOBAL_WINDOW), "spritemap.dat");
  // COIBoardAddDynamicSprite(testBoard, tc->square);
  COIWindowSetBoard(COI_GLOBAL_WINDOW, testBoard, &testLoop);
  COI_GLOBAL_WINDOW->extraDraw = &extraDraw;

  COIWindowLoop(COI_GLOBAL_WINDOW, true);


  // COIBoardRemoveDynamicSprite(testBoard, tc->square);
  // COISpriteDestroy(tc->square);
  free(tc);
  COIBoardDestroy(testBoard);
  COIAssetLoaderDestroy(COI_GLOBAL_LOADER);
  COIWindowDestroy(COI_GLOBAL_WINDOW);


  return 0;
}
