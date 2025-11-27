# PENUMBRA C++ Migration - Quick Reference

## Core Decision
Migrate PENUMBRA_PROTO_03 (GameMaker) → Custom C++ engine (macOS, OpenGL 3.3+)

**Key Fix:** Replace z-position float with discrete collision layers (solves GML render order issues)

**Architecture:** Full 3D world + isometric camera (orthographic) → OpenGL z-buffer handles depth automatically

---

## Critical Stats
- **Timeline:** 4-5 months (75-101 days optimistic)
- **Code Volume:** ~30K GML → ~40-50K C++
- **Token Budget:** ~600K total
- **Target:** Phase 1 (render pipeline) by day 18
- **Success Metric:** 4x4 grid rendering correctly with z-buffer, no manual sorting

---

## 6 Agents (Ready to Deploy)

| Agent | Budget | Role | Phases |
|-------|--------|------|--------|
| **Boilerplate** | 30-40K | Project setup (one-time) | 0 |
| **Graphics** | 60-100K | 3D rendering pipeline | 1-2 |
| **Physics** | 50-70K | Movement + collision | 2-3 |
| **GML Analyzer** | 50-80K each | Extract logic (7 modules) | 1-4 |
| **Content Migration** | 40-60K | Convert rooms/sprites/enemies | 3-4 |
| **Testing** | 30-50K each | Unit/integration tests | After each phase |

---

## 5 Implementation Phases

### Phase 0: Setup (Days 1-5)
- CMake + SDL2 + OpenGL context
- Math library (vectors, matrices, projection)
- Shader pipeline
- **Deliverable:** Blank window with working renderer

### Phase 1: 3D Grid Rendering (Days 6-18) **CRITICAL**
- TileGrid (3D sparse storage)
- IsometricCamera (orthographic projection)
- Sprite quads (positioned in 3D world)
- Z-buffer depth testing
- **Deliverable:** 4x4 grid of tiles at different (x,y,z) rendering correctly

### Phase 1.5: Validation (Days 19-21)
- Overlapping sprites test
- No z-fighting
- Render order correct
- **Deliverable:** Confidence z-buffer solves GML render issues

### Phase 2: Physics (Days 22-38)
- PlayerPhysics state machine
- Grid-based movement (acceleration/deceleration)
- Collision detection (3D point queries)
- Jumping + falling
- **Deliverable:** Playable character in 3D space

### Phase 3: Platforms + Enemies (Days 39-55)
- Platform system (static + moving, path following)
- Enemy spawning + basic AI
- Player carrying on platforms
- **Deliverable:** Functional platforming

### Phase 4: Persistence (Days 56-70)
- SaveSystem (JSON format)
- RoomSystem (room definitions + transitions)
- ObjectFactory (dynamic spawning)
- All 22 rooms converted
- **Deliverable:** Can save/load, move between rooms

### Phase 5: Polish (Days 71-100+)
- Camera modes (follow, tile-center, overview)
- Tile system (all types)
- UI/Menu (minimal)
- Audio stub

---

## Critical GML Files to Study

1. `scr_grid.gml` - Room3D constructor (CRITICAL)
2. `Room3DCameraSystem.gml` - Projection math
3. `Room3DRendering.gml` - Vertex buffer rendering
4. `obj_player/Create_0.gml` - Player physics
5. `Room3DPlatforms.gml` - Platform collision

---

## Architecture Summary

```
Application Layer (Game loop, input)
    ↓
Game Systems (Player, platforms, enemies, cameras)
    ↓
Physics & Collision (TileGrid layers, detection, gravity)
    ↓
Rendering (OpenGL, vertex buffers, sprites)
    ↓
Platform (SDL2, OpenGL context, file I/O)
```

---

## Z-Position Fix (Core Innovation)

**GML Problem:** Single `z_position` float → ambiguous collisions

**C++ Solution:** Discrete layers per tile
```cpp
struct TileCollider {
    struct Layer {
        float top_height;      // Surface top
        float bottom_height;   // Surface bottom
        TileType type;         // Platform, hazard, etc
    };
    std::vector<Layer> layers;  // Stacked zones
};
```

**Benefits:**
- Clear collision boundaries
- No floating-point precision issues
- Debuggable (visual layer display)
- Supports multi-platform scenarios

---

## Rendering Architecture (3D Not 2D)

**Key Insight:** 3D world + orthographic camera = isometric view

- Tiles are 3D positions: `Vector3 world_position`
- Sprites rendered as textured quads in 3D space
- Camera positioned at isometric angle (~45° yaw, ~30° pitch)
- OpenGL z-buffer handles render order automatically (no manual sorting)
- Orthographic projection maintains straight lines in all axes

---

## Success Criteria (Minimal)

✅ Phase 1: 4x4 grid renders with correct depth ordering (no manual sort)
✅ Phase 2: Player moves smoothly through 3D space with reliable collision
✅ Phase 3: Platforms work, player carries correctly
✅ Phase 4: Save/load preserves state, room transitions work
✅ Final: All 22 GML rooms run identically in C++

---

## Quick Start Commands

```bash
# Phase 0: Initialize project
/agent:boilerplate --init --project-name penumbra-cpp --target-platform macos

# Phase 1: Graphics (parallel)
/agent:graphics --phase 1 --deliverable math
/agent:graphics --phase 1 --deliverable opengl-context
/agent:graphics --phase 1 --deliverable vao-vbo
/agent:graphics --phase 1 --deliverable shaders
/agent:graphics --phase 1 --deliverable sprite-rendering
/agent:gml-analyzer --module scr_grid --context-file [path]
/agent:gml-analyzer --module Room3DRendering --context-file [path]
/agent:testing --phase 1 --system rendering

# Phase 2: Physics (parallel)
/agent:physics --phase 2 --system tilegrid
/agent:physics --phase 2 --system player-physics
/agent:physics --phase 2 --system collision
/agent:gml-analyzer --module obj_player --context-file [path]
/agent:testing --phase 2 --system physics
```

---

## Key Files Created/Modified

### Source Structure
```
penumbra-cpp/
├── src/
│   ├── main.cpp
│   ├── core/ (Math, Platform, Resources)
│   ├── game/ (TileGrid, Player, Platform, Enemy, StateManager)
│   ├── rendering/ (Camera, Renderer, Shaders)
│   └── systems/ (RoomSystem, SaveSystem, ObjectFactory)
├── include/ (All headers)
├── assets/ (shaders/, sprites/, rooms/, sounds/)
├── CMakeLists.txt
├── conanfile.txt
└── tests/
```

### Content Files (Generated Phase 4)
```
assets/rooms/Floor1_A.json through Floor1_O.json
assets/sprites/catalog.json
assets/enemy_types.json
assets/tile_types.json
```

---

## Token Efficiency Strategy

**Per-Phase Budgets:**
- Phase 0: 30-40K (setup)
- Phase 1: 120-150K (graphics + GML analyzer + testing)
- Phase 2: 130-160K (physics + GML analyzer + testing)
- Phase 3: 120-150K (platforms + content prep + testing)
- Phase 4: 90-120K (content migration + testing)
- Phase 5: 60-80K (polish)

**Parallelization:** While one agent builds, others analyze/test
- Graphics builds Phase 1 → GML Analyzer studies internals
- Physics builds Phase 2 → Content Migration converts rooms
- Testing runs after each component completes

---

## Dependencies & Blocking Points

1. **Phase 0 → Phase 1:** Boilerplate must complete
2. **Phase 1 → Phase 2:** Rendering validated
3. **Phase 2 → Phase 3:** Player movement stable
4. **Phase 3 → Phase 4:** Gameplay complete

**Parallel Allowed:** GML analysis, testing, content conversion (don't block phases)

---

## Reference Plan
Full detailed plan: `/Users/thomaslarson/.claude/plans/cozy-bubbling-salamander.md`

Quick reference (this file): `/Users/thomaslarson/.claude/plans/PENUMBRA_QUICK_REF.md`

Agent definitions: `~/.claude/agents/*.md` (6 files)

---

## Current Status
✅ Plan approved
✅ Agents defined (6 ready)
✅ Todo list active (22 items)
⏳ Ready for Phase 0 execution
