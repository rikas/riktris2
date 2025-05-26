#include "texture_manager.h"
#include <iostream>

TextureManager &TextureManager::getInstance() {
  static TextureManager instance;
  return instance;
}

const Texture2D &TextureManager::getTexture(const std::string &filename) {
  auto it = textures.find(filename);

  if (it == textures.end()) {
    // Load texture if not found
    std::cout << "Loading texture: " << filename << std::endl;
    Texture2D texture = LoadTexture(filename.c_str());
    textures[filename] = texture;
    return textures[filename];
  }
  return it->second;
}

void TextureManager::preloadTexture(const std::string &filename) {
  getTexture(filename); // This will load it if not already loaded
}

void TextureManager::unloadTexture(const std::string &filename) {
  auto it = textures.find(filename);

  if (it != textures.end()) {
    UnloadTexture(it->second);
    textures.erase(it);
    std::cout << "Unloaded texture: " << filename << std::endl;
  }
}

void TextureManager::unloadAll() {
  for (auto &pair : textures) {
    UnloadTexture(pair.second);
  }
  textures.clear();
  std::cout << "Unloaded all textures" << std::endl;
}

TextureManager::~TextureManager() { unloadAll(); }
