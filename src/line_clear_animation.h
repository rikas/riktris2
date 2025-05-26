#pragma once

#include <raylib.h>
#include <vector>

enum class AnimationState { NONE, FLASHING, CLEARING, DROPPING };

struct LineClearAnimation {
  std::vector<int> rowsToClear;                // Rows that need to be cleared
  AnimationState state = AnimationState::NONE; // Current state of the animation
  float timer = 0.0f;                          // Timer for animation progress
  float flashDuration = 0.3f;                  // How long lines flash before clearing
  float clearDuration = 0.2f;                  // How long clearing animation takes
  float dropDuration = 0.3f;                   // How long drop animation takes
  int flashCount = 0;                          // Current flash cycle
  int maxFlashes = 6;                          // Total number of flashes (3 on/off cycles)
  bool isActive = false;                       // Is the animation currently running
};
