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
  default:
    printf("Error: No level %i\n", level);
  }
}