#pragma once

#include "../playfield.h"
#include "../tetrimino_bag.h"
#include "game_scene.h"
#include <string>

class GameplayScene : public GameScene {
private:
  Playfield *playfield;
  Tetrimino *currentTetrimino;
  TetriminoBag *tetriminoBag;

  int currentLevel = 1;      // Current level
  long currentScore = 0;     // Current score
  int totalLinesCleared = 0; // Total lines cleared

  // Line clear scoring (official Tetris scoring)
  static const int SINGLE_LINE_SCORE = 40;
  static const int DOUBLE_LINE_SCORE = 100;
  static const int TRIPLE_LINE_SCORE = 300;
  static const int TETRIS_LINE_SCORE = 1200;

  void RotateCurrentTetrimino(ROTATE_DIRECTION direction);
  bool WallKick(int fromIndex, int toIndex) const;
  float getFallSpeed();
  void handleInput(float deltaTime);
  void handleLineClears(int linesCleared);
  void updateScore(int linesCleared);
  void updateLevel();
  Tetrimino *generateTetrimino() {
    TETRIMINO_SHAPE nextShape = tetriminoBag->getNextShape();
    return new Tetrimino(nextShape, getFallSpeed());
  }
  std::unique_ptr<Tetrimino> getGhostPiece() {
    auto ghost = std::make_unique<Tetrimino>(*currentTetrimino);

    while (!playfield->isTouchingDown(ghost.get())) {
      ghost->moveDown();
    }
    return ghost;
  }

  // Don't spawn a new tetrimino during animation
  bool canGenerateNewTetrimino() const;

public:
  explicit GameplayScene(const std::string &name);
  void Update() override;
  void Draw() override;
};
