#pragma once

#include <string>
#include <vector>

namespace Penumbra {
namespace Platform {

/**
 * Platform-specific file path handling
 */
class FileSystem {
public:
    /**
     * Get the application's base directory
     */
    static std::string getBasePath();

    /**
     * Get the user's data directory for save files
     */
    static std::string getUserDataPath();

    /**
     * Check if a file exists
     */
    static bool fileExists(const std::string& path);

    /**
     * Read entire file into string
     */
    static bool readFile(const std::string& path, std::string& outContent);

    /**
     * Write string to file
     */
    static bool writeFile(const std::string& path, const std::string& content);

    /**
     * List files in directory matching pattern
     */
    static std::vector<std::string> listFiles(const std::string& directory,
                                               const std::string& pattern = "*");

    /**
     * Create directory (including parent directories)
     */
    static bool createDirectory(const std::string& path);

    /**
     * Join path components with platform-specific separator
     */
    static std::string joinPath(const std::string& base, const std::string& component);

    /**
     * Get file extension from path
     */
    static std::string getExtension(const std::string& path);

    /**
     * Get filename without directory from path
     */
    static std::string getFilename(const std::string& path);
};

/**
 * Platform-specific timing utilities
 */
class Time {
public:
    /**
     * Get high-precision time in seconds since application start
     */
    static double getTime();

    /**
     * Sleep for specified milliseconds
     */
    static void sleep(unsigned int milliseconds);
};

/**
 * Platform detection macros and utilities
 */
#if defined(__APPLE__) && defined(__MACH__)
    #define PENUMBRA_PLATFORM_MACOS
    constexpr const char* PLATFORM_NAME = "macOS";
#elif defined(_WIN32) || defined(_WIN64)
    #define PENUMBRA_PLATFORM_WINDOWS
    constexpr const char* PLATFORM_NAME = "Windows";
#elif defined(__linux__)
    #define PENUMBRA_PLATFORM_LINUX
    constexpr const char* PLATFORM_NAME = "Linux";
#else
    #define PENUMBRA_PLATFORM_UNKNOWN
    constexpr const char* PLATFORM_NAME = "Unknown";
#endif

/**
 * Get platform name string
 */
inline const char* getPlatformName() {
    return PLATFORM_NAME;
}

} // namespace Platform
} // namespace Penumbra
