#pragma once

#include <string>
#include <unordered_map>
#include <memory>

namespace Penumbra {
namespace Resources {

/**
 * Forward declarations
 */
class Texture;
class Shader;
class Sound;

/**
 * Resource manager for loading and caching game assets
 * Implements singleton pattern for global access
 */
class ResourceManager {
public:
    /**
     * Get singleton instance
     */
    static ResourceManager& getInstance();

    /**
     * Initialize resource manager with base asset path
     */
    void initialize(const std::string& assetBasePath);

    /**
     * Load and cache texture from file
     * @param name Identifier for the texture
     * @param path Path relative to asset base path
     * @return Pointer to loaded texture, or nullptr on failure
     */
    Texture* loadTexture(const std::string& name, const std::string& path);

    /**
     * Get cached texture by name
     * @return Pointer to texture, or nullptr if not found
     */
    Texture* getTexture(const std::string& name);

    /**
     * Load and compile shader program
     * @param name Identifier for the shader
     * @param vertexPath Path to vertex shader
     * @param fragmentPath Path to fragment shader
     * @return Pointer to compiled shader, or nullptr on failure
     */
    Shader* loadShader(const std::string& name,
                       const std::string& vertexPath,
                       const std::string& fragmentPath);

    /**
     * Get cached shader by name
     * @return Pointer to shader, or nullptr if not found
     */
    Shader* getShader(const std::string& name);

    /**
     * Load sound from file
     * @param name Identifier for the sound
     * @param path Path relative to asset base path
     * @return Pointer to loaded sound, or nullptr on failure
     */
    Sound* loadSound(const std::string& name, const std::string& path);

    /**
     * Get cached sound by name
     * @return Pointer to sound, or nullptr if not found
     */
    Sound* getSound(const std::string& name);

    /**
     * Clear all cached resources
     */
    void clearAll();

    /**
     * Clear resources of specific type
     */
    void clearTextures();
    void clearShaders();
    void clearSounds();

private:
    ResourceManager() = default;
    ~ResourceManager() = default;
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    std::string basePath;
    std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
    std::unordered_map<std::string, std::unique_ptr<Shader>> shaders;
    std::unordered_map<std::string, std::unique_ptr<Sound>> sounds;
};

/**
 * Texture resource representation
 */
class Texture {
public:
    Texture() : textureID(0), width(0), height(0) {}
    ~Texture();

    bool loadFromFile(const std::string& path);
    void bind() const;
    void unbind() const;

    unsigned int getID() const { return textureID; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    unsigned int textureID;
    int width;
    int height;
};

/**
 * Shader program resource
 */
class Shader {
public:
    Shader() : programID(0) {}
    ~Shader();

    bool loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
    void use() const;

    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, float x, float y) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setVec4(const std::string& name, float x, float y, float z, float w) const;
    void setMat4(const std::string& name, const float* value) const;

    unsigned int getID() const { return programID; }

private:
    unsigned int programID;
    bool compileShader(const std::string& source, unsigned int type, unsigned int& outID);
    bool linkProgram(unsigned int vertexID, unsigned int fragmentID);
};

/**
 * Sound resource (placeholder for audio system)
 */
class Sound {
public:
    Sound() : soundID(0) {}
    ~Sound() = default;

    bool loadFromFile(const std::string& path);
    void play();
    void stop();

private:
    unsigned int soundID;
};

} // namespace Resources
} // namespace Penumbra
