#include "tetrimino.h"
#include "mino_grid.h"

void Tetrimino::Draw(int offsetX, int offsetY, MINO_DRAW_TYPE draw_type) {
  int tx, ty;

  for (int x = 0; x < NUMBER_OF_ROTATIONS; x++) {
    for (int y = 0; y < NUMBER_OF_ROTATIONS; y++) {
      tx = x * (MINO_W + 1) + offsetX + col * (MINO_W + 1);
      ty = y * (MINO_W + 1) + offsetY + row * (MINO_W + 1);

      if (isFilled(x, y)) {
        if (draw_type == MINO_BLOCK) {
          DrawTexture(*minoTexture, tx, ty, Fade(WHITE, lockTimer > 0 ? 0.7f - lockTimer : 1));
        } else {
          DrawTexture(*ghostTexture, tx, ty, Fade(WHITE, 0.5f)); // Semi-transparent ghost
        }
      }
    }
  }
}

bool Tetrimino::isFilled(int x, int y) {
  return getRotation() & (0x8000 >> (y * NUMBER_OF_ROTATIONS + x));
}

void Tetrimino::Rotate(ROTATE_DIRECTION direction) {
  int newIndex = 0;

  switch (direction) {
  case LEFT:
    newIndex = (rotationIndex - 1) % NUMBER_OF_ROTATIONS;
    break;
  case RIGHT:
    newIndex = (rotationIndex + 1) % NUMBER_OF_ROTATIONS;
    break;
  }

  if (newIndex < 0) {
    newIndex = NUMBER_OF_ROTATIONS - 1;
  }

  rotationIndex = newIndex;
}
