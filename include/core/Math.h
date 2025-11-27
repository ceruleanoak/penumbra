#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

namespace Penumbra {
namespace Math {

/**
 * 2D Vector type alias for convenience
 */
using Vec2 = glm::vec2;

/**
 * 3D Vector type alias for convenience
 */
using Vec3 = glm::vec3;

/**
 * 4D Vector type alias for convenience
 */
using Vec4 = glm::vec4;

/**
 * 4x4 Matrix type alias for convenience
 */
using Mat4 = glm::mat4;

/**
 * Axis-Aligned Bounding Box for collision detection
 */
struct AABB {
    Vec2 min;
    Vec2 max;

    AABB() : min(0.0f), max(0.0f) {}
    AABB(const Vec2& min, const Vec2& max) : min(min), max(max) {}
    AABB(float x, float y, float width, float height)
        : min(x, y), max(x + width, y + height) {}

    float width() const { return max.x - min.x; }
    float height() const { return max.y - min.y; }
    Vec2 center() const { return (min + max) * 0.5f; }

    bool contains(const Vec2& point) const {
        return point.x >= min.x && point.x <= max.x &&
               point.y >= min.y && point.y <= max.y;
    }

    bool intersects(const AABB& other) const {
        return min.x <= other.max.x && max.x >= other.min.x &&
               min.y <= other.max.y && max.y >= other.min.y;
    }
};

/**
 * Rectangle structure for sprite rendering
 */
struct Rect {
    float x, y, width, height;

    Rect() : x(0), y(0), width(0), height(0) {}
    Rect(float x, float y, float w, float h) : x(x), y(y), width(w), height(h) {}

    AABB toAABB() const { return AABB(x, y, width, height); }
};

/**
 * Color structure (RGBA)
 */
struct Color {
    float r, g, b, a;

    Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
    Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}

    Vec4 toVec4() const { return Vec4(r, g, b, a); }

    static const Color White;
    static const Color Black;
    static const Color Red;
    static const Color Green;
    static const Color Blue;
    static const Color Yellow;
    static const Color Transparent;
};

/**
 * Linear interpolation between two values
 */
template<typename T>
inline T lerp(const T& a, const T& b, float t) {
    return a + (b - a) * t;
}

/**
 * Clamp value between min and max
 */
template<typename T>
inline T clamp(const T& value, const T& min, const T& max) {
    return value < min ? min : (value > max ? max : value);
}

/**
 * Check if two floating point values are approximately equal
 */
inline bool approxEqual(float a, float b, float epsilon = 1e-5f) {
    return std::abs(a - b) < epsilon;
}

/**
 * Convert degrees to radians
 */
inline float toRadians(float degrees) {
    return degrees * (3.14159265359f / 180.0f);
}

/**
 * Convert radians to degrees
 */
inline float toDegrees(float radians) {
    return radians * (180.0f / 3.14159265359f);
}

} // namespace Math
} // namespace Penumbra
