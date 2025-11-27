#pragma once

#include "core/Math.h"

namespace Penumbra {
namespace Rendering {

/**
 * Camera follow modes
 */
enum class CameraMode {
    Fixed,
    FollowPlayer,
    Lerp,
    DeadZone
};

/**
 * 2D Camera for game rendering
 * Manages view matrix and viewport transformations
 */
class Camera {
public:
    Camera();
    Camera(float viewportWidth, float viewportHeight);

    /**
     * Initialize camera with viewport dimensions
     */
    void initialize(float viewportWidth, float viewportHeight);

    /**
     * Update camera position and following logic
     */
    void update(float deltaTime);

    /**
     * Set camera position directly
     */
    void setPosition(float x, float y);
    void setPosition(const Math::Vec2& position);

    /**
     * Get camera position
     */
    Math::Vec2 getPosition() const { return position; }

    /**
     * Set target for camera to follow
     */
    void setTarget(const Math::Vec2& target);

    /**
     * Set camera follow mode
     */
    void setMode(CameraMode mode) { this->mode = mode; }

    /**
     * Set lerp speed for smooth following (0-1, higher = faster)
     */
    void setLerpSpeed(float speed) { lerpSpeed = Math::clamp(speed, 0.0f, 1.0f); }

    /**
     * Set dead zone for DeadZone camera mode
     * @param width Width of dead zone in pixels
     * @param height Height of dead zone in pixels
     */
    void setDeadZone(float width, float height);

    /**
     * Set camera bounds (limits where camera can move)
     */
    void setBounds(float minX, float minY, float maxX, float maxY);

    /**
     * Clear camera bounds (unlimited movement)
     */
    void clearBounds();

    /**
     * Get view matrix for rendering
     */
    Math::Mat4 getViewMatrix() const;

    /**
     * Get projection matrix for rendering
     */
    Math::Mat4 getProjectionMatrix() const;

    /**
     * Set zoom level (1.0 = normal, >1.0 = zoom in, <1.0 = zoom out)
     */
    void setZoom(float zoom);

    /**
     * Get current zoom level
     */
    float getZoom() const { return zoom; }

    /**
     * Convert screen coordinates to world coordinates
     */
    Math::Vec2 screenToWorld(float screenX, float screenY) const;

    /**
     * Convert world coordinates to screen coordinates
     */
    Math::Vec2 worldToScreen(float worldX, float worldY) const;

    /**
     * Get viewport dimensions
     */
    float getViewportWidth() const { return viewportWidth; }
    float getViewportHeight() const { return viewportHeight; }

    /**
     * Update viewport dimensions (call when window resizes)
     */
    void setViewportSize(float width, float height);

    /**
     * Shake camera for impact effects
     */
    void shake(float intensity, float duration);

private:
    Math::Vec2 position;
    Math::Vec2 targetPosition;
    CameraMode mode;
    float lerpSpeed;

    // Dead zone parameters
    Math::Vec2 deadZoneSize;

    // Bounds
    bool hasBounds;
    Math::Vec2 boundsMin;
    Math::Vec2 boundsMax;

    // Viewport
    float viewportWidth;
    float viewportHeight;
    float zoom;

    // Camera shake
    float shakeIntensity;
    float shakeDuration;
    float shakeTimer;

    void applyBounds();
    Math::Vec2 getShakeOffset() const;
};

} // namespace Rendering
} // namespace Penumbra
