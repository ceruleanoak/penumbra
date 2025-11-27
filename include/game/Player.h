#pragma once

#include "core/Math.h"
#include <string>

namespace Penumbra {
namespace Game {

// Forward declarations
class TileGrid;

/**
 * Player movement state
 */
enum class PlayerState {
    Idle,
    Walking,
    Jumping,
    Falling,
    Climbing,
    Dead
};

/**
 * Player entity with physics and input handling
 */
class Player {
public:
    Player();

    /**
     * Initialize player at position
     */
    void initialize(float x, float y);

    /**
     * Update player physics and state
     * @param deltaTime Time elapsed since last update
     * @param grid Tile grid for collision checking
     */
    void update(float deltaTime, const TileGrid& grid);

    /**
     * Handle keyboard input
     */
    void handleInput(bool left, bool right, bool jump, bool down);

    /**
     * Get player's collision bounds
     */
    Math::AABB getBounds() const;

    /**
     * Get player's render position (center of sprite)
     */
    Math::Vec2 getPosition() const { return position; }

    /**
     * Get player's current state
     */
    PlayerState getState() const { return state; }

    /**
     * Check if player is on ground
     */
    bool isOnGround() const { return onGround; }

    /**
     * Set player position (teleport)
     */
    void setPosition(float x, float y);

    /**
     * Apply damage to player
     */
    void takeDamage(int amount);

    /**
     * Heal player
     */
    void heal(int amount);

    /**
     * Get player health
     */
    int getHealth() const { return health; }

    /**
     * Get maximum health
     */
    int getMaxHealth() const { return maxHealth; }

    /**
     * Check if player is alive
     */
    bool isAlive() const { return health > 0 && state != PlayerState::Dead; }

    /**
     * Respawn player at checkpoint
     */
    void respawn(float x, float y);

    /**
     * Serialize player state to JSON
     */
    std::string saveToJson() const;

    /**
     * Deserialize player state from JSON
     */
    bool loadFromJson(const std::string& jsonData);

private:
    // Physics constants
    static constexpr float MOVE_SPEED = 120.0f;
    static constexpr float JUMP_FORCE = 300.0f;
    static constexpr float GRAVITY = 800.0f;
    static constexpr float MAX_FALL_SPEED = 400.0f;
    static constexpr float GROUND_FRICTION = 0.8f;
    static constexpr float AIR_FRICTION = 0.95f;

    // Dimensions
    static constexpr float PLAYER_WIDTH = 12.0f;
    static constexpr float PLAYER_HEIGHT = 16.0f;

    // State
    Math::Vec2 position;
    Math::Vec2 velocity;
    PlayerState state;
    bool onGround;
    bool facingRight;
    float coyoteTime;

    // Stats
    int health;
    int maxHealth;

    // Internal methods
    void updatePhysics(float deltaTime, const TileGrid& grid);
    void updateState();
    void resolveCollisions(const TileGrid& grid);
    bool checkGroundCollision(const TileGrid& grid);
};

} // namespace Game
} // namespace Penumbra
