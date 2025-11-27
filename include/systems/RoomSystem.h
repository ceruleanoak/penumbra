#pragma once

#include "game/TileGrid.h"
#include "game/Enemy.h"
#include "game/Platform.h"
#include "core/Math.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace Penumbra {
namespace Systems {

/**
 * Room transition direction
 */
enum class TransitionDirection {
    None,
    North,
    South,
    East,
    West
};

/**
 * Room data structure
 * Contains tile grid, entities, and metadata
 */
struct Room {
    std::string id;
    std::string name;
    Game::TileGrid tileGrid;
    std::vector<std::unique_ptr<Game::Enemy>> enemies;
    std::vector<std::unique_ptr<Game::Platform>> platforms;
    Math::Vec2 playerSpawnPoint;

    // Room connections
    std::string northRoom;
    std::string southRoom;
    std::string eastRoom;
    std::string westRoom;

    // Metadata
    Math::Color backgroundColor;
    std::string musicTrack;
    bool discovered;

    Room() : discovered(false) {}
};

/**
 * Room system managing level layout and transitions
 */
class RoomSystem {
public:
    RoomSystem();

    /**
     * Initialize room system
     */
    void initialize();

    /**
     * Load room from JSON file
     * @param roomID Unique identifier for the room
     * @param jsonPath Path to room JSON file
     * @return true if room loaded successfully
     */
    bool loadRoom(const std::string& roomID, const std::string& jsonPath);

    /**
     * Load room from JSON data string
     */
    bool loadRoomFromJson(const std::string& roomID, const std::string& jsonData);

    /**
     * Save room to JSON file
     */
    bool saveRoom(const std::string& roomID, const std::string& jsonPath) const;

    /**
     * Get room data by ID
     */
    Room* getRoom(const std::string& roomID);
    const Room* getRoom(const std::string& roomID) const;

    /**
     * Set current active room
     */
    bool setCurrentRoom(const std::string& roomID);

    /**
     * Get current active room
     */
    Room* getCurrentRoom() { return currentRoom; }
    const Room* getCurrentRoom() const { return currentRoom; }

    /**
     * Get current room ID
     */
    std::string getCurrentRoomID() const { return currentRoomID; }

    /**
     * Check if player should transition to adjacent room
     * @param playerPos Player position in world coordinates
     * @return Transition direction if transition should occur
     */
    TransitionDirection checkTransition(const Math::Vec2& playerPos) const;

    /**
     * Transition to adjacent room
     * @param direction Direction of transition
     * @param outSpawnPos Output parameter for player spawn position in new room
     * @return true if transition successful
     */
    bool transitionRoom(TransitionDirection direction, Math::Vec2& outSpawnPos);

    /**
     * Create new empty room
     */
    void createRoom(const std::string& roomID, int width, int height);

    /**
     * Remove room from system
     */
    void removeRoom(const std::string& roomID);

    /**
     * Link two rooms together
     */
    void linkRooms(const std::string& roomA, const std::string& roomB,
                   TransitionDirection directionFromA);

    /**
     * Get list of all room IDs
     */
    std::vector<std::string> getRoomIDs() const;

    /**
     * Check if room exists
     */
    bool hasRoom(const std::string& roomID) const;

    /**
     * Clear all rooms
     */
    void clear();

    /**
     * Update current room entities
     */
    void update(float deltaTime);

    /**
     * Mark room as discovered
     */
    void markDiscovered(const std::string& roomID);

    /**
     * Check if room has been discovered
     */
    bool isDiscovered(const std::string& roomID) const;

private:
    std::unordered_map<std::string, std::unique_ptr<Room>> rooms;
    Room* currentRoom;
    std::string currentRoomID;

    std::string getRoomInDirection(const std::string& fromRoom,
                                   TransitionDirection direction) const;
    Math::Vec2 getSpawnPositionForTransition(TransitionDirection direction,
                                             const Room* targetRoom) const;
};

} // namespace Systems
} // namespace Penumbra
