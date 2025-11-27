# PENUMBRA C++ Migration

GameMaker → Custom C++ engine migration for PENUMBRA_PROTO_03, an atmospheric isometric exploration platformer.

## Project Overview

A complete architectural redesign and rewrite of the PENUMBRA game engine from GameMaker/GML to C++, targeting macOS with OpenGL 3.3+ graphics and CMake build system.

**Key Architectural Improvements:**
- **3D World Model**: Full 3D world with orthographic isometric camera (maintains straight lines in all axes)
- **Z-Buffer Rendering**: Automatic depth sorting via OpenGL (eliminates manual depth sort bugs)
- **Discrete Collision Layers**: Per-tile collision system (fixes z-position ambiguity from GML)
- **Clean Architecture**: Dependency injection and clear API boundaries (loose coupling)

## Documentation

- **[PLAN.md](./PLAN.md)** - Complete detailed migration plan with architecture, phases, and specifications (~1600 lines)
- **[QUICK_REF.md](./QUICK_REF.md)** - Quick reference guide for busy sessions (1-page summary)

## Timeline & Status

- **Total Duration**: 4-5 months (~75-101 days optimistic)
- **Code Volume**: ~30K GML → ~40-50K C++
- **Token Budget**: ~600K total

### Phase Overview

| Phase | Duration | Focus | Status |
|-------|----------|-------|--------|
| **Phase 0** | Days 1-5 | Project setup (CMake, SDL2, OpenGL) | Pending |
| **Phase 1** | Days 6-18 | 3D rendering with z-buffer (CRITICAL) | Pending |
| **Phase 1.5** | Days 19-21 | Render order validation | Pending |
| **Phase 2** | Days 22-38 | Player physics and collision | Pending |
| **Phase 3** | Days 39-55 | Platforms and enemies | Pending |
| **Phase 4** | Days 56-70 | Save/load and content migration | Pending |
| **Phase 5** | Days 71-100+ | Polish and finalization | Pending |

## Technology Stack

- **Language**: C++17
- **Build System**: CMake 3.20+
- **Graphics**: OpenGL 3.3+ Core Profile
- **Window/Input**: SDL2
- **Math**: GLM (vectors, matrices, orthographic projection)
- **JSON**: nlohmann_json
- **Testing**: Google Test (gtest)
- **Dependency Management**: Conan 2.x

## Project Structure

```
penumbra/
├── CMakeLists.txt          # Build configuration
├── conanfile.txt           # Dependency specification
├── src/
│   ├── main.cpp
│   ├── core/               # Math, Platform, Resources
│   ├── game/               # TileGrid, Player, Platforms, Enemies
│   ├── rendering/          # Camera, Renderer, Shaders
│   └── systems/            # RoomSystem, SaveSystem, ObjectFactory
├── include/                # All header files
├── assets/
│   ├── shaders/            # GLSL vertex/fragment shaders
│   ├── sprites/            # Sprite assets
│   ├── rooms/              # Room JSON definitions
│   └── sounds/             # Audio (placeholder)
├── tests/                  # Unit and integration tests
└── build/                  # Build output (gitignored)
```

## Getting Started

### Prerequisites
- macOS 10.15+
- CMake 3.20+
- Conan 2.x
- OpenGL 3.3+ compatible GPU

### Build Instructions

```bash
cd /Users/thomaslarson/gamedev/penumbra

# Install dependencies (first time only)
conan install . --build=missing

# Configure and build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Run
./build/penumbra
```

### Run Tests

```bash
cmake --build build --target test
# or
ctest --build build
```

## Development Workflow

1. **Feature branches only**: Create feature branch for each phase/module
2. **Commit frequently**: Atomic commits with clear messages
3. **Validate before merge**: Tests must pass, no warnings

```bash
git checkout -b feature/phase-1-graphics
# ... work ...
git add .
git commit -m "Phase 1: Implement 3D grid rendering with z-buffer"
git push origin feature/phase-1-graphics
# Create pull request on GitHub
```

## Key Architectural Decisions

### Graphics Architecture
- **Isometric Projection**: 3D world with orthographic camera at fixed isometric angle
- **Z-Buffer**: OpenGL handles depth ordering (no manual sorting)
- **Sprite System**: Textured quads positioned in 3D space

### Physics & Movement
- **Grid-Based**: Tile-to-tile positioning with fractional offsets (0.0-1.0)
- **Collision Layers**: Discrete per-tile collision zones (not continuous z-value)
- **State Machine**: Player physics managed via state transitions

### Save/Load System
- **JSON Format**: Human-readable, debuggable persistence
- **Feature Parity**: All GML save files convertible to JSON

## Performance Targets

- **Render**: 60+ FPS on macOS with 22-room world
- **Memory**: < 512MB for full game with all assets
- **Load Time**: < 2 seconds per room
- **Startup**: < 5 seconds from launch to playable

## Next Steps

Ready to begin Phase 0 (project scaffolding). See [PLAN.md](./PLAN.md) for detailed implementation instructions.

## Contact & Notes

This project is in active development. For questions or issues, refer to the detailed plan or create a GitHub issue.

---

**Repository**: https://github.com/ceruleanoak/penumbra
**Last Updated**: 2025-11-27
