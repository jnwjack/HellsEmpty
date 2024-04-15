#include "levels.h"

void loadLevel(TestContext* tc, COIBoard* board, unsigned int level) {
  // Load filename
  char filename[100];
  sprintf(filename, "spritemap_%i.dat", level);
  COIBoardLoadSpriteMap(board, COIWindowGetRenderer(COI_GLOBAL_WINDOW), filename);

  // Clean up old actors
  if (tc->actors) {
    LinkedListResetCursor(tc->actors);
    Actor* currentActor = LinkedListNext(tc->actors);
    while (currentActor) {
      LinkedListRemove(tc->actors, currentActor);
      actorDestroy(currentActor, board);
      currentActor = LinkedListNext(tc->actors);
    }
  }

  switch (level) {
  case 1:
    // LinkedListAdd(tc->actors, (void*)rockCreate(board, 256, 640));
    // LinkedListAdd(tc->actors, (void*)dogCreate(board, 128, 640, true));
    // LinkedListAdd(tc->actors, (void*)dogCreate(board, SCREEN_WIDTH / 3, 0));
    // LinkedListAdd(tc->actors, (void*)angelCreate(board, 500, 250));
    break;
  case 2:
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 500, 250));
    break;
  case 3:
    LinkedListAdd(tc->actors, (void*)rockCreate(board, 350, 250));
    break;
  case 4:
    break;
  case 5:
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 500, 250));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 700, 250));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 600, 450));
    break;
  case 6:
    LinkedListAdd(tc->actors, (void*)dogCreate(board, 512, 512, true));
    break;
  case 7:
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 192, 96));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 256, 128));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 320, 128));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 384, 160));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 384, 256));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 32, 384));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 96, 384));
    break;
  case 8:
    // LinkedListAdd(tc->actors, (void*)rockCreate(board, 256, 640));
    // LinkedListAdd(tc->actors, (void*)dogCreate(board, 128, 640, true));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 640, 384));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 672, 384));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 704, 384));
    break;
  case 9:
    // LinkedListAdd(tc->actors, (void*)rockCreate(board, 256, 640));
    // LinkedListAdd(tc->actors, (void*)dogCreate(board, 128, 640, true));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 192, 96));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 256, 96));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 320, 96));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 384, 96));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 448, 96));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 512, 96));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 576, 96));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 640, 96));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 704, 96));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 768, 96));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 832, 96));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 192, 160));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 256, 160));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 320, 160));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 384, 160));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 448, 160));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 512, 160));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 576, 160));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 640, 160));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 704, 160));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 768, 160));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 832, 160));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 192, 224));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 256, 224));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 320, 224));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 384, 224));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 448, 224));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 512, 224));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 576, 224));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 640, 224));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 704, 224));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 768, 224));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 832, 224));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 192, 288));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 256, 288));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 320, 288));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 384, 288));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 448, 288));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 512, 288));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 576, 288));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 640, 288));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 704, 288));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 768, 288));
    LinkedListAdd(tc->actors, (void*)angelCreate(board, 832, 288));
    break;
  case 10:
    break;
  default:
    
    printf("Error: No level %i\n", level);
  }
}