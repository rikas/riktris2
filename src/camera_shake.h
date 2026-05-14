#pragma once

#include <raylib.h>

class CameraShake {
private:
  bool active;     // Is the camera shake active
  float intensity; // Intensity of the shake
  float duration;  // Duration of the shake
  float elapsed;   // Time elapsed since the shake started
  Vector2 offset;  // Original camera offset before shake
public:
  CameraShake() : active(false), intensity(0.0f), duration(0.0f), elapsed(0.0f) {}

  void startShake(float shakeIntensity, float shakeDuration) {
    intensity = shakeIntensity;
    duration = shakeDuration;
    elapsed = 0.0f;
    active = true;
    offset = {0, 0};
  }

  void update() {
    if (!active) {
      return;
    }

    elapsed += GetFrameTime();

    if (elapsed >= duration) {
      stopShake();
      return;
    }

    // Calculate the current intensity that will go down over time
    float progress = elapsed / duration;
    float currentIntensity = intensity * (1.0f - progress);

    // Generate random offset for shake effect
    offset.x = (GetRandomValue(-100, 100) / 100.0f) * currentIntensity;
    offset.y = (GetRandomValue(-100, 100) / 100.0f) * currentIntensity;
  }

  void stopShake() {
    active = false;
    intensity = 0.0f;
    duration = 0.0f;
    elapsed = 0.0f;
    offset = {0, 0};
  }

  void applyToCamera(Camera2D &camera, Vector2 baseOffset) {
    if (active) {
      // camera.offset.x = baseOffset.x + offset.x;
      camera.offset.y = baseOffset.y + offset.y;
    } else {
      // Reset camera offset when shake is not active
      camera.offset = {0, 0};
    }
  }

  Vector2 getOffset() const { return offset; }
  bool isActive() const { return active; }
};
