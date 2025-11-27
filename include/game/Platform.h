#pragma once

#include "core/Math.h"
#include <string>

namespace Penumbra {
namespace Game {

/**
 * Platform movement pattern types
 */
enum class PlatformPattern {
    Static,
    LinearLoop,
    PingPong,
    Circular,
    PathFollow
};

/**
 * Moving platform entity
 */
class Platform {
public:
    Platform();
    Platform(float x, float y, float width, float height);

    /**
     * Update platform position based on movement pattern
     */
    void update(float deltaTime);

    /**
     * Get platform collision bounds
     */
    Math::AABB getBounds() const;

    /**
     * Get platform position
     */
    Math::Vec2 getPosition() const { return position; }

    /**
     * Get platform velocity (for moving player with platform)
     */
    Math::Vec2 getVelocity() const { return velocity; }

    /**
     * Set movement pattern
     */
    void setPattern(PlatformPattern pattern);

    /**
     * Set linear movement parameters
     * @param startPos Starting position
     * @param endPos Ending position
     * @param speed Movement speed in pixels per second
     */
    void setLinearMovement(const Math::Vec2& startPos, const Math::Vec2& endPos, float speed);

    /**
     * Set circular movement parameters
     * @param center Center of circular path
     * @param radius Radius of circle
     * @param angularSpeed Angular speed in radians per second
     */
    void setCircularMovement(const Math::Vec2& center, float radius, float angularSpeed);

    /**
     * Check if platform is active
     */
    bool isActive() const { return active; }

    /**
     * Set platform active state
     */
    void setActive(bool active) { this->active = active; }

    /**
     * Serialize to JSON
     */
    std::string saveToJson() const;

    /**
     * Deserialize from JSON
     */
    bool loadFromJson(const std::string& jsonData);

private:
    Math::Vec2 position;
    Math::Vec2 size;
    Math::Vec2 velocity;

    PlatformPattern pattern;
    bool active;

    // Linear movement state
    Math::Vec2 startPosition;
    Math::Vec2 endPosition;
    float moveSpeed;
    float movementProgress;
    bool movingForward;

    // Circular movement state
    Math::Vec2 circleCenter;
    float circleRadius;
    float angularSpeed;
    float currentAngle;

    void updateLinearMovement(float deltaTime);
    void updateCircularMovement(float deltaTime);
};

} // namespace Game
} // namespace Penumbra
