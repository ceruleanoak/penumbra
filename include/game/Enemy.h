#pragma once

#include "core/Math.h"
#include <string>

namespace Penumbra {
namespace Game {

// Forward declarations
class TileGrid;
class Player;

/**
 * Enemy AI behavior types
 */
enum class EnemyBehavior {
    Patrol,
    Chase,
    Guard,
    Fly
};

/**
 * Enemy entity with AI and combat
 */
class Enemy {
public:
    Enemy();
    Enemy(float x, float y, EnemyBehavior behavior);

    /**
     * Update enemy AI and physics
     */
    void update(float deltaTime, const TileGrid& grid, const Player& player);

    /**
     * Get enemy collision bounds
     */
    Math::AABB getBounds() const;

    /**
     * Get enemy position
     */
    Math::Vec2 getPosition() const { return position; }

    /**
     * Set patrol path for patrol behavior
     */
    void setPatrolPath(const Math::Vec2& pointA, const Math::Vec2& pointB);

    /**
     * Set detection range for chase behavior
     */
    void setDetectionRange(float range) { detectionRange = range; }

    /**
     * Take damage
     */
    void takeDamage(int amount);

    /**
     * Check if enemy is alive
     */
    bool isAlive() const { return health > 0; }

    /**
     * Check if enemy should be removed
     */
    bool shouldRemove() const { return !isAlive() && deathTimer <= 0.0f; }

    /**
     * Get damage dealt to player on contact
     */
    int getDamage() const { return contactDamage; }

    /**
     * Serialize to JSON
     */
    std::string saveToJson() const;

    /**
     * Deserialize from JSON
     */
    bool loadFromJson(const std::string& jsonData);

private:
    // Constants
    static constexpr float PATROL_SPEED = 40.0f;
    static constexpr float CHASE_SPEED = 80.0f;
    static constexpr float GRAVITY = 600.0f;
    static constexpr float DEATH_DURATION = 1.0f;

    // State
    Math::Vec2 position;
    Math::Vec2 velocity;
    EnemyBehavior behavior;
    bool facingRight;
    int health;
    int maxHealth;
    int contactDamage;
    float deathTimer;

    // AI state
    Math::Vec2 patrolPointA;
    Math::Vec2 patrolPointB;
    bool movingToPointB;
    float detectionRange;
    bool chasingPlayer;

    // Internal methods
    void updatePatrol(float deltaTime, const TileGrid& grid);
    void updateChase(float deltaTime, const TileGrid& grid, const Player& player);
    void updateGuard(float deltaTime, const Player& player);
    void updateFly(float deltaTime, const Player& player);

    void applyGravity(float deltaTime, const TileGrid& grid);
    bool isPlayerInRange(const Player& player) const;
    void moveTowards(const Math::Vec2& target, float speed, float deltaTime);
};

} // namespace Game
} // namespace Penumbra
