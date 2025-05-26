
#include "sound_manager.h"
#include <iostream>

SoundManager &SoundManager::getInstance() {
  static SoundManager instance;
  return instance;
}

const Sound &SoundManager::getSound(const std::string &filename) {
  auto it = sounds.find(filename);

  if (it == sounds.end()) {
    // Load sound if not found
    std::cout << "Loading sound: " << SOUND_DIR << filename << std::endl;

    std::string fullPath = SOUND_DIR + filename;

    Sound sound = LoadSound(fullPath.c_str());
    sounds[filename] = sound;
    return sounds[filename];
  }
  return it->second;
}

void SoundManager::preloadSound(const std::string &filename) {
  getSound(filename); // This will load it if not already loaded
}

void SoundManager::unloadSound(const std::string &filename) {
  auto it = sounds.find(filename);

  if (it != sounds.end()) {
    UnloadSound(it->second);
    sounds.erase(it);
    std::cout << "Unloaded sound: " << filename << std::endl;
  }
}

void SoundManager::unloadAll() {
  for (auto &pair : sounds) {
    UnloadSound(pair.second);
  }
  sounds.clear();
  std::cout << "Unloaded all sounds" << std::endl;
}

SoundManager::~SoundManager() { unloadAll(); }
