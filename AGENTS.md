# AGENTS.md

## Build

```bash
cmake -B build && cmake --build build
```

Requires `raylib` and `physfs` installed via Homebrew (paths are hardcoded to `/opt/homebrew/`). This project is **macOS-only**.

Run from the repo root (so `.dat` asset archives are found):

```bash
./build/raytris
```

## No tests, no CI

There is no test suite and no CI. Manual build + run is the only verification.

## Formatting

`.clang-format` sets `ColumnLimit: 100` (not the typical 80 or 120).

```bash
clang-format -i src/**/*.cpp src/**/*.h
```

## Asset loading

Assets are **not** loose files at runtime. They are zipped into `gfx.dat`, `sfx.dat`, and `misc.dat` at the repo root and mounted via PhysFS. Internal paths follow the pattern `data/gfx/playfield.png`. Always run the binary from the repo root.

## Architecture

- **Scene system:** Stack-based singleton `SceneManager`. Scenes are identified by `GameSceneId` enum. Only `GAMEPLAY_SCENE` is implemented; others (`LOGO`, `TITLE`, `ENDING`, `OPTIONS`, `PAUSE`) are declared but empty.
- **Scene interface:** Each scene inherits `GameScene` and implements `Update()` / `Draw()`.
- **`must_init` pattern:** `utils.h` — call this for any critical init; it aborts on failure.
- **imgui submodule** (`imgui/`) exists but is **not wired into the build** yet.

## Key files

| File | Purpose |
|------|---------|
| `src/main.cpp` | Entrypoint |
| `src/globals.h` | Compile-time constants (FPS=60, window size 600×600) |
| `src/scene_manager.{h,cpp}` | Scene stack routing |
| `src/gameplay_scene.{h,cpp}` | Main (only) gameplay scene |
| `src/playfield.{h,cpp}` | Tetris board + animation state |
| `src/mino_grid.{h,cpp}` | Grid data structure for placed minos |
| `src/tetrimino_manager.{h,cpp}` | Active piece logic |
