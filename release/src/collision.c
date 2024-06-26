#include "collision.h"
#include "types.h"

bool _hasRightOfCollision(COISprite* sprite) {
    return (sprite->_assetID == PLATFORM_LEFT ||
            sprite->_assetID == PLATFORM_BASE);
}

bool _hasLeftOfCollision(COISprite* sprite) {
    return (sprite->_assetID == PLATFORM_RIGHT ||
            sprite->_assetID == PLATFORM_BASE);
}

bool _hasUnderCollision(COISprite* sprite) {
    return (sprite->_assetID == PLATFORM_LEFT ||
            sprite->_assetID == PLATFORM_MIDDLE ||
            sprite->_assetID == PLATFORM_RIGHT ||
            sprite->_assetID == PLATFORM_BASE ||
            sprite->_assetID == ROCK);
}

unsigned int collisionCheckRightOf(COISprite* sprite, COIBoard* board, int deltaX, int deltaY) {
  // only try bottom left for now
  int lX = sprite->_x + deltaX;
  int rX = lX + sprite->_width;
  int tY = sprite->_y + deltaY;
  int bY = tY + sprite->_height;
  
    
  unsigned int result = 0;
  for (int i = 0; i < board->_spriteCount; i++) {
    if (_hasLeftOfCollision(board->_sprites[i])) {
      if (COISpriteIsLeftOfPoint(board->_sprites[i], lX, bY)) {
          result |= CORNER_COLLIDE_BL;
      }
      if (COISpriteIsLeftOfPoint(board->_sprites[i], lX, tY)) {
        result |= CORNER_COLLIDE_TL;
      }
    }
  }

  LinkedListResetCursor(board->dynamicSprites);
  COISprite* dynSprite = LinkedListNext(board->dynamicSprites);
  while (dynSprite != NULL) {
    if (_hasLeftOfCollision(dynSprite)) {
      if (COISpriteIsLeftOfPoint(dynSprite, lX, bY)) {
        result |= CORNER_COLLIDE_BL;
      }
      if (COISpriteIsLeftOfPoint(dynSprite, rX, bY)) {
        result |= CORNER_COLLIDE_BR;
      }
    }

    dynSprite = LinkedListNext(board->dynamicSprites);
  }

  return result;
}

unsigned int collisionCheckLeftOf(COISprite* sprite, COIBoard* board, int deltaX, int deltaY) {
  // only try bottom left for now
  int lX = sprite->_x + deltaX;
  int rX = lX + sprite->_width;
  int tY = sprite->_y + deltaY;
  int bY = tY + sprite->_height;
  
    
  unsigned int result = 0;
  for (int i = 0; i < board->_spriteCount; i++) {
    if (_hasRightOfCollision(board->_sprites[i])) {
      if (COISpriteIsRightOfPoint(board->_sprites[i], rX, bY)) {
        result |= CORNER_COLLIDE_BR;
      }
      if (COISpriteIsRightOfPoint(board->_sprites[i], rX, tY)) {
        result |= CORNER_COLLIDE_TR;
      }
    }
  }

  LinkedListResetCursor(board->dynamicSprites);
  COISprite* dynSprite = LinkedListNext(board->dynamicSprites);
  while (dynSprite != NULL) {
    if (_hasRightOfCollision(dynSprite)) {
      if (COISpriteIsRightOfPoint(dynSprite, lX, bY)) {
        result |= CORNER_COLLIDE_BL;
      }
      if (COISpriteIsRightOfPoint(dynSprite, rX, bY)) {
        result |= CORNER_COLLIDE_BR;
      }
    }

    dynSprite = LinkedListNext(board->dynamicSprites);
  }

  return result;
}

unsigned int collisionCheckOnTopOf(COISprite* sprite, COIBoard* board, int deltaX, int deltaY) {
  // only try bottom left for now
  int lX = sprite->_x + deltaX;
  int rX = lX + sprite->_width;
  int tY = sprite->_y + deltaY;
  int bY = tY + sprite->_height;
  
    
  unsigned int result = 0;
  for (int i = 0; i < board->_spriteCount; i++) {
    if (_hasUnderCollision(board->_sprites[i])) {
      if (COISpriteUnderPoint(board->_sprites[i], lX, bY)) {
        result |= CORNER_COLLIDE_BL;
      }
      if (COISpriteUnderPoint(board->_sprites[i], rX, bY)) {
        result |= CORNER_COLLIDE_BR;
      }
    }
  }

  LinkedListResetCursor(board->dynamicSprites);
  COISprite* dynSprite = LinkedListNext(board->dynamicSprites);
  while (dynSprite != NULL) {
    if (_hasUnderCollision(dynSprite)) {
      if (COISpriteUnderPoint(dynSprite, lX, bY)) {
        result |= CORNER_COLLIDE_BL;
      }
      if (COISpriteUnderPoint(dynSprite, rX, bY)) {
        result |= CORNER_COLLIDE_BR;
      }
    }

    dynSprite = LinkedListNext(board->dynamicSprites);
  }

  return result;
}

bool _checkIfSpriteInsideRect(COISprite* sprite, SDL_Rect* rect) {
  int lX = sprite->_x;
  int rX = lX + sprite->_width;
  int tY = sprite->_y;
  int bY = tY + sprite->_height;

  int extentX = rect->x + rect->w;
  int extentY = rect->y + rect->h;

  if (lX >= rect->x && lX <= extentX &&
	    tY >= rect->y && tY <= extentY) {
    return true;
  }
  if (rX >= rect->x && rX <= extentX &&
	    tY >= rect->y && tY <= extentY) {
    return true;
  }
  if (rX >= rect->x && rX <= extentX &&
	    bY >= rect->y && bY <= extentY) {
    return true;
  }
  if (lX >= rect->x && lX <= extentX &&
	    bY >= rect->y && bY <= extentY) {
    return true;
  }

  return false;
}

unsigned int sdlRectCollisionReturnOnFirst(SDL_Rect* rect, COIBoard* board) {
  int lX = rect->x;
  int rX = lX + rect->w;
  int tY = rect->y;
  int bY = tY + rect->h;

  unsigned int result = 0;
  for (int i = 0; i < board->_spriteCount; i++) {
    if (COISpriteContainsPoint(board->_sprites[i], lX, bY)) {
      return CORNER_COLLIDE_BL;
    }
    if (COISpriteContainsPoint(board->_sprites[i], rX, bY)) {
      return CORNER_COLLIDE_BR;
    }
    if (COISpriteContainsPoint(board->_sprites[i], lX, tY)) {
      return CORNER_COLLIDE_TL;
    }
    if (COISpriteContainsPoint(board->_sprites[i], rX, tY)) {
      return CORNER_COLLIDE_TR;
    }

    if (_checkIfSpriteInsideRect(board->_sprites[i], rect)) {
      return 1; // Hacky
    }
  }

  LinkedListResetCursor(board->dynamicSprites);
  COISprite* dynSprite = LinkedListNext(board->dynamicSprites);
  while (dynSprite != NULL) {
    if (COISpriteContainsPoint(dynSprite, lX, bY)) {
      return CORNER_COLLIDE_BL;
    }
    if (COISpriteContainsPoint(dynSprite, rX, bY)) {
      return CORNER_COLLIDE_BR;
    }
    if (COISpriteContainsPoint(dynSprite, lX, tY)) {
      return CORNER_COLLIDE_TL;
    }
    if (COISpriteContainsPoint(dynSprite, rX, tY)) {
      return CORNER_COLLIDE_TR;
    }

    if (_checkIfSpriteInsideRect(dynSprite, rect)) {
      return 1; // Hacky
    }

    dynSprite = LinkedListNext(board->dynamicSprites);
  }

  return 0;
}

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
  }

  return result;
}

unsigned int collisionWithPlayer(COISprite* sprite, COISprite* playerSprite) {
  int lX = sprite->_x;
  int rX = lX + sprite->_width;
  int tY = sprite->_y;
  int bY = tY + sprite->_height;

  unsigned int result = 0;
  if (COISpriteContainsPoint(playerSprite, lX, bY)) {
    result |= CORNER_COLLIDE_BL;
  }
  if (COISpriteContainsPoint(playerSprite, rX, bY)) {
    result |= CORNER_COLLIDE_BR;
  }
  if (COISpriteContainsPoint(playerSprite, lX, tY)) {
    result |= CORNER_COLLIDE_TL;
  }
  if (COISpriteContainsPoint(playerSprite, rX, tY)) {
    result |= CORNER_COLLIDE_TR;
  }
  
  return result;
}