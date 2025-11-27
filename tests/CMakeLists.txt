# Test configuration for Penumbra

# Find GTest
find_package(GTest REQUIRED)

# Enable test discovery
include(GoogleTest)

# Common test sources (if we have utility functions for tests)
set(TEST_COMMON_SOURCES
    # Add common test utilities here if needed
)

# Math and Core tests
add_executable(core_tests
    core_test.cpp
    ${TEST_COMMON_SOURCES}
)

target_include_directories(core_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(core_tests PRIVATE
    GTest::gtest
    GTest::gtest_main
    glm::glm
)

gtest_discover_tests(core_tests)

# Rendering tests
add_executable(rendering_tests
    rendering_test.cpp
    ${TEST_COMMON_SOURCES}
)

target_include_directories(rendering_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(rendering_tests PRIVATE
    GTest::gtest
    GTest::gtest_main
    glm::glm
    OpenGL::GL
)

if(APPLE)
    target_link_libraries(rendering_tests PRIVATE
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
endif()

gtest_discover_tests(rendering_tests)

# Physics and game logic tests
add_executable(physics_tests
    physics_test.cpp
    ${TEST_COMMON_SOURCES}
)

target_include_directories(physics_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(physics_tests PRIVATE
    GTest::gtest
    GTest::gtest_main
    glm::glm
)

gtest_discover_tests(physics_tests)

# System tests (SaveSystem, RoomSystem, etc.)
add_executable(system_tests
    system_test.cpp
    ${TEST_COMMON_SOURCES}
)

target_include_directories(system_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(system_tests PRIVATE
    GTest::gtest
    GTest::gtest_main
    glm::glm
    nlohmann_json::nlohmann_json
)

gtest_discover_tests(system_tests)

# Add custom target to run all tests
add_custom_target(run_all_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS core_tests rendering_tests physics_tests system_tests
    COMMENT "Running all tests"
)
