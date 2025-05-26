#pragma once

#include "scenes/game_scene.h"
#include <memory>
#include <stack>
#include <string>
#include <vector>

typedef enum GameSceneId {
  LOGO_SCENE = 0,
  TITLE_SCENE,
  GAMEPLAY_SCENE,
  ENDING_SCENE,
  OPTIONS_SCENE,
  PAUSE_SCENE
} GameSceneId;

class SceneManager {
private:
  std::vector<std::unique_ptr<GameScene>> scenes;
  std::stack<GameSceneId> sceneStack;

  // Private constructor for singleton
  SceneManager();

  // Delete copy constructor and assignment operator
  SceneManager(const SceneManager &) = delete;
  SceneManager &operator=(const SceneManager &) = delete;

public:
  // Singleton access method
  static SceneManager &getInstance();

  ~SceneManager() = default;

  // Basic scene management
  void switchTo(GameSceneId id);

  // Stack-based scene management
  void pushScene(GameSceneId id);
  void popScene();
  void clearStack();

  void Update();
  void Draw();

  GameSceneId getTopSceneId() const;
  bool hasActiveScenes() const;
  size_t getStackSize() const;
  std::unique_ptr<GameScene> &getScene(GameSceneId id);
  std::string getCurrentSceneName() const;
  std::unique_ptr<GameScene> createScene(GameSceneId id);
  void unloadScene(GameSceneId id);
  void preloadScene(GameSceneId id);
};
