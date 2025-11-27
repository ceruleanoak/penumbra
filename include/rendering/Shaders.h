#pragma once

#include <string>

namespace Penumbra {
namespace Rendering {

/**
 * Built-in shader source code
 * Provides default shaders for sprite rendering
 */
namespace Shaders {

/**
 * Default vertex shader source
 */
extern const char* DEFAULT_VERTEX_SHADER;

/**
 * Default fragment shader source
 */
extern const char* DEFAULT_FRAGMENT_SHADER;

/**
 * Passthrough vertex shader (no transformations)
 */
extern const char* PASSTHROUGH_VERTEX_SHADER;

/**
 * Solid color fragment shader
 */
extern const char* SOLID_COLOR_FRAGMENT_SHADER;

/**
 * Debug wireframe fragment shader
 */
extern const char* DEBUG_FRAGMENT_SHADER;

/**
 * Utility function to compile shader from source
 * @param source Shader source code
 * @param type GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
 * @param outID Output shader ID if successful
 * @return true if compilation successful
 */
bool compileShader(const std::string& source, unsigned int type, unsigned int& outID);

/**
 * Utility function to link shader program
 * @param vertexID Compiled vertex shader ID
 * @param fragmentID Compiled fragment shader ID
 * @param outProgramID Output program ID if successful
 * @return true if linking successful
 */
bool linkProgram(unsigned int vertexID, unsigned int fragmentID, unsigned int& outProgramID);

/**
 * Create shader program from source strings
 * @param vertexSource Vertex shader source code
 * @param fragmentSource Fragment shader source code
 * @param outProgramID Output program ID if successful
 * @return true if creation successful
 */
bool createShaderProgram(const std::string& vertexSource,
                         const std::string& fragmentSource,
                         unsigned int& outProgramID);

/**
 * Load shader program from files
 * @param vertexPath Path to vertex shader file
 * @param fragmentPath Path to fragment shader file
 * @param outProgramID Output program ID if successful
 * @return true if loading successful
 */
bool loadShaderProgram(const std::string& vertexPath,
                       const std::string& fragmentPath,
                       unsigned int& outProgramID);

} // namespace Shaders
} // namespace Rendering
} // namespace Penumbra
