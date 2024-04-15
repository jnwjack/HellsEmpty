#ifdef __WINDOWS__
#define SDL_MAIN_HANDLED
#endif

#include "collision.h"
#include "actor.h"
#include "levels.h"

#define COLLISION_PLATFORM 2
#define MANABAR_HEIGHT 224 // 32 * 7


// Auxilliary drawing outside of COIWindow loop
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

  SDL_SetRenderDrawColor(COIWindowGetRenderer(COI_GLOBAL_WINDOW), 0, 0, 255, 255);
  SDL_RenderFillRect(COIWindowGetRenderer(COI_GLOBAL_WINDOW), &tc->manaBar);

  // Click Guide
  if (tc->activeSoulIndex > -1) {
    if (tc->canSpawnActor) {
      SDL_SetRenderDrawColor(COIWindowGetRenderer(COI_GLOBAL_WINDOW), 0, 255, 0, 120);
    } else {
      SDL_SetRenderDrawColor(COIWindowGetRenderer(COI_GLOBAL_WINDOW), 255, 0, 0, 120);
    }

    SDL_RenderFillRect(COIWindowGetRenderer(COI_GLOBAL_WINDOW), &tc->clickGuide);
  }

  // Reset old values
  SDL_SetRenderDrawColor(COIWindowGetRenderer(COI_GLOBAL_WINDOW), r, g, b, a);
}

void endLoop(COIBoard* board, SDL_Event* event, void* context) {
  // Do Nothing
}

void startLoop(COIBoard* board, SDL_Event* event, void* context) {

}

void testLoop(COIBoard* board, SDL_Event* event, void* context) {
  TestContext* tc = (TestContext*)context;

  COIBoardQueueDraw(board);

  if (tc->level >= 10) {
    COIStringSetVisible(tc->endText, true);
  }

  int mouseX, mouseY;
  SDL_GetMouseState(&mouseX, &mouseY);
  if (mouseX != tc->mouseX || mouseY != tc->mouseY) {
    COIBoardQueueDraw(board);
  }
  tc->mouseX = mouseX;
  tc->mouseY = mouseY;

  if (tc->activeSoulIndex > -1) {
    int soul = tc->souls.values[tc->activeSoulIndex];
    // 3x3 block
    if (soul == DOG) {
       tc->clickGuide.x = tc->mouseX - (32 / 2) - 32;
       tc->clickGuide.w = 96;
       tc->clickGuide.y = tc->mouseY - (32 / 2) - 32;
       tc->clickGuide.h = 96;
    } else if (soul == ROCK) {
      tc->clickGuide.x = tc->mouseX - (32 / 2);
      tc->clickGuide.w = 32;
      tc->clickGuide.y = tc->mouseY - (32 / 2);
      tc->clickGuide.h = 32;
    }

    tc->canSpawnActor = sdlRectCollisionReturnOnFirst(&tc->clickGuide, board) == 0;
  }

  // Accept input
  if (event->type == SDL_KEYDOWN) {
    if (!tc->started) {
      tc->started = true;
      COIStringSetVisible(tc->title, false);
      COIStringSetVisible(tc->instructions, false);
    }
    if (event->key.keysym.sym == SDLK_SPACE && !tc->player->jumping) {
      tc->player->yMomentum = MAX(-15, tc->player->yMomentum - 30);
      tc->player->jumping = true;
    } else if (event->key.keysym.sym == SDLK_a) {
      tc->player->moving = LEFT;
	    // COIBoardMoveSprite(board, tc->square, -30, 0);
    } else if (event->key.keysym.sym == SDLK_d) {
      tc->player->moving = RIGHT;
      // COIBoardMoveSprite(board, tc->square, 30, 0);
    }
  } else if (event->type == SDL_KEYUP) {
    if ((event->key.keysym.sym == SDLK_a && tc->player->moving == LEFT) ||
        (event->key.keysym.sym == SDLK_d && tc->player->moving == RIGHT)) {
      tc->player->moving = NONE;
    } else if (event->key.keysym.sym == SDLK_q) {
      if (tc->activeSoulIndex > -1) {
        COIBoardQueueDraw(board);
        tc->activeSoulIndex = tc->activeSoulIndex == 0 ? tc->souls.length - 1 : tc->activeSoulIndex - 1;
      }
    } else if (event->key.keysym.sym == SDLK_e) {
      if (tc->activeSoulIndex > -1) {
        COIBoardQueueDraw(board);
        tc->activeSoulIndex = tc->activeSoulIndex == tc->souls.length - 1 ? 0 : tc->activeSoulIndex + 1;
      }
    }
  } else if (event->type == SDL_MOUSEBUTTONUP) {
    int manaCost = 0;
    if (tc->souls.values[tc->activeSoulIndex] == ROCK) {
      manaCost = 100;
    } else if (tc->souls.values[tc->activeSoulIndex] == DOG) {
      manaCost = 200; 
    }

    if (tc->activeSoulIndex > -1 && tc->canSpawnActor && tc->player->mana >= manaCost) {
      if (tc->souls.values[tc->activeSoulIndex] == ROCK) {
        LinkedListAdd(tc->actors, rockCreate(board, tc->mouseX - 32 / 2, tc->mouseY - 32 / 2));
      } else if (tc->souls.values[tc->activeSoulIndex] == DOG) {
        LinkedListAdd(tc->actors, dogCreate(board, tc->mouseX - 32 / 2, tc->mouseY - 32 / 2, false));
      }
      
      tc->player->mana -= manaCost;
      COIBoardQueueDraw(board);
    }
  }

  // Update game state
  playerTick(tc->player, board, context);
  LinkedListResetCursor(tc->actors);
  Actor* currentActor = LinkedListNext(tc->actors);
  while (currentActor) {
    if (currentActor->timeLeft == 0) {
      Actor* nextActor = LinkedListNext(tc->actors);
      LinkedListRemove(tc->actors, currentActor);
      actorDestroy(currentActor, board);
      currentActor = nextActor;
    } else {
      currentActor->tick(currentActor, board, context);
      currentActor = LinkedListNext(tc->actors);
    }
  }
}

int main(int argc, char** argv) {
  SDL_SetMainReady();

  COIWindowInit(SCREEN_WIDTH, SCREEN_HEIGHT);
  COIAssetLoaderInit();

  COIBoard* testBoard = COIBoardCreate(25, 25, 180, 0, COI_GLOBAL_WINDOW->_width, COI_GLOBAL_WINDOW->_height * 3, COI_GLOBAL_LOADER);
  TestContext* tc = malloc(sizeof(TestContext));
  tc->level = 9;
  tc->actors = LinkedListCreate();
  tc->player = playerCreate(testBoard, SCREEN_WIDTH / 3, SCREEN_HEIGHT - 128);
  tc->canSpawnActor = false;
  tc->titleTextType = COITextTypeCreate(160, 230, 10, 25, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  tc->smallerTextType = COITextTypeCreate(64, 255, 255, 255, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  tc->title = COIStringCreate("Hell's Empty", 32, 64, tc->titleTextType);
  tc->instructions = COIStringCreate("Press any key to begin", 128, 320, tc->smallerTextType);
  tc->endText = COIStringCreate("Fin.", 384, 128, tc->titleTextType);
  tc->started = false;
  COIBoardAddString(testBoard, tc->title);
  COIBoardAddString(testBoard, tc->instructions);
  COIBoardAddString(testBoard, tc->endText);
  COIStringSetVisible(tc->title, true);
  COIStringSetVisible(tc->instructions, true);
  COIStringSetVisible(tc->endText, false);
  loadLevel(tc, testBoard, tc->level);

  for (int i = 0; i < MAX_HEALTH; i++) {
    COISprite* heart = COISpriteCreateFromAssetID(0, i * 32, 32, 32, COI_GLOBAL_LOADER, HEART, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
    heart->_visible = true;
    heart->_autoHandle = false;
    COIBoardAddDynamicSprite(testBoard, heart);
    tc->hearts[i] = heart;
  }

  IntListInitialize(&tc->souls, 3);
  tc->activeSoulIndex = -1;
  tc->manaBar.x = SCREEN_WIDTH - 32;
  tc->manaBar.y = 0;
  tc->manaBar.w = 32;
  tc->manaBar.h = MANABAR_HEIGHT;
  COIBoardSetContext(testBoard, (void*)tc);
  // COIBoardLoadSpriteMap(testBoard, COIWindowGetRenderer(COI_GLOBAL_WINDOW), "spritemap.dat");
  // COIBoardAddDynamicSprite(testBoard, tc->square);
  COIWindowSetBoard(COI_GLOBAL_WINDOW, testBoard, &testLoop);
  COI_GLOBAL_WINDOW->extraDraw = &extraDraw;

  COIWindowLoop(COI_GLOBAL_WINDOW, true);


  // COIBoardRemoveDynamicSprite(testBoard, tc->square);
  // COISpriteDestroy(tc->square);
  IntListDestroy(&tc->souls);
  free(tc);
  COIBoardDestroy(testBoard);
  COIAssetLoaderDestroy(COI_GLOBAL_LOADER);
  COIWindowDestroy(COI_GLOBAL_WINDOW);


  return 0;
}
