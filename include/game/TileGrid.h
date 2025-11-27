#pragma once

#include "core/Math.h"
#include <vector>
#include <string>

namespace Penumbra {
namespace Game {

/**
 * Tile types in the grid
 */
enum class TileType {
    Empty = 0,
    Solid = 1,
    Platform = 2,
    Hazard = 3,
    Ladder = 4
};

/**
 * Individual tile in the grid
 */
struct Tile {
    TileType type;
    int textureIndex;
    Math::Color tint;

    Tile() : type(TileType::Empty), textureIndex(0), tint(Math::Color::White) {}
    Tile(TileType type, int texIndex = 0)
        : type(type), textureIndex(texIndex), tint(Math::Color::White) {}

    bool isSolid() const { return type == TileType::Solid; }
    bool isPlatform() const { return type == TileType::Platform; }
    bool isHazard() const { return type == TileType::Hazard; }
    bool isCollidable() const { return isSolid() || isPlatform(); }
};

/**
 * Grid-based level structure
 * Manages tile layout and collision queries
 */
class TileGrid {
public:
    static constexpr int TILE_SIZE = 16;

    TileGrid();
    TileGrid(int width, int height);

    /**
     * Initialize grid with specified dimensions
     */
    void initialize(int width, int height);

    /**
     * Set tile at grid position
     */
    void setTile(int x, int y, const Tile& tile);

    /**
     * Get tile at grid position
     */
    const Tile& getTile(int x, int y) const;

    /**
     * Check if grid position is valid
     */
    bool isValidPosition(int x, int y) const;

    /**
     * Convert world position to grid coordinates
     */
    void worldToGrid(float worldX, float worldY, int& outGridX, int& outGridY) const;

    /**
     * Convert grid coordinates to world position
     */
    void gridToWorld(int gridX, int gridY, float& outWorldX, float& outWorldY) const;

    /**
     * Check for collision with tiles in AABB
     * @return true if any solid/platform tile intersects the AABB
     */
    bool checkCollision(const Math::AABB& bounds) const;

    /**
     * Get all tiles that intersect with AABB
     */
    std::vector<Math::AABB> getCollidingTiles(const Math::AABB& bounds) const;

    /**
     * Load grid from JSON data
     */
    bool loadFromJson(const std::string& jsonData);

    /**
     * Save grid to JSON format
     */
    std::string saveToJson() const;

    /**
     * Clear all tiles
     */
    void clear();

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getTileSize() const { return TILE_SIZE; }

private:
    int width;
    int height;
    std::vector<Tile> tiles;

    int toIndex(int x, int y) const { return y * width + x; }
};

} // namespace Game
} // namespace Penumbra
