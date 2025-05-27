#include "gameplay_scene.h"
#include "../sound_manager.h"
#include <iostream>
#include <raylib.h>

float leftKeyTimer = 0.0f;
float rightKeyTimer = 0.0f;
float downKeyTimer = 0.0f;
float fallTimer = 0.0f;

const float LOCK_DELAY = 0.5f;        // Time before a tetrimino locks in place
const float KEY_REPEAT_DELAY = 0.15f; // Initial delay before repeating
const float KEY_REPEAT_RATE = 0.05f;  // Time between repeats

GameplayScene::GameplayScene(const std::string &name) : GameScene(name) {
  playfield = new Playfield();
  tetriminoBag = new TetriminoBag();
  currentTetrimino = generateTetrimino();

  // Preload the sounds that will be used in the scene
  SoundManager soundManager = SoundManager::getInstance();
  soundManager.preloadSound("move_new.wav");
  soundManager.preloadSound("rotate_new.wav");
  soundManager.preloadSound("lock.wav");
}

// Official Tetris speed curve (frames at 60 FPS)
// Level 1: 48 frames = 0.8 seconds
// Level 2: 43 frames = ~0.72 seconds
// etc.
float GameplayScene::getFallSpeed() {
  static const float speeds[] = {
      0.8f,  // Level 1
      0.72f, // Level 2
      0.63f, // Level 3
      0.55f, // Level 4
      0.47f, // Level 5
      0.38f, // Level 6
      0.30f, // Level 7
      0.22f, // Level 8
      0.13f, // Level 9
      0.10f, // Level 10
      0.08f, // Level 11
      0.07f, // Level 12
      0.05f, // Level 13
      0.04f, // Level 14
      0.03f, // Level 15
      0.02f  // Level 16+
  };

  int levelIndex = std::min(currentLevel - 1, 15); // Cap at level 16
  return speeds[levelIndex];
}

void GameplayScene::RotateCurrentTetrimino(ROTATE_DIRECTION direction) {
  int originalRotation = currentTetrimino->getRotationIndex();

  // Rotate the tetrimino in the current direction, then we need to check if we need to reset the
  // rotation or if we can keep it.
  std::cout << "Rotating tetrimino from rotation " << originalRotation << std::endl;
  currentTetrimino->Rotate(direction);

  // Col and row before the wall kick calculations
  int origCol = currentTetrimino->getCol();
  int origRow = currentTetrimino->getRow();
  bool kickSuccess = false;

  // If the resulting rotation will overlap existing minos or will end up outside of the playfield
  // then we need to perform a wall kick. This may or may not succeed, depending on where the
  // tetrimino is. If it doesn't succeed then we assume we can't rotate the tetrimino at all and
  // put it back to the original rotation.
  if (playfield->TetriminoOverlapping(currentTetrimino)) {
    std::cout << "Tetrimino overlapping, attempting wall kick." << std::endl;
    kickSuccess = WallKick(originalRotation, currentTetrimino->getRotationIndex());

    if (!kickSuccess) {
      currentTetrimino->setRotation(originalRotation);
      currentTetrimino->setCol(origCol);
      currentTetrimino->setRow(origRow);
    }
  }
}

// Try to wall kick a tetrimino. This should be called only if the tetrimino is overlapping existing
// minos in the playfield or out of bounds. Depending on the rotation being executed different
// coordinates will be tested for the wall kick.
//
// This function will return true if it successfully wall kicked the tetrimino or false otherwise.
bool GameplayScene::WallKick(int fromRotation, int toRotation) const {
  // Will store the kick coordinates for the given rotation
  KickData kicks;
  KickData *kickData;

  if (currentTetrimino->getShape() == TETRIMINO_I) {
    kickData = WALL_KICKS_I;
  } else {
    kickData = WALL_KICKS;
  }

  if (fromRotation == 0 && toRotation == 1) {
    memcpy(kicks, kickData[0], sizeof(kicks));
  } else if (fromRotation == 1 && toRotation == 0) {
    memcpy(kicks, kickData[1], sizeof(kicks));
  } else if (fromRotation == 1 && toRotation == 2) {
    memcpy(kicks, kickData[2], sizeof(kicks));
  } else if (fromRotation == 2 && toRotation == 1) {
    memcpy(kicks, kickData[3], sizeof(kicks));
  } else if (fromRotation == 2 && toRotation == 3) {
    memcpy(kicks, kickData[4], sizeof(kicks));
  } else if (fromRotation == 3 && toRotation == 2) {
    memcpy(kicks, kickData[5], sizeof(kicks));
  } else if (fromRotation == 3 && toRotation == 0) {
    memcpy(kicks, kickData[6], sizeof(kicks));
  } else if (fromRotation == 0 && toRotation == 3) {
    memcpy(kicks, kickData[7], sizeof(kicks));
  }

  int origCol = currentTetrimino->getCol();
  int origRow = currentTetrimino->getRow();

  // Tries wall kicks until we're done
  for (int i = 0; i < NUMBER_OF_ROTATIONS; i++) {
    currentTetrimino->setCol(origCol + kicks[i][0]);
    currentTetrimino->setRow(origRow + kicks[i][1]);

    if (!playfield->TetriminoOverlapping(currentTetrimino)) {
      return true;
    }
  }

  // We couldn't wall kick!
  return false;
}

void GameplayScene::Update() {
  float deltaTime = GetFrameTime();

  // Always update animations
  playfield->Update();

  // Pause game logic during line clear animations
  if (playfield->isAnimationRunning()) {
    return;
  }

  float currentFallSpeed = getFallSpeed();
  fallTimer += deltaTime;

  // Check if the current tetrimino should fall 1 row down
  if (fallTimer >= currentFallSpeed && !currentTetrimino->isLocked()) {
    if (!playfield->isTouchingDown(currentTetrimino)) {
      currentTetrimino->moveDown();
      currentTetrimino->resetLockTimer(); // Reset lock timer
    } else {
      if (!currentTetrimino->isLocking()) {
        // Just landed
        currentTetrimino->resetLockTimer(); // Reset lock timer
      }
    }
    fallTimer = 0.0f; // Reset fall timer
  }

  // Handle lock delay
  if (playfield->isTouchingDown(currentTetrimino) && !currentTetrimino->isLocked()) {
    currentTetrimino->addToLockTimer(deltaTime);

    if (currentTetrimino->getLockTimer() >= LOCK_DELAY) {
      std::cout << "Locking tetrimino after " << currentTetrimino->getLockTimer() << " seconds."
                << std::endl;
      currentTetrimino->lock();

      Sound lockSfx = SoundManager::getInstance().getSound("soundss.wav");
      PlaySound(lockSfx);
    }

    // Reset lock timer if piece moves away from the bottom
    // if (!playfield->isTouchingDown(currentTetrimino)) {
    //   currentTetrimino->resetLockTimer(); // Reset lock timer
    // }
  }

  if (currentTetrimino->isLocked() && !playfield->isAnimationRunning()) {
    playfield->addTetriminoToGrid(currentTetrimino); // Add the tetrimino to the grid

    // Check for and clear completed lines
    // int linesCleared = playfield->clearCompletedLines();
    std::vector<int> completedRows = playfield->getCompletedRows();

    if (!completedRows.empty()) {
      // Start animation instead of immediate clearing
      playfield->startLineClearAnimation(completedRows);
      // playfield->executeLineClear();
      handleLineClears(completedRows.size());
    }
  }

  // Generate a new tetrimino only if the current one is locked and no animation is running
  if (currentTetrimino->isLocked()) {
    delete currentTetrimino;                // Clean up the old tetrimino
    currentTetrimino = generateTetrimino(); // Create a new one
    return;
  }

  handleInput(deltaTime);
}

void GameplayScene::handleInput(float deltaTime) {
  /********************************************
   * UP KEY HANDLING (ROTATION CLOCKWISE)
   ********************************************/
  if (IsKeyPressed(KEY_UP) && !currentTetrimino->isLocked()) {
    RotateCurrentTetrimino(ROTATE_DIRECTION::RIGHT);
    currentTetrimino->resetLockTimer(); // Reset lock timer
    Sound rotateSfx = SoundManager::getInstance().getSound("rotate_new.wav");
    PlaySound(rotateSfx);
  }

  /********************************************
   * SPACE KEY HANDLING (HARD DROP)
   ********************************************/
  if (IsKeyPressed(KEY_SPACE) && !currentTetrimino->isLocked()) {
    // TODO: this is to be used for points
    int dropDistance = 0;

    while (!playfield->isTouchingDown(currentTetrimino)) {
      currentTetrimino->moveDown();
      dropDistance++;
    }

    currentTetrimino->lock();

    // TODO: maybe play a different sound for hard drop
    Sound lockSfx = SoundManager::getInstance().getSound("soundss.wav");
    SetSoundPitch(lockSfx, 4.1f);
    PlaySound(lockSfx);
  }

  /********************************************
   * RIGHT KEY HANDLING
   ********************************************/
  if (IsKeyDown(KEY_RIGHT) && !currentTetrimino->isLocked()) {
    if (IsKeyPressed(KEY_RIGHT)) {
      // First press, move immediately
      if (!playfield->isTouchingRight(currentTetrimino)) {
        currentTetrimino->resetLockTimer(); // Reset lock timer
        currentTetrimino->moveRight();
        Sound moveSfx = SoundManager::getInstance().getSound("move_new.wav");
        PlaySound(moveSfx);
      }
      rightKeyTimer = KEY_REPEAT_DELAY; // Set initial delay
    } else {
      // Key held down - check timer
      rightKeyTimer -= deltaTime;
      if (rightKeyTimer <= 0.0f) {
        if (!playfield->isTouchingRight(currentTetrimino)) {
          currentTetrimino->resetLockTimer(); // Reset lock timer
          currentTetrimino->moveRight();
          Sound moveSfx = SoundManager::getInstance().getSound("move_new.wav");
          PlaySound(moveSfx);
        }
        rightKeyTimer = KEY_REPEAT_RATE; // Set repeat rate
      }
    }
  } else {
    rightKeyTimer = 0.0f; // Reset timer when key released
  }

  /********************************************
   * LEFT KEY HANDLING
   ********************************************/
  if (IsKeyDown(KEY_LEFT) && !currentTetrimino->isLocked()) {
    if (IsKeyPressed(KEY_LEFT)) {
      // First press, move immediately
      if (!playfield->isTouchingLeft(currentTetrimino)) {
        currentTetrimino->resetLockTimer(); // Reset lock timer
        currentTetrimino->moveLeft();
      }
      leftKeyTimer = KEY_REPEAT_DELAY; // Set initial delay
    } else {
      // Key held down - check timer
      leftKeyTimer -= deltaTime;
      if (leftKeyTimer <= 0.0f) {
        if (!playfield->isTouchingLeft(currentTetrimino)) {
          currentTetrimino->resetLockTimer(); // Reset lock timer
          currentTetrimino->moveLeft();
        }
        leftKeyTimer = KEY_REPEAT_RATE; // Set repeat rate
      }
    }
  } else {
    leftKeyTimer = 0.0f; // Reset timer when key released
  }

  /********************************************
   * DOWN KEY HANDLING
   ********************************************/
  if (IsKeyDown(KEY_DOWN) && !currentTetrimino->isLocked()) {
    if (IsKeyPressed(KEY_DOWN)) {
      if (!playfield->isTouchingDown(currentTetrimino)) {
        currentTetrimino->moveDown(true);
      }
      downKeyTimer = KEY_REPEAT_DELAY; // Set initial delay
    } else {
      // Key held down - check timer
      downKeyTimer -= deltaTime;
      if (downKeyTimer <= 0.0f) {
        if (!playfield->isTouchingDown(currentTetrimino)) {
          currentTetrimino->moveDown(false);
        }
        downKeyTimer = KEY_REPEAT_RATE; // Set repeat rate
      }
    }
  } else {
    downKeyTimer = 0.0f; // Reset timer when key released
  }
}

void GameplayScene::handleLineClears(int linesCleared) {
  if (linesCleared <= 0)
    return;

  // Update total lines cleared
  totalLinesCleared += linesCleared;

  // Update score based on the number of lines cleared
  updateScore(linesCleared);

  // Check if we need to increase the level (typically every 10 lines)
  updateLevel();

  // Play sound effect based on the number of lines cleared
  // SoundManager::getInstance().playLineClearSound(linesCleared);
}

void GameplayScene::updateScore(int linesCleared) {
  int basePoints = 0;

  switch (linesCleared) {
  case 1:
    basePoints = SINGLE_LINE_SCORE; // 40 points for a single line clear
    break;
  case 2:
    basePoints = DOUBLE_LINE_SCORE; // 100 points for a double line clear
    break;
  case 3:
    basePoints = TRIPLE_LINE_SCORE; // 300 points for a triple line clear
    break;
  case 4:
    basePoints = TETRIS_LINE_SCORE; // 1200 points for a Tetris (four lines clear)
    break;
  default:
    return; // No points for invalid line clears
  }

  // Score is multiplied by the current level (official Tetris scoring)
  currentScore += basePoints * currentLevel;
}

void GameplayScene::updateLevel() {
  // Standard rule: level increases every 10 lines cleared
  int newLevel = (totalLinesCleared / 10) + 1;

  if (newLevel > currentLevel) {
    currentLevel = newLevel;
    // Optionally, you can play a sound effect or display a message when the level increases
    std::cout << "Level up! New level: " << currentLevel << std::endl;
    // SoundManager::getInstance().playLevelUpSound();
  }
}

void GameplayScene::Draw() {
  ClearBackground(BLACK);

  playfield->Draw();

  DrawText(TextFormat("Score: %ld", currentScore), 10, 20, 15, WHITE);
  DrawText(TextFormat("Level: %d", currentLevel), 10, 40, 15, WHITE);
  DrawText(TextFormat("Lines: %d", totalLinesCleared), 10, 60, 15, WHITE);
  DrawText(TextFormat("lockTimer: %02.02f", currentTetrimino->getLockTimer()), 10, 110, 15, GREEN);
  DrawText(TextFormat("fallSpeed: %02.02f", getFallSpeed()), 10, 130, 15, YELLOW);
  DrawText(TextFormat("fallTimer: %02.02f", fallTimer), 10, 150, 15, YELLOW);
  DrawText(TextFormat("deltaTime: %02.02f", GetFrameTime()), 10, 170, 15, YELLOW);
  DrawText(TextFormat("animating: %s", playfield->isAnimationRunning() ? "TRUE" : "FALSE"), 10, 200,
           15, BLUE);

  Tetrimino *ghostTetrimino = getGhostPiece().release();

  int startX = playfield->getDrawStart().x;
  int startY = playfield->getDrawStart().y;

  if (!playfield->isAnimationRunning()) {
    currentTetrimino->Draw(startX, startY, MINO_BLOCK);
    ghostTetrimino->Draw(startX, startY, MINO_GHOST);
  }

  DrawFPS(WINDOW_W - 30, 0);
}
