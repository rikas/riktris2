#include "scene_manager.h"
#include "scenes/gameplay_scene.h"
#include <iostream>

SceneManager::SceneManager() {
  // Initialize all scenes
  scenes.resize(6); // Updated size for new scenes

  // Start with LOGO scene
  sceneStack.push(GAMEPLAY_SCENE);
}

SceneManager &SceneManager::getInstance() {
  static SceneManager instance;
  return instance;
}
// Factory method - creates scenes based on ID
std::unique_ptr<GameScene> SceneManager::createScene(GameSceneId id) {
  switch (id) {
  case LOGO_SCENE:
    std::cout << "Factory creating: Logo Scene" << std::endl;
    return std::make_unique<GameplayScene>("Gameplay Scene");

  case TITLE_SCENE:
    std::cout << "Factory creating: Title Scene" << std::endl;
    return std::make_unique<GameplayScene>("Gameplay Scene");

  case GAMEPLAY_SCENE:
    std::cout << "Factory creating: Gameplay Scene" << std::endl;
    return std::make_unique<GameplayScene>("Gameplay Scene");

  case ENDING_SCENE:
    std::cout << "Factory creating: Ending Scene" << std::endl;
    return std::make_unique<GameplayScene>("Gameplay Scene");

  case OPTIONS_SCENE:
    std::cout << "Factory creating: Options Scene" << std::endl;
    return std::make_unique<GameplayScene>("Gameplay Scene");

  case PAUSE_SCENE:
    std::cout << "Factory creating: Pause Scene" << std::endl;
    return std::make_unique<GameplayScene>("Gameplay Scene");

  default:
    std::cerr << "Factory error: Unknown scene ID: " << id << std::endl;
    return nullptr;
  }
}

// Get scene with lazy initialization using factory
std::unique_ptr<GameScene> &SceneManager::getScene(GameSceneId id) {
  if (!scenes[id]) {
    scenes[id] = createScene(id);
  }
  return scenes[id];
}

// Switch to a specific scene by ID. It clears the stack and pushes the new scene.
void SceneManager::switchTo(GameSceneId id) {
  if (id >= 0 && id < scenes.size()) {
    clearStack();
    sceneStack.push(id);
    auto &scene = getScene(id);

    if (scene) {
      std::cout << "Switched to: " << scenes[id]->getName() << std::endl;
    }
  }
}

void SceneManager::pushScene(GameSceneId id) {
  if (id >= 0 && id < scenes.size()) {
    sceneStack.push(id);
    auto &scene = getScene(id);
    if (scene) {
      std::cout << "Pushed scene: " << scenes[id]->getName()
                << " (Stack size: " << sceneStack.size() << ")" << std::endl;
    }
  }
}

void SceneManager::popScene() {
  if (sceneStack.size() > 1) { // Keep at least one scene
    GameSceneId poppedScene = sceneStack.top();
    sceneStack.pop();
    auto &scene = getScene(poppedScene);
    if (scene) {
      std::cout << "Popped scene: " << scenes[poppedScene]->getName()
                << " (Stack size: " << sceneStack.size() << ")" << std::endl;
    }
  }
}

void SceneManager::clearStack() {
  while (!sceneStack.empty()) {
    sceneStack.pop();
  }
}

// Optional: Method to preload specific scenes
void SceneManager::preloadScene(GameSceneId id) {
  if (id >= 0 && id < scenes.size() && !scenes[id]) {
    std::cout << "Preloading scene: " << id << std::endl;
    scenes[id] = createScene(id);
  }
}

// Optional: Method to unload unused scenes to free memory
void SceneManager::unloadScene(GameSceneId id) {
  if (id >= 0 && id < scenes.size() && scenes[id]) {
    // Make sure scene isn't currently in the stack
    std::stack<GameSceneId> tempStack = sceneStack;
    bool inUse = false;

    while (!tempStack.empty()) {
      if (tempStack.top() == id) {
        inUse = true;
        break;
      }
      tempStack.pop();
    }

    if (!inUse) {
      std::cout << "Unloading scene: " << scenes[id]->getName() << std::endl;
      scenes[id].reset(); // Destroy the scene
    } else {
      std::cout << "Cannot unload scene " << id << " - currently in use" << std::endl;
    }
  }
}

void SceneManager::Update() {
  // Only update the top scene (the active one)
  if (!sceneStack.empty()) {
    GameSceneId currentSceneId = sceneStack.top();
    if (currentSceneId < scenes.size()) {
      auto &scene = getScene(currentSceneId);
      if (scene) {
        scene->Update();
      }
    }
  }
}

void SceneManager::Draw() {
  if (sceneStack.empty())
    return;

  // Create a temporary stack to iterate from bottom to top
  std::stack<GameSceneId> tempStack;
  std::stack<GameSceneId> originalStack = sceneStack; // Copy the stack

  // Move all elements to temp stack (reverses order)
  while (!originalStack.empty()) {
    tempStack.push(originalStack.top());
    originalStack.pop();
  }

  // Render from bottom to top
  while (!tempStack.empty()) {
    GameSceneId scene = tempStack.top();
    tempStack.pop();

    if (scene < scenes.size()) {
      scenes[scene]->Draw();
    }
  }
}

GameSceneId SceneManager::getTopSceneId() const {
  return sceneStack.empty() ? LOGO_SCENE : sceneStack.top();
}

bool SceneManager::hasActiveScenes() const { return !sceneStack.empty(); }

size_t SceneManager::getStackSize() const { return sceneStack.size(); }

std::string SceneManager::getCurrentSceneName() const {
  GameSceneId topId = getTopSceneId();
  auto &nonConstThis = const_cast<SceneManager &>(*this);
  auto &scene = nonConstThis.getScene(topId);
  return scene->getName();
}
