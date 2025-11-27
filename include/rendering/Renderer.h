#pragma once

#include "core/Math.h"
#include "core/Resources.h"
#include <vector>

namespace Penumbra {
namespace Rendering {

// Forward declarations
class Camera;

/**
 * Sprite render data
 */
struct Sprite {
    Math::Vec2 position;
    Math::Vec2 size;
    Math::Rect textureRect;
    Math::Color color;
    float rotation;
    Math::Vec2 origin;
    int layer;

    Sprite()
        : position(0.0f, 0.0f)
        , size(16.0f, 16.0f)
        , textureRect(0.0f, 0.0f, 1.0f, 1.0f)
        , color(Math::Color::White)
        , rotation(0.0f)
        , origin(0.5f, 0.5f)
        , layer(0)
    {}
};

/**
 * Batch renderer for efficient sprite rendering
 */
class SpriteBatch {
public:
    SpriteBatch();
    ~SpriteBatch();

    /**
     * Initialize sprite batch with maximum sprite count
     */
    void initialize(size_t maxSprites = 10000);

    /**
     * Begin batching sprites
     */
    void begin(const Camera& camera, Resources::Shader* shader, Resources::Texture* texture);

    /**
     * Submit sprite to batch
     */
    void draw(const Sprite& sprite);

    /**
     * Draw sprite with explicit parameters
     */
    void draw(const Math::Vec2& position,
              const Math::Vec2& size,
              const Math::Color& color = Math::Color::White,
              float rotation = 0.0f);

    /**
     * Draw textured sprite region
     */
    void draw(const Math::Vec2& position,
              const Math::Vec2& size,
              const Math::Rect& textureRect,
              const Math::Color& color = Math::Color::White,
              float rotation = 0.0f);

    /**
     * End batching and render all sprites
     */
    void end();

    /**
     * Flush current batch immediately
     */
    void flush();

    /**
     * Get number of sprites in current batch
     */
    size_t getSpriteCount() const { return spriteCount; }

    /**
     * Get number of draw calls in last frame
     */
    size_t getDrawCalls() const { return drawCalls; }

private:
    struct Vertex {
        Math::Vec3 position;
        Math::Vec2 texCoord;
        Math::Vec4 color;
    };

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    size_t maxSprites;
    size_t spriteCount;
    size_t drawCalls;

    Resources::Shader* currentShader;
    Resources::Texture* currentTexture;
    Math::Mat4 viewProjection;

    void setupBuffers();
    void addSpriteVertices(const Sprite& sprite);
};

/**
 * Main renderer managing all rendering operations
 */
class Renderer {
public:
    Renderer();

    /**
     * Initialize renderer
     */
    void initialize(int windowWidth, int windowHeight);

    /**
     * Begin frame
     */
    void beginFrame(const Camera& camera);

    /**
     * End frame
     */
    void endFrame();

    /**
     * Get sprite batch for rendering
     */
    SpriteBatch& getSpriteBatch() { return spriteBatch; }

    /**
     * Set clear color
     */
    void setClearColor(const Math::Color& color);

    /**
     * Clear screen
     */
    void clear();

    /**
     * Draw filled rectangle
     */
    void drawRect(const Math::Rect& rect, const Math::Color& color);

    /**
     * Draw rectangle outline
     */
    void drawRectOutline(const Math::Rect& rect, const Math::Color& color, float thickness = 1.0f);

    /**
     * Draw line
     */
    void drawLine(const Math::Vec2& start, const Math::Vec2& end,
                  const Math::Color& color, float thickness = 1.0f);

    /**
     * Enable/disable debug rendering
     */
    void setDebugMode(bool enabled) { debugMode = enabled; }
    bool isDebugMode() const { return debugMode; }

    /**
     * Get render statistics
     */
    struct Stats {
        size_t drawCalls;
        size_t spritesDrawn;
        size_t verticesDrawn;
    };
    const Stats& getStats() const { return stats; }

private:
    SpriteBatch spriteBatch;
    Math::Color clearColor;
    bool debugMode;
    Stats stats;
};

} // namespace Rendering
} // namespace Penumbra
