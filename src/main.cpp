#include "globals.h"
#include "scene_manager.h"
#include "utils.h"
#include <iostream>
#include <physfs.h>
#include <raylib.h>

using namespace std;

int main(void) {
  const int screenWidth = WINDOW_W;
  const int screenHeight = WINDOW_H;

  SetTraceLogLevel(LOG_WARNING | LOG_ERROR);

  InitWindow(screenWidth, screenHeight, "Riktris");
  InitAudioDevice(); // Initialize audio device

  SetTargetFPS(FPS); // Set our game to run at 60 frames-per-second

  int framesCounter = 0;

  // Read the zip files and make them available for data loading.
  must_init(PHYSFS_init(NULL), "PHYSFS engine");
  must_init(PHYSFS_mount("gfx.dat", NULL, 1), "gfx zip file");
  must_init(PHYSFS_mount("sfx.dat", NULL, 1), "sfx zip file");
  must_init(PHYSFS_mount("misc.dat", NULL, 1), "dat zip file");

  SceneManager &sceneManager = SceneManager::getInstance();

  std::cout << "Starting with scene: " << sceneManager.getCurrentSceneName() << std::endl;

  // Main game loop, detects window close of ESC key
  while (!WindowShouldClose()) {
    framesCounter++;
    sceneManager.Update();

    BeginDrawing();

    sceneManager.Draw();

    EndDrawing();
  }

  CloseWindow(); // Close window and OpenGL context

  return 0;
}
