#pragma once

#include "game/Enemy.h"
#include "game/Platform.h"
#include <memory>
#include <string>
#include <nlohmann/json.hpp>

namespace Penumbra {
namespace Systems {

/**
 * Factory for creating game objects from JSON data
 * Provides unified interface for entity creation and serialization
 */
class ObjectFactory {
public:
    /**
     * Create enemy from JSON object
     * @param json JSON object containing enemy data
     * @return Unique pointer to created enemy, or nullptr on failure
     */
    static std::unique_ptr<Game::Enemy> createEnemy(const nlohmann::json& json);

    /**
     * Create enemy from type and position
     * @param type Enemy behavior type string ("patrol", "chase", "guard", "fly")
     * @param x X position
     * @param y Y position
     * @return Unique pointer to created enemy
     */
    static std::unique_ptr<Game::Enemy> createEnemy(const std::string& type, float x, float y);

    /**
     * Serialize enemy to JSON
     */
    static nlohmann::json enemyToJson(const Game::Enemy& enemy);

    /**
     * Create platform from JSON object
     * @param json JSON object containing platform data
     * @return Unique pointer to created platform, or nullptr on failure
     */
    static std::unique_ptr<Game::Platform> createPlatform(const nlohmann::json& json);

    /**
     * Create static platform
     * @param x X position
     * @param y Y position
     * @param width Platform width
     * @param height Platform height
     * @return Unique pointer to created platform
     */
    static std::unique_ptr<Game::Platform> createStaticPlatform(float x, float y,
                                                                  float width, float height);

    /**
     * Create moving platform
     * @param x Starting X position
     * @param y Starting Y position
     * @param width Platform width
     * @param height Platform height
     * @param endX End X position
     * @param endY End Y position
     * @param speed Movement speed
     * @return Unique pointer to created platform
     */
    static std::unique_ptr<Game::Platform> createMovingPlatform(float x, float y,
                                                                 float width, float height,
                                                                 float endX, float endY,
                                                                 float speed);

    /**
     * Serialize platform to JSON
     */
    static nlohmann::json platformToJson(const Game::Platform& platform);

    /**
     * Parse enemy behavior from string
     */
    static Game::EnemyBehavior parseEnemyBehavior(const std::string& behaviorStr);

    /**
     * Convert enemy behavior to string
     */
    static std::string enemyBehaviorToString(Game::EnemyBehavior behavior);

    /**
     * Parse platform pattern from string
     */
    static Game::PlatformPattern parsePlatformPattern(const std::string& patternStr);

    /**
     * Convert platform pattern to string
     */
    static std::string platformPatternToString(Game::PlatformPattern pattern);

    /**
     * Create object from generic JSON with type field
     * Automatically determines object type and creates appropriate entity
     */
    static std::unique_ptr<void> createFromJson(const nlohmann::json& json, std::string& outType);

    /**
     * Batch create entities from JSON array
     * @param jsonArray Array of entity JSON objects
     * @param outEnemies Output vector for created enemies
     * @param outPlatforms Output vector for created platforms
     * @return Number of entities successfully created
     */
    static int createBatchFromJson(const nlohmann::json& jsonArray,
                                    std::vector<std::unique_ptr<Game::Enemy>>& outEnemies,
                                    std::vector<std::unique_ptr<Game::Platform>>& outPlatforms);

    /**
     * Validate JSON object has required fields for entity type
     */
    static bool validateEnemyJson(const nlohmann::json& json);
    static bool validatePlatformJson(const nlohmann::json& json);

private:
    ObjectFactory() = delete;
};

} // namespace Systems
} // namespace Penumbra
