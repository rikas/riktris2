#pragma once

#include <string>

class GameScene {
private:
  std::string name;

public:
  explicit GameScene(const std::string &sceneName) : name(sceneName) {}
  virtual ~GameScene() = default;
  const std::string &getName() const { return name; }
  virtual void Update() = 0;
  virtual void Draw() = 0;
};
