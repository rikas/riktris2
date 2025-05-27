#pragma once

#include "sound_manager.h"
#include "texture_manager.h"
#include <raylib.h>
#include <string>
#include <vector>

#define NUMBER_OF_ROTATIONS 4

// Because a tetromino is basically a set of 'big pixels' that can be either on or off, it is quite
// suitable and efficient to represent it as a bitmask rather than a matrix of integers.
//
// Example for the S shape:
//
// X . . .     1 0 0 0
// X X . .  =  1 1 0 0  =  1000110001000000 (in binary)  =  0x8C40 (in hexadecimal)
// . X . .     0 1 0 0
// . . . .     0 0 0 0
//
// . X X .     0 1 1 0
// X X . .  =  1 1 0 0  =  0110110000000000 (in binary)  =  0x6C00 (in hexadecimal)
// . . . .     0 0 0 0
// . . . .     0 0 0 0
//
// I'm using the Super Rotation System (https://tetris.fandom.com/wiki/SRS) which is what is being
// used in modern Tetris games.
static int TETRIMINOS[7][NUMBER_OF_ROTATIONS] = {
    {0x4E00, 0x4640, 0x0E40, 0x4C40}, // T
    {0x6C00, 0x4620, 0x06C0, 0x8C40}, // S
    {0xC600, 0x2640, 0x0C60, 0x4C80}, // Z
    {0x0F00, 0x2222, 0x00F0, 0x4444}, // I
    {0x8E00, 0x6440, 0x0E20, 0x44C0}, // J
    {0x2E00, 0x4460, 0xE800, 0xC440}, // L
    {0x6600, 0x6600, 0x6600, 0x6600}  // O
};

// The wall kick data is always an array of 4 pairs of coordinates to replace row & col for the
// rotating tetrimino.
typedef int KickData[4][2];

// When the player attempts to rotate a tetromino, but the position it would normally occupy after
// basic rotation is obstructed, (either by the wall or floor of the playfield, or by the stack),
// the game will attempt to "kick" the tetromino into an alternative position nearby
// Wall kick tables taken from https://tetris.fandom.com/wiki/SRS
static KickData WALL_KICKS[8] = {
    {{-1, 0}, {-1, 1}, {0, -2}, {-1, -2}}, {{1, 0}, {1, -1}, {0, 2}, {1, 2}},
    {{1, 0}, {1, -1}, {0, 2}, {1, 2}},     {{-1, 0}, {-1, 1}, {0, -2}, {-1, -2}},
    {{1, 0}, {1, 1}, {0, -2}, {1, -2}},    {{-1, 0}, {-1, -1}, {0, 2}, {-1, 2}},
    {{-1, 0}, {-1, -1}, {0, 2}, {-1, 2}},  {{1, 0}, {1, -1}, {0, -2}, {1, -2}}};

// For the I tetrimino the kicks are different
static KickData WALL_KICKS_I[8] = {
    {{-2, 0}, {1, 0}, {-2, -1}, {1, 2}}, {{2, 0}, {-1, 0}, {2, 1}, {-1, -2}},
    {{-1, 0}, {2, 0}, {-1, 2}, {2, -1}}, {{1, 0}, {-2, 0}, {1, -2}, {-2, 1}},
    {{2, 0}, {-1, 0}, {2, 1}, {-1, -2}}, {{-2, 0}, {1, 0}, {-2, -1}, {1, 2}},
    {{1, 0}, {-2, 0}, {1, -2}, {-2, 1}}, {{-1, 0}, {2, 0}, {-1, 2}, {2, -1}}};

typedef enum TETRIMINO_SHAPE {
  TETRIMINO_T = 0,
  TETRIMINO_S,
  TETRIMINO_Z,
  TETRIMINO_I,
  TETRIMINO_J,
  TETRIMINO_L,
  TETRIMINO_O
} TETRIMINO_SHAPE;

typedef enum ROTATE_DIRECTION { LEFT = 0, RIGHT } ROTATE_DIRECTION;

// Tetrimino names for loading textures
const std::vector<std::string> MINO_NAMES = {"t", "s", "z", "i", "j", "l", "o"};

typedef enum MINO_DRAW_TYPE { MINO_BLOCK, MINO_GHOST } MINO_DRAW_TYPE;

class Tetrimino {
private:
  const Texture2D *minoTexture;
  const Texture2D *ghostTexture;
  TETRIMINO_SHAPE shape;
  int *rotations; // Each tetrimino has 4 rotations, represented as bitmasks
  int rotationIndex;
  int speed;
  bool locked; // This is used to check if the tetrimino is locked in place and cannot move anymore
  float lockTimer = 0.0f; // Timer for locking the tetrimino in place
  int col;
  int row;
  void playMoveSound() {
    Sound moveSfx = SoundManager::getInstance().getSound("move_new.wav");
    PlaySound(moveSfx);
  }

public:
  Tetrimino(TETRIMINO_SHAPE shape, int speed) : rotations(TETRIMINOS[shape]) {
    this->speed = speed;
    this->shape = shape;

    const std::string mino_filename = "mino_" + MINO_NAMES[shape] + ".png";
    minoTexture = &TextureManager::getInstance().getTexture(mino_filename.c_str());

    const std::string ghost_filename = "mino_ghost_" + MINO_NAMES[shape] + ".png";
    ghostTexture = &TextureManager::getInstance().getTexture(ghost_filename.c_str());

    reset();
  }

  bool isFilled(int x, int y);
  int getRotation() const { return rotations[rotationIndex]; }
  int getRotationIndex() const { return rotationIndex; }

  // Resets the tetrimino values. The default col is 3 because it's the center of the playfield.
  void reset() {
    row = 0;
    col = 3;
    locked = false;
    rotationIndex = 0;
    lockTimer = 0.0f;
  }

  ~Tetrimino() = default;
  void Draw(int offsetX, int offsetY, MINO_DRAW_TYPE draw_type);
  void moveLeft() {
    col--;
    playMoveSound();
  }
  void moveRight() {
    col++;
    playMoveSound();
  }
  void moveDown(bool withSound = false) {
    row++;
    if (withSound) {
      playMoveSound();
    }
  }
  int getCol() const { return col; }
  int getRow() const { return row; }
  void setCol(int newCol) { col = newCol; }
  void setRow(int newRow) { row = newRow; }
  void setRotation(int newRotation) {
    if (newRotation < 0) {
      newRotation = NUMBER_OF_ROTATIONS - 1;
    } else if (newRotation >= NUMBER_OF_ROTATIONS) {
      newRotation = 0;
    }
    rotationIndex = newRotation;
  }
  TETRIMINO_SHAPE getShape() const { return shape; }
  void Rotate(ROTATE_DIRECTION direction);
  bool isLocked() const { return locked; }
  void lock() {
    locked = true;
    resetLockTimer();
  }
  void resetLockTimer() { lockTimer = 0.0f; }
  void addToLockTimer(float deltaTime) { lockTimer += deltaTime; }
  float getLockTimer() const { return lockTimer; }
  bool isLocking() const { return lockTimer > 0.0f; }
};
