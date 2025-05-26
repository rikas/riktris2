#pragma once

#include "globals.h"
#include "line_clear_animation.h"
#include "mino_grid.h"
#include "tetrimino.h"
#include "utils.h"
#include <raylib.h>
#include <vector>

// The padding in pixels around the playfield texture (basically the borders of
// the texture). This will be useful to know where to start drawing the tetriminos.
#define PLAYFIELD_PADDING_X 6u
#define PLAYFIELD_PADDING_Y 7u

class Playfield {
private:
  LineClearAnimation clearAnimation;
  Texture2D playfieldTexture;
  MinoGrid grid;

  MinoGrid tempGrid;                 // for line clearing animation
  std::vector<float> rowDropOffsets; // Offsets for dropping rows during animation

  // The x,y position of the playfield texture in the window.
  Vector2 position;

  // Where to start drawing the tetriminos in the playfield texture.
  Vector2 drawStart;

public:
  Playfield() {
    // Load necessary textures
    playfieldTexture = LoadTexture("data/gfx/playfield.png");

    // Center the playfield texture in the window
    position = {(float)WINDOW_W / 2 - (float)playfieldTexture.width / 2,
                (float)WINDOW_H / 2 - (float)playfieldTexture.height / 2};

    drawStart = {position.x + PLAYFIELD_PADDING_X, position.y + PLAYFIELD_PADDING_Y};
  }

  ~Playfield() { UnloadTexture(playfieldTexture); }

  void Update();

  // Drawing with animation support
  void Draw();
  void drawWithAnimation();
  void drawFlashingRows();
  void drawDroppingRows();

  Vector2 getDrawStart() const { return drawStart; }
  bool isTouchingLeft(Tetrimino *mino) const;
  bool isTouchingRight(Tetrimino *mino) const;
  bool isTouchingDown(Tetrimino *mino) const;
  bool TetriminoOverlapping(Tetrimino *tetrimino) const;
  std::vector<int> getCompletedRows() const { return grid.getCompletedRows(); }

  void RotateCurrentTetrimino();
  void addTetriminoToGrid(Tetrimino *tetrimino) { grid.addTetrimino(tetrimino); }

  // Modified line clearing
  void executeLineClear();

  // Animation methods
  void startLineClearAnimation(const std::vector<int> &rows);
  bool isAnimationRunning() const { return clearAnimation.isActive; }
};
