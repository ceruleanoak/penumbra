cmake_minimum_required(VERSION 3.20)
project(penumbra VERSION 1.0.0 LANGUAGES CXX)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# macOS-specific settings
if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum macOS deployment version")
    # OpenGL framework on macOS
    find_package(OpenGL REQUIRED)
endif()

# Compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
    # macOS clang needs explicit include paths for libc++
    if(APPLE)
        include_directories(/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/c++/v1)
        include_directories(/Library/Developer/CommandLineTools/usr/lib/clang/16/include)
        include_directories(/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include)
    endif()
endif()

# Conan package manager setup
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

# Find required packages
find_package(SDL2 REQUIRED)
find_package(glm REQUIRED)
find_package(nlohmann_json REQUIRED)

# Main executable sources
set(PENUMBRA_SOURCES
    src/main.cpp
)

# Main executable
add_executable(penumbra ${PENUMBRA_SOURCES})

# Include directories
target_include_directories(penumbra PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(penumbra PRIVATE
    SDL2::SDL2
    glm::glm
    nlohmann_json::nlohmann_json
)

# macOS OpenGL linking
if(APPLE)
    target_link_libraries(penumbra PRIVATE OpenGL::GL)
endif()

# Copy assets to build directory
add_custom_command(TARGET penumbra POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:penumbra>/assets
    COMMENT "Copying assets to build directory"
)

# Enable testing (disabled until GTest build is fixed)
# enable_testing()
# add_subdirectory(tests)

# Install targets
install(TARGETS penumbra DESTINATION bin)
install(DIRECTORY assets DESTINATION share/penumbra)
