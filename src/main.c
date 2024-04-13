#include "engine/COI.h"

#define MAX_MOMENTUM 10
typedef struct TestContext {
    COISprite* square;
    int momentum;
} TestContext;

void testLoop(COIBoard* board, SDL_Event* event, void* context) {
    TestContext* tc = (TestContext*)context;

    if (event->type == SDL_KEYDOWN) {
        if (event->key.keysym.sym == SDLK_SPACE) {
            tc->momentum = MAX(-15, tc->momentum - 30);
        }
    }

    tc->momentum = MIN(MAX_MOMENTUM, tc->momentum + 1);
    int movementVal = MIN(board->_height, tc->square->_y + tc->square->_height + tc->momentum);
    COIBoardMoveSprite(board, tc->square, 0, tc->momentum);
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