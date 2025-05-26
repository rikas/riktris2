#pragma once

#include <raylib.h>
#include <string>
#include <unordered_map>

const std::string SOUND_DIR = "data/sfx/";

class SoundManager {
private:
  std::unordered_map<std::string, Sound> sounds;

  SoundManager() = default;

public:
  static SoundManager &getInstance();

  // Load sound if not already loaded, return reference to cached sound
  const Sound &getSound(const std::string &filename);

  // Preload sound
  void preloadSound(const std::string &filename);

  // Unload specific sound
  void unloadSound(const std::string &filename);

  // Unload all sounds
  void unloadAll();

  ~SoundManager();
};
