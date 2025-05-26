#pragma once

#include <raylib.h>
#include <string>
#include <unordered_map>

const std::string TEXTURES_DIR = "data/gfx/";

class TextureManager {
private:
  std::unordered_map<std::string, Texture2D> textures;

  TextureManager() = default;

public:
  static TextureManager &getInstance();

  // Load texture if not already loaded, return reference to cached texture
  const Texture2D &getTexture(const std::string &filename);

  // Preload texture
  void preloadTexture(const std::string &filename);

  // Unload specific texture
  void unloadTexture(const std::string &filename);

  // Unload all textures
  void unloadAll();

  ~TextureManager();
};
