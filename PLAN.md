# PENUMBRA Migration Plan: GameMaker → Custom C++ Engine

## Executive Summary

Migrate PENUMBRA_PROTO_03 from GameMaker GML (~30K lines) to a custom C++ engine targeting macOS with OpenGL 3.3+. Focus on **architectural improvements**, specifically fixing the known z-position/vertical collision inconsistency that plagues the current GML implementation. Complete rewrite approach with no parallel GameMaker maintenance.

**Estimated Duration:** 4-6 months (flexible timeline)
**Target Platform:** macOS (expandable to Windows/Linux later)
**Build System:** CMake + Conan for dependencies
**Key Architectural Change:** Fix verticality with discrete platform layers instead of continuous z-values

---

## Core Problem to Address: Z-Position Vertical Collision Architecture

### Current Implementation Issue (GML)
- Verticality managed via single `z_position` float value
- No clear collision boundaries at different heights
- Tile-based horizontal collision works, but vertical collisions are ambiguous
- Results in inconsistent landing detection and edge-case collisions with platforms

### Proposed C++ Solution: Discrete Platform Layer System

Instead of a continuous z-position, use a **layer-based collision model**:

```cpp
struct TileCollider {
    // Height ranges for different collision zones
    struct Layer {
        float top_height;      // Z-position of this layer's top surface
        float bottom_height;   // Z-position of this layer's bottom
        TileType type;         // What kind of surface (platform, hazard, etc)
    };

    std::vector<Layer> layers;  // Stacked collision zones per tile

    // Query API
    Layer* GetCollisionAt(float z_position);
    bool CanLandOn(const Layer& layer, float z_velocity);
    float GetLandingHeight(float z_position);
};

struct PlayerState {
    // Current layer the player is standing on
    TileCollider::Layer* current_layer;

    // Continuous Z value relative to current layer
    float z_above_surface;

    // Discrete layer for collision queries
    int standing_on_layer_index;
};
```

**Benefits:**
- Clear, predictable collision boundaries
- Easy to debug (visual layer display in editor)
- Supports multi-platform scenarios (multiple layers per tile)
- No floating-point precision issues with vertical alignment
- Natural fit with tile grid system

---

## Architecture Overview

### Four-Layer Architecture

```
┌─────────────────────────────────┐
│  Application Layer              │  Game loop, input, update cycle
├─────────────────────────────────┤
│  Game Systems Layer             │  Player, platforms, enemies, cameras
├─────────────────────────────────┤
│  Physics & Collision Layer      │  Tile layers, collision detection, gravity
├─────────────────────────────────┤
│  Rendering Layer                │  OpenGL, vertex buffers, isometric projection
├─────────────────────────────────┤
│  Platform Layer                 │  SDL2, OpenGL context, file I/O
└─────────────────────────────────┘
```

### Core Modules (Dependency Order)

**Phase 0: Foundation Infrastructure**
1. `Math` - Isometric projection, vector/matrix operations, easing functions
2. `Platform` - SDL2 window/event handling, OpenGL context management
3. `Resources` - Asset loading (textures, sprites), memory management
4. `Debug` - Visualization, console output, profiling

**Phase 1: World System (Foundation)**
1. `TileGrid` - 2D tile array with layered collision system
2. `Camera` - Isometric projection, smooth following, rotation
3. `Rendering` - Vertex buffers, face culling, depth sorting
4. `Collision` - Layer-based hit detection, platform queries

**Phase 2: Game Objects & Physics**
1. `StateManager` - C++ state machine equivalent to SnowState
2. `Physics` - Gravity, velocity, platform-relative movement
3. `Player` - Movement, collision, interaction
4. `Platform` - Path following, player carrying, physics
5. `Enemy` - Type system, behavior patterns, AI

**Phase 3: Content & Persistence**
1. `RoomSystem` - Room definitions, exit/entrance linking
2. `ObjectFactory` - Dynamic object spawning, lifecycle management
3. `SaveSystem` - JSON serialization, world state persistence
4. `TileTypeSystem` - Extensible tile behavior definitions

**Phase 4: Polish & Integration**
1. `Camera` - Full implementation with all modes
2. `UI/Menu` - Minimal implementation (expandable)
3. `AudioManager` - Stub for minimal audio support
4. `Editor` - In-game level editor (deferred)

---

## Detailed System Specifications

### 1. TileGrid System (3D Foundation)

```cpp
class TileGrid {
    // 3D sparse grid (not a 2D array - tiles have explicit positions)
    static const int GRID_WIDTH = 16;
    static const int GRID_HEIGHT = 16;
    static const int GRID_DEPTH = 8;   // Stack height (z-axis levels)
    static const float TILE_SIZE = 48.0f;

    struct Tile {
        Vector3 world_position;         // 3D world position (x, y, z)
        Vector3 position_in_grid;       // Grid coordinates (grid_x, grid_y, grid_z)

        TileType* type;                 // Behavior definition (platform, hazard, etc.)
        Sprite* sprite;                 // Visual representation
        AnimationState animation;       // Frame + timing

        // Collision layers (for tile-stacking scenarios)
        std::vector<CollisionLayer> layers;
    };

    // Sparse storage - only occupied tiles, accessed by (x, y, z) key
    std::map<Vector3, Tile> tiles;

    // Core API
    Tile* GetTile(int grid_x, int grid_y, int grid_z);
    Tile* GetTileAt(Vector3 world_pos);
    std::vector<Tile*> GetTilesInRegion(Vector3 center, float radius);

    // For rendering
    std::vector<Tile*> GetVisibleTiles(const Camera& cam);

    // For physics
    CollisionLayer* GetCollisionLayerAt(Vector3 world_pos);
    bool IsBlocked(int grid_x, int grid_y, int grid_z);

    // State machine for room loading/unloading
    StateManager state;  // loading -> ready -> transitioning -> unloading
};
```

**Key Differences from GML 2D Approach:**
- **3D positions** - Each tile has (x, y, z) world coordinates
- **Sparse storage** - Only occupied tiles stored (efficient for large worlds)
- **No face geometry** - Single sprite per tile (not multiple faces)
- **Automatic depth** - OpenGL z-buffer handles render order, no sorting needed
- **Simpler collision** - Query point at world_pos instead of calculating face intersections

**Key Functions:**
- `PlaceTile(Vector3 pos, TileType type, Sprite sprite)` - Add tile to grid
- `RemoveTile(Vector3 pos)` - Remove tile
- `SetTileType(Vector3 pos, TileType type)` - Change tile behavior
- `GetTileAt(Vector3 pos)` - Efficient lookup

### 2. 3D Isometric Rendering Pipeline (CRITICAL PRIORITY)

**Architecture: Full 3D World with Isometric Camera**

Instead of 2D isometric projection with manual depth sorting, use proper 3D rendering:
- World is fully 3D: each tile occupies space at (tile_x, tile_y, tile_z)
- Sprites are textured quads (planes) positioned in 3D world
- Camera positioned at isometric angle for desired perspective
- OpenGL depth testing handles render order automatically via z-buffer

**Isometric Camera Setup:**
```cpp
class IsometricCamera {
    Vector3 position;      // Positioned above and to side for isometric view
    Vector3 target;        // Look-at center of world

    // Typical isometric setup:
    // Position camera at ~45° yaw, ~30° pitch relative to grid center
    // Creates "straight lines in all 3 axes" effect

    Matrix4 view;          // Standard 3D view matrix
    Matrix4 projection;    // Standard orthographic (not perspective)

    // For perfect isometric:
    // - Orthographic projection (parallel lines stay parallel)
    // - Camera angle creates isometric lines
    // - No perspective distortion (unlike perspective projection)
};
```

**Sprite Rendering (Key Innovation):**
```cpp
struct TileSprite {
    Vector3 world_position;    // 3D position in world
    Sprite* sprite_texture;
    Vector2 uv_min, uv_max;

    // Rendered as a textured quad (2 triangles)
    // All 4 vertices at same world position, facing camera
    // Size: 48x48 units (tile size)
    // Depth: automatically sorted by OpenGL z-buffer
};

// Render loop:
// 1. Clear color + depth buffer
// 2. Set 3D projection (orthographic)
// 3. For each drawable object in scene:
//    - Create quad at world position
//    - Bind sprite texture
//    - Draw (OpenGL handles depth sorting)
// 4. Render UI layer (2D overlay)
```

**Vertex Layout for Sprite Quads:**
```cpp
struct SpriteVertex {
    Vector3 position;      // 3D world position
    Vector2 uv;            // Texture coordinates
    Vector4 color;         // Tint color
};

// For a 48-unit tile sprite at position (5, 4, 0):
vertices[0] = {pos: (5, 4, 0), uv: (0, 0)};    // Bottom-left
vertices[1] = {pos: (5, 4, 0), uv: (1, 0)};    // Bottom-right
vertices[2] = {pos: (5, 4, 0), uv: (0, 1)};    // Top-left
vertices[3] = {pos: (5, 4, 0), uv: (1, 1)};    // Top-right

// All positioned at same world point (sprites are billboards in isometric view)
// Actual screen size determined by orthographic projection + camera distance
```

**Rendering Advantages Over 2D:**
1. **Automatic depth sorting** - z-buffer handles render order, no manual sorting needed
2. **Proper 3D collision** - Can use raycasting, bounding boxes in 3D space
3. **Camera flexibility** - Can pan, rotate, zoom without projection formula complexity
4. **Extensibility** - Easy to add actual 3D geometry later (particle effects, etc.)
5. **Straight lines in all axes** - Orthographic projection maintains geometry exactly

**Rendering Sequence:**
1. Clear color and depth buffers
2. Set view matrix (camera position)
3. Set orthographic projection
4. Update animations for visible sprites
5. Render all tile sprites (sorted by world position in rendering order)
6. Render all object sprites (players, enemies, platforms)
7. Render UI layer (switched to 2D projection)

### 3. Player Movement Physics

```cpp
struct PlayerPhysics {
    // Grid-based positioning
    struct Position {
        int tile_x, tile_y;              // Grid coordinates
        float offset_w, offset_h;        // 0.0-1.0 within tile
        float z_above_surface;           // Height above current platform
    } position;

    // Velocity
    Vector2 horizontal_velocity;         // Tile units per frame
    float vertical_velocity;             // Z units per frame

    // Physics constants
    const float ACCELERATION = 0.08f;    // Tile units/frame/frame
    const float DECELERATION = 0.85f;   // Factor per frame
    const float MAX_SPEED = 0.03f;       // Tiles per frame
    const float GRAVITY = 0.15f;         // Z units/frame/frame
    const float MAX_FALL_SPEED = 1.0f;
    const float JUMP_SPEED = 0.10f;

    // Collision state
    struct CollisionState {
        TileCollider::Layer* standing_on;
        int standing_on_layer;
        bool is_on_ground;
        bool is_in_wall;
    } collision;

    // State machine
    StateManager state;  // idle -> moving -> jumping -> falling -> crashed
};
```

**Collision Detection Process:**
1. **Horizontal Collision:** Check tile_x ± 1, tile_y ± 1 for walls
2. **Vertical Collision:** Query current tile's collision layers at z_position
3. **Platform Landing:** Test if z_velocity > 0 and z_above_surface <= 0
4. **Edge Sliding:** Allow partial movement if only one axis blocked

### 4. Camera System

```cpp
class IsometricCamera {
    struct State {
        Vector3 position;           // World position
        Vector3 target;             // Look-at point
        Vector3 up = {0, 0, 1};     // Z is up in isometric view

        float current_angle;        // 0/90/180/270
        float target_angle;
        float rotation_speed = 0.1f;

        // Camera follow modes
        enum Mode { PLAYER_FOLLOW, TILE_CENTERED, ROOM_OVERVIEW };
        Mode current_mode;

        // Zoom level
        float height = 10.0f;       // Distance from center
    };

    // Isometric projection
    Matrix4 GetProjectionMatrix(float aspect_ratio);
    Vector2 ProjectPoint(Vector3 world_pos);

    // Smooth following
    void UpdateFollowPlayer(Player& player, float dt);
    void UpdateRotation(float dt);
};
```

**Angle-Based Face Culling:**
```cpp
// Based on camera angle, determine which faces are visible
bool IsFaceVisible(FaceDirection face, float camera_angle) {
    // camera_angle: 0° = looking at +Y, 90° = +X, etc
    // Determine which 2 faces of the 4 are visible
    // Uses lookup table for efficiency
}
```

### 5. State Machine Framework (SnowState Equivalent)

```cpp
class StateManager {
    struct StateDefinition {
        std::string name;
        std::function<void()> on_enter;
        std::function<void(float dt)> on_update;
        std::function<void()> on_exit;
    };

    std::map<std::string, StateDefinition> states;
    std::string current_state;
    StateDefinition* current_def;

    // API matching SnowState behavior
    void Define(const std::string& name, StateDefinition def);
    void Change(const std::string& new_state);
    std::string GetCurrent() const;

    // Safety features
    bool CanTransition(const std::string& from, const std::string& to);
    void DetectDeadlocks();  // Warn if state can't be exited
};
```

### 6. Save/Load System (JSON-based)

Replace INI format with JSON for clarity:

```json
{
  "version": 1,
  "player": {
    "tile_x": 5,
    "tile_y": 5,
    "z_above_surface": 0.0,
    "state": "idle"
  },
  "rooms": {
    "Floor1_A": {
      "tiles": [
        {"x": 3, "y": 4, "height": 0.0, "type": "platform"},
        {"x": 4, "y": 4, "height": 0.5, "type": "slope"}
      ],
      "objects": [
        {"type": "platform", "x": 5, "y": 5, "path": [...]}
      ]
    }
  }
}
```

---

## Migration Phases (GRAPHICS-FIRST PRIORITY)

### Phase 0: Project Setup & Graphics Foundation (Days 1-5)
- Create C++ project structure with CMake
- Set up SDL2 + OpenGL 3.3+ context
- Implement basic window, event loop, render loop
- Create math library (vectors, matrices, orthographic projection)
- Implement shader system (basic textured quad shader)
- **Deliverable:** Blank OpenGL window with working input and shader pipeline

### Phase 1: 3D Grid Rendering with Sprites (Days 6-18) **CRITICAL - FIRST PROTOTYPE**
- Implement TileGrid data structure (3D positions, not 2D array)
- Implement isometric Camera system (orthographic projection, positioned for isometric view)
- Create vertex buffer system for sprite quads
- Implement sprite loading and texture binding (convert GameMaker sprites)
- Render test: 4x4 grid of sprite tiles at different (x, y, z) positions
- Implement basic depth testing via z-buffer
- **Deliverable:** A 3D grid with sprite quads rendering correctly, no manual depth sorting needed
- **Validation:** Verify sprites render in correct order based on OpenGL depth buffer, straight lines in all axes

### Phase 1.5: Verify Render Order (Days 19-21)
- Test overlapping sprites (multiple z-values at same x, y)
- Test diagonal movement (changing x, y, z simultaneously)
- Verify no "z-fighting" or incorrect render order
- Confirm GML version's depth-sorting issues are eliminated
- **Deliverable:** Confidence that 3D approach solves render order problems

### Phase 2: Player & Movement (Days 22-38)
- Implement PlayerPhysics state machine (3D aware)
- Implement tile-to-tile movement with acceleration/deceleration
- Implement collision detection (3D point queries)
- Implement jumping and falling with layer-aware landing
- Implement camera following (still isometric)
- **Deliverable:** Playable character moving through 3D test grid

### Phase 3: Platforms & Enemies (Days 39-55)
- Implement Platform system with path following (3D paths)
- Implement player carrying (moving with platforms in 3D space)
- Implement Enemy spawning and basic AI
- Implement type-specific behaviors (stationary, pusher, bouncer)
- **Deliverable:** Rooms with platforms, enemies, functional platforming

### Phase 4: Save/Load & Content (Days 56-70)
- Implement SaveSystem (JSON serialization, includes z-positions)
- Implement RoomSystem (room definitions, exit linking)
- Implement ObjectFactory (dynamic spawning)
- Load all room definitions from GML source (convert to 3D coordinates)
- **Deliverable:** Persistence works, can transition between rooms

### Phase 5: Polish & Features (Days 71-100+)
- Verify camera angles and isometric perspective for all viewing modes
- Implement Tile system (all tile types, behaviors)
- Implement basic UI/Menu system (2D overlay on 3D world)
- Implement audio stub (for future expansion)
- In-game editor (deferred - can be added once core systems stable)

---

## Technical Implementation Guidelines

### Verticality/Collision - The Z-Position Fix (CRITICAL)

**GML Problem:** Single z_position float creates ambiguity

**C++ Solution:** Discrete layers per tile

```cpp
// Example: Multi-level tile
TileGrid tile(3, 4);
tile.layers.push_back({
    top_height: 1.0f,
    bottom_height: 0.0f,
    type: PLATFORM
});
tile.layers.push_back({
    top_height: 2.0f,
    bottom_height: 1.5f,
    type: HAZARD_SPIKE
});

// Player collision query
Player p;
auto landing_layer = tile.GetCollisionLayerAt(1.5f);  // Returns hazard spike layer
p.TakeDamage();  // Explicit behavior
```

### Rendering Math

```cpp
// Isometric projection (camera angle = 0)
float screen_x = (world_x - world_y) * 48 * cos(0°);     // = (x - y) * 48
float screen_y = (world_x + world_y) * 24 * sin(0°) - z; // = -(x + y) * 24 - z

// Vertex for top face of tile at (3, 4) with height 0
vertices[0] = {screen_x, screen_y}            // NW corner
vertices[1] = {screen_x + 48, screen_y}       // NE corner
vertices[2] = {screen_x + 48, screen_y - 24}  // SE corner
vertices[3] = {screen_x, screen_y - 24}       // SW corner
```

### State Machine Patterns

```cpp
// Room loading
game_state.Define("loading_room", {
    .on_enter = [&]() { room.StartLoading(); },
    .on_update = [&](float dt) {
        if (room.IsLoaded()) game_state.Change("playing");
    }
});

game_state.Define("playing", {
    .on_enter = [&]() { room.SpawnObjects(); },
    .on_update = [&](float dt) { room.Update(dt); }
});
```

### Dependency Management (Conan)

```
conanfile.txt:
[requires]
SDL2/2.28.0
glm/0.9.9.8
nlohmann_json/3.11.2

[generators]
CMakeDeps
CMakeToolchain
```

### File Structure

```
penumbra-cpp/
├── CMakeLists.txt
├── conanfile.txt
├── src/
│   ├── main.cpp
│   ├── core/
│   │   ├── Math.h/cpp (projection, vectors)
│   │   ├── Platform.h/cpp (SDL2, OpenGL)
│   │   └── Resources.h/cpp (asset loading)
│   ├── game/
│   │   ├── TileGrid.h/cpp
│   │   ├── Player.h/cpp
│   │   ├── Platform.h/cpp
│   │   ├── Enemy.h/cpp
│   │   └── StateManager.h/cpp
│   ├── rendering/
│   │   ├── Camera.h/cpp
│   │   ├── Renderer.h/cpp (vertex buffers)
│   │   └── Shaders.h/cpp
│   └── systems/
│       ├── RoomSystem.h/cpp
│       ├── SaveSystem.h/cpp
│       └── ObjectFactory.h/cpp
├── assets/
│   ├── sprites/
│   ├── rooms/
│   └── sounds/
└── build/
```

---

## Key Architectural Improvements Over GML

| Issue | GML Solution | C++ Improvement |
|-------|--------------|-----------------|
| Z-position ambiguity | Single float | Discrete collision layers per tile |
| Coupling | Every system accesses Room3D directly | Dependency injection + clear APIs |
| Physics precision | Floating point accumulation | Integer grid + fractional offsets (fixed) |
| Memory management | Garbage collected | Explicit lifecycle + smart pointers |
| Performance | Interpreted bytecode | Compiled native code |
| Debugging | In-game editor only | Debugger + visualization tools |

---

## Testing Strategy

### Unit Tests (Per Phase)
- TileGrid collision queries
- Isometric projection math
- State machine transitions
- JSON serialization/deserialization

### Integration Tests
- Player collision with layered tiles
- Platform path following with player carrying
- Room transitions and save/load consistency
- Camera angle changes with face culling

### Manual Testing
- Level progression (play through all rooms)
- Edge cases (corner collisions, multi-layer platforms)
- State machine deadlocks
- Cross-platform (macOS)

---

## Expansion Path (Post-MVP)

1. **Additional platforms:** Windows, Linux (OpenGL already portable)
2. **Advanced audio:** Full audio system with dynamic mixing
3. **In-game editor:** Level designer tool (high priority for content iteration)
4. **More enemies:** Complex AI patterns (walker, fly, snake)
5. **Visual effects:** Particle systems, shader effects, CRT filter
6. **Accessibility:** Colorblind modes, input assistance

---

## Critical Files from GML to Study

**Before starting Phase 1:**
1. `/Users/thomaslarson/GameMakerProjects/PENUMBRA_PROTO_03/scripts/scr_grid/scr_grid.gml` - Room3D constructor and state machine
2. `/Users/thomaslarson/GameMakerProjects/PENUMBRA_PROTO_03/scripts/Room3DCameraSystem/Room3DCameraSystem.gml` - Camera projection
3. `/Users/thomaslarson/GameMakerProjects/PENUMBRA_PROTO_03/scripts/Room3DRendering/Room3DRendering.gml` - Vertex buffer rendering
4. `/Users/thomaslarson/GameMakerProjects/PENUMBRA_PROTO_03/objects/obj_player/Create_0.gml` - Player physics

**For collision/verticality understanding:**
5. `/Users/thomaslarson/GameMakerProjects/PENUMBRA_PROTO_03/scripts/Room3DPlatforms/Room3DPlatforms.gml` - Platform collision

---

## Estimated Migration Effort (3D Graphics-First Architecture)

| System | Complexity | Effort (days) | Priority | Phase | Notes |
|--------|-----------|---------------|----------|-------|-------|
| Project Setup & Graphics Foundation | HIGH | 5 | CRITICAL | 0 | CMake, SDL2, OpenGL, shader pipeline |
| 3D TileGrid & Sparse Storage | HIGH | 8 | CRITICAL | 1 | 3D positions, efficient lookup, sparse map |
| Isometric Camera & Orthographic Projection | HIGH | 6 | CRITICAL | 1 | Camera math, view/projection matrices |
| Sprite Quad Rendering & Z-Buffer | HIGH | 8 | CRITICAL | 1 | Textured quads, vertex buffer, depth testing |
| Render Order Validation | HIGH | 3 | CRITICAL | 1.5 | Verify z-buffer solves depth issues |
| Player Movement Physics (3D) | HIGH | 12 | HIGH | 2 | Grid-based, acceleration/deceleration |
| 3D Collision Detection | HIGH | 5 | HIGH | 2 | Point queries, layer-aware landing |
| Platform System (3D) | HIGH | 10 | HIGH | 3 | Path following, player carrying in 3D |
| State Machine Framework | MEDIUM-HIGH | 6 | HIGH | 2-3 | C++ equivalent to SnowState |
| Enemy System | MEDIUM | 6 | MEDIUM | 3 | Type system, basic AI patterns |
| Save/Load (JSON, 3D) | MEDIUM | 5 | MEDIUM | 4 | Serialization, persistence |
| Room System & Exit Linking | MEDIUM | 4 | MEDIUM | 4 | Room definitions, transitions |
| Object Spawning & Factory | MEDIUM | 4 | MEDIUM | 3 | Dynamic instantiation |
| Tile Type System | MEDIUM | 4 | MEDIUM | 4 | Behaviors, tile type definitions |
| Camera Modes | MEDIUM | 4 | MEDIUM | 5 | Follow, tile-center, overview |
| Dungeon Themes/Sprites | LOW-MEDIUM | 3 | LOW | 5 | Visual variety |
| UI/Menu System | MEDIUM | 6 | LOW | 5 | 2D overlay on 3D world |
| Audio Stub | LOW | 2 | LOWEST | 5 | Placeholder for future |
| **TOTAL** | | **101 days** | | | 4-5 months |
| **Optimistic** | | **75-85 days** | | | With parallel workflow |

---

## Sub-Agent Strategy (Token-Efficient Task Delegation)

For a migration of this complexity, parallel agent work prevents bottlenecks and manages token usage. Define specialized agents now to handle distinct workstreams during implementation.

### Agent Archetypes & Responsibilities

#### 1. **GML Analyzer Agent** (High Token Usage)
**Purpose:** Extract architectural patterns, formulas, and logic from GML source
**Trigger:** Phase 1-2 (early, high priority)
**Token Budget:** 50-80K per session
**Key Responsibilities:**
- Deep-dive analysis of specific GML modules (one module per session)
- Extract algorithms (collision math, physics, projection)
- Document data structures and state machines
- Identify edge cases and special behaviors

**Modules to Analyze (in order):**
1. `scr_grid.gml` - Room3D state machine, grid management (CRITICAL)
2. `Room3DCameraSystem.gml` - Isometric projection math
3. `Room3DRendering.gml` - Vertex buffer construction, face culling
4. `obj_player/Create_0.gml` - Player physics state machine
5. `Room3DPlatforms.gml` - Platform collision detection
6. `scr_objects.gml` - Dynamic object spawning
7. `scr_saveload.gml` - Persistence logic

**Output Format:**
```markdown
## Module: [module_name]

### Purpose
[What this module does]

### Key Data Structures
[Struct definitions, variable names, purpose]

### Critical Algorithms
1. [Algorithm name] - [Formula or pseudocode]

### State Machine(s)
- States: [list]
- Transitions: [list with conditions]
- Edge cases: [list]

### Dependencies
[What this module needs from other systems]

### Port Notes
[What needs special handling in C++]
```

---

#### 2. **C++ Boilerplate Generator Agent** (Token-Efficient)
**Purpose:** Generate project scaffolding, CMake files, header stubs
**Trigger:** Phase 0 (setup)
**Token Budget:** 30-40K
**Key Responsibilities:**
- CMakeLists.txt with Conan integration
- Project directory structure
- Shader file templates (vertex/fragment)
- Header file stubs with forward declarations
- Build system configuration

**Outputs:**
- Complete CMakeLists.txt (with SDL2, OpenGL3.3+, nlohmann_json, glm)
- conanfile.txt
- Project folder structure (src/, include/, assets/, build/)
- Shader templates (textured quad shader for Phase 1)
- Main.cpp skeleton with game loop

**Constraints:**
- macOS-first (but cross-platform capable)
- OpenGL 3.3+ core profile
- Modern C++17 minimum

---

#### 3. **Graphics Implementation Agent** (High Token Usage)
**Purpose:** Implement 3D rendering pipeline
**Trigger:** Phase 0-1 (early, CRITICAL)
**Token Budget:** 60-100K per sprint
**Key Responsibilities:**
- Math library (vectors, matrices, isometric projection formulas)
- OpenGL context management (SDL2 wrapper)
- Vertex buffer system (VAO/VBO management)
- Shader compilation and pipeline
- Sprite loading and texture management
- Camera system with orthographic projection
- Depth buffer testing and validation

**Phase 1 Output:**
```cpp
// Can render a 4x4 grid of sprite quads at different (x,y,z) positions
// Depth ordering handled by OpenGL (no manual sorting)
// Straight lines in all 3 axes due to orthographic projection
```

**Testing Requirements:**
- Render overlapping sprites at same (x, y) different z → verify correct order
- Move sprite in (x, y, z) space → verify projection math
- Verify z-fighting doesn't occur

---

#### 4. **Physics & Collision Agent** (High Token Usage)
**Purpose:** Implement grid-based movement and collision
**Trigger:** Phase 2
**Token Budget:** 50-70K per sprint
**Key Responsibilities:**
- TileGrid 3D structure (sparse storage, lookups)
- Player physics state machine
- Acceleration/deceleration movement
- Horizontal collision (wall detection)
- Vertical collision (layer-aware landing)
- Platform interaction
- State machine framework (SnowState equivalent)

**Phase 2 Output:**
```cpp
// Player moves through 3D grid with physics
// Smooth acceleration/deceleration
// Reliable collision detection (no z-position ambiguity)
// Falls correctly and lands on platforms
```

**Critical Validation:**
- Player can't pass through walls
- Landing detection is consistent
- z-position collisions are unambiguous
- Platform carrying works

---

#### 5. **Content Migration Agent** (Token-Efficient)
**Purpose:** Convert game content from GML to C++ JSON format
**Trigger:** Phase 3-4
**Token Budget:** 40-60K
**Key Responsibilities:**
- Convert room definitions (GML → JSON)
- Convert 2D grid coordinates to 3D world coordinates
- Sprite asset catalog (inventory, sprite UVs, animation frames)
- Enemy type definitions
- Tile type system
- Validation that conversions are lossless

**Outputs:**
- `rooms/Floor1_A.json`, `rooms/Floor1_B.json`, etc. (all 22 rooms)
- `sprites/catalog.json` (sprite index, frame count, animation data)
- `enemy_types.json` (all enemy behaviors)
- `tile_types.json` (all tile definitions)

**Validation:**
- Compare converted room with GML original
- Verify all objects spawn correctly
- Confirm visual appearance matches

---

#### 6. **Testing & Validation Agent** (Token-Efficient)
**Purpose:** Write and run unit/integration tests, validate each phase
**Trigger:** After each phase (1, 2, 3, 4)
**Token Budget:** 30-50K per phase
**Key Responsibilities:**
- Unit tests (math, data structures)
- Integration tests (rendering, physics, gameplay)
- Regression testing (compare with GML)
- Performance profiling
- Edge case discovery

**Test Coverage:**
- Phase 1: Render order, projection math, z-buffer correctness
- Phase 2: Collision detection, movement physics, edge cases
- Phase 3: Platform mechanics, enemy AI, interaction edge cases
- Phase 4: Save/load consistency, room transitions, feature parity

---

### Coordination Strategy

**Sequential Phases (Blocking):**
```
Phase 0: Boilerplate Generator (setup)
    ↓
Phase 1: Graphics Implementation (render pipeline)
    ├─ Parallel: GML Analyzer (Room3DRendering, Room3DCameraSystem)
    └─ Parallel: Testing Agent (render order validation)
    ↓
Phase 2: Physics & Collision + GML Analyzer (player systems)
    ├─ Parallel: GML Analyzer (obj_player, Room3DPlatforms)
    └─ Parallel: Testing Agent (collision, physics validation)
    ↓
Phase 3: Physics & Collision (platforms, enemies) + Content Migration (prep)
    ├─ Parallel: GML Analyzer (scr_objects, scr_enemies)
    └─ Parallel: Testing Agent (gameplay scenarios)
    ↓
Phase 4: Content Migration (convert all content)
    ├─ Parallel: Physics Agent (finalizations)
    └─ Parallel: Testing Agent (feature parity)
    ↓
Phase 5: Polish + Boilerplate (additional platforms, tools)
```

**Parallel Opportunities:**
- While Graphics Agent builds Phase 1, GML Analyzer studies Room3D internals
- While Physics Agent implements player movement, Content Migration converts room definitions
- Testing Agent runs continuously after each component is ready

---

### Agent Communication Protocol

**Input Requirements:**
- Each agent receives:
  1. Specific task description
  2. Relevant GML source code (if analyzing)
  3. Success criteria
  4. Output format specification
  5. Known constraints/patterns from previous phases

**Output Requirements:**
- Code with comments for complex logic
- Summary document explaining decisions
- List of assumptions made
- Known limitations or edge cases
- Integration notes (how to use in larger system)

**Handoff Points:**
- Graphics Agent → Physics Agent: Confirmed rendering pipeline works
- Physics Agent → Content Migration: Confirmed movement/collision stable
- Content Migration → Testing Agent: Content converted, ready for validation

---

### Token Budget Allocation

| Agent | Phase | Budget | Purpose |
|-------|-------|--------|---------|
| Boilerplate | 0 | 30-40K | Setup, one-time |
| GML Analyzer | 1-4 | 50-80K per | Extract logic (7 sessions) |
| Graphics | 1-2 | 60-100K per | Critical path (2 sprints) |
| Physics | 2-3 | 50-70K per | Gameplay systems (2 sprints) |
| Content Migration | 3-4 | 40-60K | One large session |
| Testing | After each | 30-50K | Validation, continuous |
| **TOTAL** | | **~600K tokens** | Over 4-5 months |

**Per-Sprint Token Usage:**
- Phase 0: 30-40K (Boilerplate)
- Phase 1: 120-150K (Graphics + GML Analyzer + Testing)
- Phase 2: 130-160K (Physics + GML Analyzer + Testing + Graphics refinement)
- Phase 3: 120-150K (Physics finish + Content prep + GML Analyzer + Testing)
- Phase 4: 90-120K (Content Migration + Testing + refinements)
- Phase 5: 60-80K (Polish, editor, audio stub)

---

### Agent Prompt Templates

#### GML Analyzer Prompt Template
```
Analyze this GameMaker GML module for migration to C++.

MODULE: [module_name]
CONTEXT: [what this does in the overall system]
CRITICAL PATTERNS: [known architectural patterns]

Please provide:
1. Purpose and responsibility
2. Key data structures (exact variable names, types, purposes)
3. State machines (if any)
4. Critical algorithms (formulas, pseudocode)
5. Dependencies on other systems
6. Edge cases and special behaviors
7. C++ port notes (what needs attention)

GML Source:
[full module code]

Format output as detailed markdown suitable for C++ implementation reference.
```

#### Graphics Agent Prompt Template
```
Implement 3D isometric rendering pipeline for custom C++ game engine.

ARCHITECTURE:
- OpenGL 3.3+ core profile
- SDL2 for window management
- Orthographic projection (not perspective)
- Sprites as textured quads in 3D space
- Depth buffer for automatic render ordering

PHASE: [1 for initial, 1.5 for validation]

DELIVERABLE:
[specific code requirements]

Please provide:
1. Complete implementation of [specific system]
2. Usage example code
3. Known limitations
4. Testing approach
5. Integration notes

Reference GML implementations:
[relevant GML code snippets]
```

#### Testing Agent Prompt Template
```
Write comprehensive tests for [system] in C++.

SYSTEM: [name]
PHASE: [phase number]
SUCCESS CRITERIA:
[criteria from plan]

Test categories:
1. Unit tests (individual functions)
2. Integration tests (system interaction)
3. Regression tests (compare GML behavior)
4. Edge case discovery

Provide:
1. Test code (Google Test or similar)
2. Test data/fixtures
3. Expected outputs
4. Known edge cases verified
```

---

## Implementation: Using Agents Effectively

**During Phase 1 (Critical Graphics Path):**
1. **Monday:** Boilerplate Agent completes project setup (CMake, directory structure)
2. **Parallel Tuesday-Thursday:**
   - Graphics Agent: Builds math library, OpenGL context, VAO/VBO system
   - GML Analyzer: Studies Room3D rendering implementation (scr_grid + Room3DRendering)
3. **Friday:**
   - Graphics Agent: Implements sprite quad rendering
   - Testing Agent: Validates render order with overlapping sprites

**General Workflow for Each Phase:**
1. GML Analyzer prepares reference documentation
2. Implementation Agent (Graphics/Physics/Content) uses reference
3. Testing Agent validates against GML original behavior
4. Main orchestrator integrates components

---

## Expected Efficiency Gains

| Approach | Time | Token Usage | Quality |
|----------|------|-------------|---------|
| Single developer | 4-5 months | ~600K | High (but slower) |
| Parallel agents | 3-4 months | ~700K | High (faster iteration) |
| No agents | 5-6 months | ~500K | Lower (bottlenecks) |

**Benefits:**
- GML analysis happens while coding progresses
- Testing finds issues immediately
- Content migration happens in parallel
- Reduces context switching for main developer
- Manages token usage via focused, specialized agents

---

## Agent Definitions for Claude Code

Create these agent definitions before proceeding. Each agent can be invoked via CLI or Claude Code commands.

### Agent Definition Format

Save each agent definition as a `.md` file in `~/.claude/agents/` with the following structure:

```
~/.claude/agents/
├── gml-analyzer.md
├── graphics-implementation.md
├── physics-collision.md
├── content-migration.md
├── testing-validation.md
└── boilerplate-generator.md
```

### 1. GML Analyzer Agent Definition

**File:** `~/.claude/agents/gml-analyzer.md`

```markdown
# GML Analyzer Agent

## Purpose
Extract architectural patterns, formulas, and logic from GameMaker GML source code for C++ migration.

## Capabilities
- Deep analysis of GML modules
- Extract algorithms and formulas
- Document data structures
- Identify state machines and edge cases
- Generate C++ port guidelines

## Invocation
\`\`\`bash
/agent:gml-analyzer --module [module_name] --context-file [path_to_gml]
\`\`\`

## Parameters
- `module`: Name of GML module (e.g., "scr_grid", "obj_player")
- `context-file`: Path to GML source file
- `focus`: Optional focus area (algorithms|structures|state-machine|edge-cases)

## Output Format
Markdown document with sections:
1. Purpose
2. Key Data Structures
3. Critical Algorithms
4. State Machine(s)
5. Dependencies
6. Edge Cases
7. C++ Port Notes

## Success Criteria
- All algorithms documented with pseudocode
- Every variable and struct member explained
- State transitions clearly mapped
- Edge cases identified
- C++ equivalents suggested

## Token Budget
50-80K per session

## Modules to Analyze (Priority Order)
1. scr_grid.gml - CRITICAL
2. Room3DCameraSystem.gml
3. Room3DRendering.gml
4. obj_player/Create_0.gml
5. Room3DPlatforms.gml
6. scr_objects.gml
7. scr_saveload.gml
```

---

### 2. Graphics Implementation Agent Definition

**File:** `~/.claude/agents/graphics-implementation.md`

```markdown
# Graphics Implementation Agent

## Purpose
Implement 3D isometric rendering pipeline using OpenGL 3.3+ and SDL2.

## Capabilities
- Math library implementation (vectors, matrices, projection)
- OpenGL context management
- Vertex buffer system (VAO/VBO)
- Shader compilation and management
- Sprite loading and texture binding
- Camera system with orthographic projection
- Depth buffer validation

## Invocation
\`\`\`bash
/agent:graphics --phase [1|1.5|2] --deliverable [system_name]
\`\`\`

## Parameters
- `phase`: Implementation phase (1=basic, 1.5=validation, 2=refinement)
- `deliverable`: Specific system (math|opengl-context|vao-vbo|shaders|camera|sprite-rendering)
- `gml-reference`: Optional path to GML reference implementation

## Phase 1 Deliverables
- [ ] Math library (Vector3, Matrix4, isometric projection formulas)
- [ ] SDL2 window and OpenGL 3.3+ context
- [ ] VAO/VBO system for vertex buffer management
- [ ] Textured quad shader (vertex + fragment)
- [ ] Sprite loading system
- [ ] Camera with orthographic projection
- [ ] 4x4 grid of sprite tiles rendering at different (x,y,z)

## Testing Requirements
- Render overlapping sprites → verify z-buffer ordering
- Move sprites in 3D space → verify projection math
- Verify straight lines in all axes (orthographic property)
- Performance: 60+ FPS on test grid

## Success Criteria
- No manual depth sorting needed (OpenGL handles it)
- Sprites render in correct order based on z-position
- No z-fighting artifacts
- Isometric perspective looks correct

## Token Budget
60-100K per sprint

## Output
- Complete C++ implementation files
- Usage examples
- Integration guide
- Known limitations
- Test validation results
```

---

### 3. Physics & Collision Agent Definition

**File:** `~/.claude/agents/physics-collision.md`

```markdown
# Physics & Collision Agent

## Purpose
Implement grid-based movement physics and 3D collision detection.

## Capabilities
- TileGrid 3D data structure
- Player physics state machine
- Acceleration/deceleration movement
- Horizontal collision detection
- Vertical collision with layer awareness
- Platform interaction and carrying
- State machine framework (SnowState equivalent)

## Invocation
\`\`\`bash
/agent:physics --phase [2|3] --system [tilegrid|player-physics|collision|platforms|state-machine]
\`\`\`

## Parameters
- `phase`: Implementation phase (2=movement, 3=platforms)
- `system`: Specific system to implement
- `gml-reference`: Path to GML implementation for reference

## Phase 2 Deliverables
- [ ] TileGrid 3D structure with sparse storage
- [ ] Player physics state machine
- [ ] Acceleration/deceleration movement
- [ ] Horizontal collision detection
- [ ] Vertical collision with layer support
- [ ] Jumping and falling physics
- [ ] State machine framework

## Phase 3 Deliverables
- [ ] Platform system (static and moving)
- [ ] Player carrying on platforms
- [ ] Enemy spawning and basic AI
- [ ] Type-specific behaviors
- [ ] Complete physics validation

## Critical Validation
- No wall clipping
- Landing detection is consistent
- Z-position collisions unambiguous
- Platform carrying works correctly
- Smooth acceleration/deceleration

## Token Budget
50-70K per sprint

## Output
- Complete C++ implementation
- State machine diagrams
- Physics constants documented
- Integration examples
- GML comparison (regression validation)
```

---

### 4. Content Migration Agent Definition

**File:** `~/.claude/agents/content-migration.md`

```markdown
# Content Migration Agent

## Purpose
Convert game content from GameMaker format to C++ JSON format.

## Capabilities
- Room definition conversion (GML → JSON)
- 2D grid coordinates → 3D world coordinates
- Sprite asset catalog creation
- Enemy type system migration
- Tile type system migration
- Validation and comparison with GML original

## Invocation
\`\`\`bash
/agent:content-migration --task [rooms|sprites|enemies|tiles] --source-dir [path]
\`\`\`

## Parameters
- `task`: What to migrate (rooms|sprites|enemies|tiles|all)
- `source-dir`: Path to GML project directory
- `validation`: Compare output with GML original (default: true)

## Deliverables
- [ ] All 22 rooms converted to JSON
- [ ] Sprite catalog with UVs and animation data
- [ ] Enemy type definitions
- [ ] Tile type definitions
- [ ] Validation report (lossless conversion?)

## Output Format

**rooms/Floor1_A.json:**
\`\`\`json
{
  "name": "Floor1_A",
  "dungeon_theme": "gray_stone",
  "tiles": [
    {"x": 5, "y": 4, "z": 0, "type": "platform"}
  ],
  "objects": [
    {"type": "platform", "x": 6, "y": 5, "z": 0, "path": [...]}
  ]
}
\`\`\`

**sprites/catalog.json:**
\`\`\`json
{
  "platform_tile": {"sprite_index": 0, "frames": 1, "uv": [0, 0, 48, 48]},
  "spike_tile": {"sprite_index": 1, "frames": 4, "uv": [0, 0, 48, 48]}
}
\`\`\`

## Validation
- Count tiles before/after conversion
- Visual spot-checks (screenshot comparison)
- Position validation (2D grid → 3D world)
- All object types present

## Token Budget
40-60K

## Output
- JSON files in project structure
- Conversion report
- Lossless validation results
- Any ambiguities or questions for clarification
```

---

### 5. Testing & Validation Agent Definition

**File:** `~/.claude/agents/testing-validation.md`

```markdown
# Testing & Validation Agent

## Purpose
Write comprehensive tests and validate each phase against GML original.

## Capabilities
- Unit test generation (Google Test framework)
- Integration test design
- Regression testing (C++ vs GML comparison)
- Performance profiling
- Edge case discovery

## Invocation
\`\`\`bash
/agent:testing --phase [1|2|3|4] --system [system_name] --gml-reference [path]
\`\`\`

## Parameters
- `phase`: Which phase to test
- `system`: System to test (rendering|physics|collision|gameplay|persistence)
- `gml-reference`: Path to GML implementation for regression tests

## Phase 1 Tests
- Math library correctness
- Isometric projection validation
- Z-buffer depth ordering
- Sprite rendering accuracy

## Phase 2 Tests
- Collision detection (unit)
- Player movement (integration)
- Physics constants validation
- Edge cases (wall corners, slopes)

## Phase 3 Tests
- Platform mechanics
- Enemy spawning
- Player-platform interaction
- Multi-level scenarios

## Phase 4 Tests
- Save/load round-trip
- Room transitions
- Feature parity with GML
- Complete gameplay progression

## Test Framework
- Google Test (gtest)
- Custom fixture for GML comparison
- Performance benchmarks

## Success Criteria
- 100% of critical paths tested
- All regressions detected before integration
- No edge cases missed
- Performance meets requirements (60 FPS)

## Token Budget
30-50K per phase

## Output
- Complete test suite (.cpp files)
- Test data and fixtures
- Test execution report
- Coverage analysis
- Performance profiles
```

---

### 6. C++ Boilerplate Generator Agent Definition

**File:** `~/.claude/agents/boilerplate-generator.md`

```markdown
# C++ Boilerplate Generator Agent

## Purpose
Generate project scaffolding, build configuration, and header stubs.

## Capabilities
- CMakeLists.txt generation with Conan
- Project directory structure
- Shader file templates
- Header file stubs with forward declarations
- Main.cpp skeleton
- Build system configuration

## Invocation
\`\`\`bash
/agent:boilerplate --init --project-name penumbra-cpp --target-platform macos
\`\`\`

## Parameters
- `init`: Initialize new project
- `project-name`: Name of project
- `target-platform`: Target OS (macos|windows|linux|all)
- `cpp-standard`: C++ version (default: 17)

## Deliverables
- [ ] CMakeLists.txt (with SDL2, OpenGL, nlohmann_json, glm, gtest)
- [ ] conanfile.txt
- [ ] Directory structure (src/, include/, assets/, build/, tests/)
- [ ] Shader templates (basic textured quad)
- [ ] Main.cpp with game loop skeleton
- [ ] CMake configuration for macOS

## Project Structure
\`\`\`
penumbra-cpp/
├── CMakeLists.txt
├── conanfile.txt
├── src/
│   ├── main.cpp
│   ├── core/
│   ├── game/
│   ├── rendering/
│   └── systems/
├── include/
│   ├── core/
│   ├── game/
│   ├── rendering/
│   └── systems/
├── assets/
│   ├── shaders/
│   ├── sprites/
│   ├── rooms/
│   └── sounds/
├── tests/
│   └── CMakeLists.txt
└── build/
\`\`\`

## Build Verification
- [ ] CMake configures successfully
- [ ] All dependencies resolve via Conan
- [ ] Project compiles without errors
- [ ] Game loop runs (blank window)

## Token Budget
30-40K (one-time)

## Output
- Complete project scaffolding
- Ready to compile
- All dependencies specified
- Build instructions
```

---

## How to Create Agents in Claude Code

### Step 1: Create Agent Definition Files

Create each `.md` file in `~/.claude/agents/` with the definitions above.

### Step 2: Register Agents (if needed)

If Claude Code requires agent registration, add to `~/.claude/agents/.index.yml`:

```yaml
agents:
  - name: gml-analyzer
    file: gml-analyzer.md
    description: Extract GML logic for C++ migration
    token_budget: 50000
    priority: high

  - name: graphics-implementation
    file: graphics-implementation.md
    description: Implement 3D isometric rendering
    token_budget: 80000
    priority: critical

  - name: physics-collision
    file: physics-collision.md
    description: Implement game physics and collision
    token_budget: 70000
    priority: high

  - name: content-migration
    file: content-migration.md
    description: Convert GML content to JSON format
    token_budget: 60000
    priority: medium

  - name: testing-validation
    file: testing-validation.md
    description: Write tests and validate implementations
    token_budget: 50000
    priority: medium

  - name: boilerplate-generator
    file: boilerplate-generator.md
    description: Generate C++ project scaffolding
    token_budget: 40000
    priority: critical
```

### Step 3: Invoke Agents During Implementation

**Phase 0 (Setup):**
```bash
/agent:boilerplate --init --project-name penumbra-cpp --target-platform macos
```

**Phase 1 (Graphics):**
```bash
/agent:graphics --phase 1 --deliverable math
/agent:graphics --phase 1 --deliverable opengl-context
/agent:graphics --phase 1 --deliverable vao-vbo
/agent:graphics --phase 1 --deliverable shaders
/agent:graphics --phase 1 --deliverable sprite-rendering
/agent:gml-analyzer --module scr_grid --context-file [path]
/agent:gml-analyzer --module Room3DRendering --context-file [path]
/agent:testing --phase 1 --system rendering
```

**Phase 2 (Physics):**
```bash
/agent:physics --phase 2 --system tilegrid
/agent:physics --phase 2 --system player-physics
/agent:physics --phase 2 --system collision
/agent:gml-analyzer --module obj_player --context-file [path]
/agent:testing --phase 2 --system physics
```

**Phase 3-4 (Content & Finalization):**
```bash
/agent:content-migration --task all --source-dir [gml_project_path]
/agent:testing --phase 3 --system gameplay
/agent:testing --phase 4 --system persistence
```

---

## Agent Coordination Rules

### Blocking Dependencies
1. **Phase 0 → Phase 1:** Boilerplate must complete before Graphics starts
2. **Phase 1 → Phase 2:** Rendering must be validated before Physics starts
3. **Phase 2 → Phase 3:** Player movement must be stable before Platforms start
4. **Phase 3 → Phase 4:** Gameplay must be complete before Content migrates

### Parallel Opportunities
- While `graphics --phase 1` runs: Launch `gml-analyzer --module scr_grid`
- While `physics --phase 2` runs: Launch `gml-analyzer --module obj_player`
- While `physics --phase 3` runs: Launch `content-migration --task rooms`
- Continuously: Launch `testing` after each component completes

### Communication Protocol

**GML Analyzer → Implementation Agent:**
- Output: Detailed algorithm documentation
- Used by: Implementation agents as reference
- Format: Markdown with pseudocode

**Implementation Agent → Testing Agent:**
- Output: Compiled code + test entry points
- Used by: Testing to write validation tests
- Format: C++ code + API documentation

**Content Migration → Testing Agent:**
- Output: Converted JSON files
- Used by: Testing to validate data integrity
- Format: JSON + validation report

---

## Quick Start: Next Steps

1. **Create agent definition files** in `~/.claude/agents/`
2. **Register agents** (if Claude Code requires it)
3. **Confirm Phase 0 setup** with user
4. **Launch Boilerplate Generator** to initialize C++ project
5. **Begin Phase 1** with Graphics Implementation + GML Analyzer

---
```

This section provides the complete agent setup instructions so you can add them to Claude Code before proceeding with implementation.

✅ **GML Analyzer Success:** Extract all critical logic accurately, zero missed edge cases
✅ **Graphics Agent Success:** Phase 1 deliverable meets all rendering requirements by day 18
✅ **Physics Agent Success:** Player movement is smooth and collision is reliable
✅ **Content Migration Success:** All 22 rooms convert losslessly
✅ **Testing Agent Success:** All regressions detected before main integration
✅ **Coordination Success:** No blocking dependencies, agents work in parallel

---



- ✅ **Graphics Foundation:** SDL2 + OpenGL context with shader pipeline working
- ✅ **3D Rendering:** 4x4 grid of sprite tiles render correctly at different (x, y, z) positions
- ✅ **Render Order:** No manual depth sorting needed—OpenGL z-buffer handles all ordering automatically
- ✅ **Straight Lines:** Isometric projection maintains geometry in all three axes (orthographic view)
- ✅ **Player Movement:** Smooth grid-based movement through 3D space with proper acceleration/deceleration
- ✅ **Collision:** Tile-based collision with layer-aware landing detection
- ✅ **Platform System:** Static and moving platforms work, player carrying functional
- ✅ **Enemy Spawning:** Basic enemy types instantiate and move correctly
- ✅ **Save/Load:** Complete world state persists and restores (including z-positions)
- ✅ **Room Transitions:** Can move between rooms seamlessly
- ✅ **Frame Rate:** Stable 60 FPS on target macOS hardware
- ✅ **Feature Parity:** All rooms from GML version load and play with identical behavior
- ✅ **No Depth Artifacts:** Verify GML version's z-position/render order issues are completely eliminated
