# Agent Coordination Guide

Complete guide for coordinating the 6-agent migration pipeline for PENUMBRA GML → C++ conversion.

---

## Overview

The migration uses 6 specialized agents working in a coordinated sequence with parallel opportunities:

1. **Boilerplate Generator** - Project scaffolding (Phase 0)
2. **Graphics Implementation** - 3D rendering pipeline (Phase 1-2)
3. **Physics & Collision** - Movement and collision (Phase 2-3)
4. **GML Analyzer** - Extract logic from GameMaker (Phase 1-4)
5. **Content Migration** - Convert game content to JSON (Phase 3-4)
6. **Testing & Validation** - Write and run tests (Phase 1-4)

---

## Blocking Dependencies

These phases must complete sequentially:

```
Phase 0: Boilerplate Generator (setup)
    ↓
Phase 1: Graphics Implementation (render pipeline)
    ├─ Parallel: GML Analyzer (Room3DRendering, Room3DCameraSystem)
    └─ Parallel: Testing Validation (render order)
    ↓
Phase 2: Graphics refinement + Physics & Collision (movement)
    ├─ Parallel: GML Analyzer (obj_player, Room3DPlatforms)
    └─ Parallel: Testing Validation (physics, collision)
    ↓
Phase 3: Physics & Collision (platforms, enemies) + Content Migration (prep)
    ├─ Parallel: GML Analyzer (scr_objects, scr_enemies)
    └─ Parallel: Testing Validation (gameplay)
    ↓
Phase 4: Content Migration (convert all content)
    ├─ Parallel: Physics finalization
    └─ Parallel: Testing Validation (feature parity)
    ↓
Phase 5: Polish + refinements
```

---

## Parallel Execution Strategy

### Phase 1: Graphics Foundation (Days 6-21)

**Sequential path:**
1. `boilerplate-generator --init` (Phase 0 prerequisite)
2. `graphics-implementation --phase 1 --deliverable math`
3. `graphics-implementation --phase 1 --deliverable opengl-context`
4. `graphics-implementation --phase 1 --deliverable vao-vbo`
5. `graphics-implementation --phase 1 --deliverable shaders`
6. `graphics-implementation --phase 1 --deliverable sprite-rendering`
7. `graphics-implementation --phase 1 --deliverable camera`
8. `testing-validation --phase 1 --system rendering`
9. `graphics-implementation --phase 1.5` (validation)

**Parallel opportunities (start while graphics Phase 1 runs):**
- Day 6: `gml-analyzer --module scr_grid --focus structures`
- Day 8: `gml-analyzer --module Room3DCameraSystem --focus algorithms`
- Day 10: `gml-analyzer --module Room3DRendering --focus algorithms`

---

### Phase 2: Player Movement (Days 22-40)

**Sequential path:**
1. Graphics Phase 1.5 validates completely ← BLOCKING
2. `physics-collision --phase 2 --system tilegrid`
3. `physics-collision --phase 2 --system player-physics`
4. `physics-collision --phase 2 --system collision`
5. `testing-validation --phase 2 --system physics`

**Parallel opportunities:**
- Day 22: `gml-analyzer --module obj_player --focus state-machine`
- Day 26: `gml-analyzer --module Room3DPlatforms --focus algorithms`
- Day 32: Prepare `content-migration --task rooms` (study room definitions)

---

### Phase 3: Platforms & Enemies (Days 41-56)

**Sequential path:**
1. Physics Phase 2 complete and validated ← BLOCKING
2. `physics-collision --phase 3 --system platforms`
3. `physics-collision --phase 3 --system enemy-spawning`
4. `testing-validation --phase 3 --system gameplay`

**Parallel opportunities:**
- Day 41: `gml-analyzer --module scr_objects --focus structures`
- Day 44: `content-migration --task rooms --validation true` (convert room definitions)
- Day 48: `content-migration --task sprites` (convert sprite catalog)
- Day 50: `content-migration --task enemies` (convert enemy definitions)

---

### Phase 4: Persistence & Finalization (Days 57-71)

**Sequential path:**
1. Gameplay Phase 3 complete ← BLOCKING
2. `content-migration --task all --validation true` (final conversion)
3. `testing-validation --phase 4 --system persistence` (save/load tests)
4. `testing-validation --phase 4 --system gameplay-progression` (full game test)

**Parallel opportunities:**
- Day 57: Physics refinements while content converts
- Day 65: Visual validation of converted content

---

## Agent Communication Protocol

### Input Requirements

Each agent receives:
1. **Specific task description** - Exact work to be done
2. **Relevant source code** - GML files or existing C++ code
3. **Success criteria** - Definition of completion
4. **Output format** - Expected deliverable structure
5. **Context** - Known constraints and patterns from previous phases

### Output Requirements

Each agent delivers:
1. **Code or documentation** - Actual implementation or analysis
2. **Summary document** - Explaining decisions and approach
3. **Assumptions list** - What was assumed about unclear requirements
4. **Known limitations** - Edge cases or future improvements
5. **Integration notes** - How to use in the larger system

### Handoff Points

**GML Analyzer → Implementation Agents:**
- **Input:** Specific GML module and analysis scope
- **Output:** Detailed algorithm documentation with pseudocode
- **Format:** Markdown with code blocks
- **Used by:** Implementation agents as authoritative reference
- **Timeline:** 2-3 days before each implementation phase

**Implementation Agents → Testing Validator:**
- **Input:** Compiled code + test entry points
- **Output:** Confirmed working implementation
- **Format:** C++ code + API documentation
- **Used by:** Testing to write validation tests
- **Timeline:** Immediately after each component completes

**Content Migration → Testing Validator:**
- **Input:** Converted JSON files
- **Output:** Confirmed lossless conversion
- **Format:** JSON + validation report
- **Used by:** Testing to validate data integrity
- **Timeline:** After content conversion completes

---

## Agent Invocation Commands

### Phase 0: Setup

```bash
# Initialize C++ project structure
/agent:boilerplate --init --project-name penumbra-cpp --target-platform macos
```

### Phase 1: Graphics Foundation

```bash
# Math library (Vector3, Matrix4, isometric projection)
/agent:graphics --phase 1 --deliverable math

# SDL2 + OpenGL 3.3+ context
/agent:graphics --phase 1 --deliverable opengl-context

# Vertex Array Objects and Vertex Buffer Objects
/agent:graphics --phase 1 --deliverable vao-vbo

# Shader compilation and management system
/agent:graphics --phase 1 --deliverable shaders

# Sprite loading and texture management
/agent:graphics --phase 1 --deliverable sprite-rendering

# Orthographic camera and projection
/agent:graphics --phase 1 --deliverable camera

# Analyze GML grid system (parallel)
/agent:gml-analyzer --module scr_grid --focus structures

# Analyze GML camera implementation (parallel)
/agent:gml-analyzer --module Room3DCameraSystem --focus algorithms

# Analyze GML rendering system (parallel)
/agent:gml-analyzer --module Room3DRendering --focus algorithms

# Test Phase 1 rendering
/agent:testing --phase 1 --system rendering --gml-reference [path]

# Validate Phase 1 correctness
/agent:graphics --phase 1.5 --validate true
```

### Phase 2: Movement Physics

```bash
# TileGrid 3D data structure with sparse storage
/agent:physics --phase 2 --system tilegrid

# Player movement physics and state machine
/agent:physics --phase 2 --system player-physics

# Collision detection system
/agent:physics --phase 2 --system collision

# Analyze GML player implementation (parallel)
/agent:gml-analyzer --module obj_player/Create_0.gml --focus state-machine

# Analyze GML platform system (parallel)
/agent:gml-analyzer --module Room3DPlatforms --focus algorithms

# Test Phase 2 physics
/agent:testing --phase 2 --system physics --gml-reference [path]
```

### Phase 3: Platforms & Enemies

```bash
# Platform system (static and moving, with player carrying)
/agent:physics --phase 3 --system platforms

# Enemy spawning and basic AI
/agent:physics --phase 3 --system enemy-spawning

# Analyze GML object system (parallel)
/agent:gml-analyzer --module scr_objects --focus structures

# Begin content migration (parallel)
/agent:content-migration --task rooms --validation true --source-dir [gml_path]

# Migrate sprite definitions (parallel)
/agent:content-migration --task sprites --validation true --source-dir [gml_path]

# Migrate enemy definitions (parallel)
/agent:content-migration --task enemies --validation true --source-dir [gml_path]

# Test Phase 3 gameplay
/agent:testing --phase 3 --system gameplay --gml-reference [path]
```

### Phase 4: Persistence & Features

```bash
# Complete content migration
/agent:content-migration --task all --validation true --source-dir [gml_path]

# Test save/load functionality
/agent:testing --phase 4 --system persistence --gml-reference [path]

# Test complete gameplay progression
/agent:testing --phase 4 --system gameplay-progression --gml-reference [path]
```

---

## Token Budget Allocation

Total budget: **~610,000 tokens** over 4-5 months

### Per Phase

| Phase | Agent | Budget | Purpose |
|-------|-------|--------|---------|
| 0 | Boilerplate | 30-40K | Setup, one-time |
| 1 | Graphics | 60-80K | Rendering foundation |
| 1 | GML Analyzer | 50-80K | Room3D analysis (3 modules) |
| 1 | Testing | 20-30K | Render order validation |
| 2 | Graphics | 20-30K | Refinement |
| 2 | Physics | 50-70K | Player movement |
| 2 | GML Analyzer | 50-80K | Player analysis (2 modules) |
| 2 | Testing | 20-30K | Physics validation |
| 3 | Physics | 50-70K | Platforms & enemies |
| 3 | GML Analyzer | 50-80K | Object system analysis (2 modules) |
| 3 | Content Migration | 40-60K | Room conversion |
| 3 | Testing | 20-30K | Gameplay validation |
| 4 | Content Migration | 20-30K | Final validation |
| 4 | Testing | 30-50K | Feature parity |
| 5 | Polish | 40-60K | Final refinements |
| **TOTAL** | | **~610K** | Over 4-5 months |

---

## Quality Gates

### Phase Completion Criteria

**Phase 0 Complete When:**
- [ ] CMakeLists.txt compiles without errors
- [ ] All dependencies resolve via Conan
- [ ] Project structure matches specification
- [ ] Blank SDL2/OpenGL window renders

**Phase 1 Complete When:**
- [ ] 4x4 grid of sprite quads renders at (x, y, z) positions
- [ ] Overlapping sprites render in correct order (z-buffer)
- [ ] No z-fighting artifacts detected
- [ ] Isometric projection preserves straight lines
- [ ] 60+ FPS on test grid
- [ ] All GML rendering analysis complete

**Phase 2 Complete When:**
- [ ] Player moves through 3D grid with smooth physics
- [ ] Collision detection works reliably (no wall clipping)
- [ ] Landing detection is consistent and unambiguous
- [ ] Horizontal and vertical collisions work separately
- [ ] Player can jump and fall correctly
- [ ] All GML player analysis complete

**Phase 3 Complete When:**
- [ ] Static platforms work (player can stand on them)
- [ ] Moving platforms work (player moves with platform)
- [ ] Enemy types spawn correctly
- [ ] Basic AI patterns work (stationary, pusher, bouncer)
- [ ] Room transitions work
- [ ] All GML object system analysis complete

**Phase 4 Complete When:**
- [ ] All 22 rooms convert to JSON without data loss
- [ ] Save/load round-trip works perfectly
- [ ] Room transitions preserve all state
- [ ] Feature parity with GML version confirmed
- [ ] All edge cases from GML eliminated

---

## Troubleshooting & Escalation

### If Graphics Phase 1 fails:
1. GML Analyzer may not have fully understood Room3D
2. Review Room3DRendering.gml more carefully
3. Check isometric projection math step-by-step
4. Consider reference implementation of simple quad rendering

### If Physics collisions are unreliable:
1. Verify TileGrid sparse storage works correctly
2. Check horizontal and vertical collision queries separately
3. Compare with GML obj_player collision logic
4. Validate layer-aware landing calculation

### If Content Migration loses data:
1. Validate conversion count matches GML
2. Visual spot-check converted rooms in new engine
3. Compare object positions before/after
4. Check all enemy types present

### If Testing finds regressions:
1. Stop implementation immediately
2. GML Analyzer studies relevant module
3. Identify root cause before continuing
4. Ensure tests pass before next phase

---

## Communication Between You and Agents

### When Starting an Agent:

Provide:
```
Task: [Specific work needed]
Context: [What this is part of / why needed]
GML Reference: [Path to GML files if needed]
Success Criteria: [How to know when done]
Known Constraints: [Any limitations from previous phases]
```

### When Reviewing Agent Output:

Check:
1. **Completeness** - Did they deliver everything requested?
2. **Quality** - Is the code production-ready or well-analyzed?
3. **Integration** - Can this connect to the next phase?
4. **Testing** - Is the work validated?

### When Agents Provide Code:

1. **Read the integration notes** - How to use in larger system
2. **Check the known limitations** - What's not implemented
3. **Review the test validation** - What's been proven working
4. **Test in context** - Verify it works with other systems

---

## Expected Timeline

- **Phase 0:** 1 week (setup)
- **Phase 1:** 2-3 weeks (graphics foundation - CRITICAL PATH)
- **Phase 2:** 2-3 weeks (player movement)
- **Phase 3:** 2-3 weeks (platforms, enemies, content prep)
- **Phase 4:** 1-2 weeks (content migration, finalization)
- **Phase 5:** 1-2 weeks (polish, refinements)

**Total: 10-14 weeks (parallel agents reduce this by ~30%)**

---

## Success Indicators

When the migration is complete, you should have:

✅ **Graphics Foundation:** SDL2 + OpenGL with working shader pipeline
✅ **3D Rendering:** 4x4 sprite grid rendering with no manual depth sorting
✅ **Player Movement:** Smooth, reliable physics and collision
✅ **Platform System:** Static and moving platforms with player carrying
✅ **Enemy System:** All enemy types spawning and behaving correctly
✅ **Persistence:** Save/load works, z-positions properly persisted
✅ **Content:** All 22 rooms converted losslessly
✅ **Feature Parity:** Complete gameplay identical to GML version
✅ **No Artifacts:** Z-position ambiguity from GML completely eliminated
✅ **Performance:** Stable 60 FPS on target hardware

---

## Next Steps

1. Review this coordination guide
2. Confirm agent definitions in `.index.yml`
3. Verify all 6 agent files exist in `~/.claude/agents/`
4. Start Phase 0 with boilerplate-generator
5. Begin Phase 1 graphics implementation

Good luck! The agent coordination strategy will keep you focused and prevent bottlenecks.
