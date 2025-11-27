#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace Penumbra {
namespace Systems {

/**
 * Save slot metadata
 */
struct SaveSlot {
    int slotIndex;
    std::string timestamp;
    std::string currentRoom;
    int playerHealth;
    int totalPlayTime;
    float completionPercentage;
    bool isValid;

    SaveSlot() : slotIndex(0), playerHealth(0), totalPlayTime(0),
                 completionPercentage(0.0f), isValid(false) {}
};

/**
 * Complete save game data
 */
struct SaveData {
    // Player state
    std::string currentRoomID;
    float playerX;
    float playerY;
    int playerHealth;
    int playerMaxHealth;

    // Game progress
    std::vector<std::string> discoveredRooms;
    std::vector<std::string> defeatedBosses;
    std::vector<std::string> collectedItems;
    std::vector<std::string> unlockedAbilities;

    // Statistics
    int totalPlayTime;
    int enemiesDefeated;
    int deathCount;

    // Metadata
    std::string timestamp;
    int saveVersion;

    SaveData() : playerX(0), playerY(0), playerHealth(100), playerMaxHealth(100),
                 totalPlayTime(0), enemiesDefeated(0), deathCount(0), saveVersion(1) {}
};

/**
 * Save system for game state persistence
 */
class SaveSystem {
public:
    static constexpr int MAX_SAVE_SLOTS = 3;
    static constexpr int CURRENT_SAVE_VERSION = 1;

    /**
     * Initialize save system
     */
    static void initialize();

    /**
     * Save game to slot
     * @param slotIndex Save slot (0-2)
     * @param data Game data to save
     * @return true if save successful
     */
    static bool saveGame(int slotIndex, const SaveData& data);

    /**
     * Load game from slot
     * @param slotIndex Save slot (0-2)
     * @param outData Output parameter for loaded data
     * @return true if load successful
     */
    static bool loadGame(int slotIndex, SaveData& outData);

    /**
     * Delete save in slot
     */
    static bool deleteSave(int slotIndex);

    /**
     * Check if save slot has valid data
     */
    static bool hasSave(int slotIndex);

    /**
     * Get save slot metadata
     */
    static SaveSlot getSaveSlotInfo(int slotIndex);

    /**
     * Get all save slots
     */
    static std::vector<SaveSlot> getAllSaveSlots();

    /**
     * Quick save to autosave slot
     */
    static bool quickSave(const SaveData& data);

    /**
     * Load from autosave slot
     */
    static bool quickLoad(SaveData& outData);

    /**
     * Export save data to JSON string
     */
    static std::string exportSaveToJson(const SaveData& data);

    /**
     * Import save data from JSON string
     */
    static bool importSaveFromJson(const std::string& jsonData, SaveData& outData);

    /**
     * Get save file path for slot
     */
    static std::string getSaveFilePath(int slotIndex);

    /**
     * Get autosave file path
     */
    static std::string getAutoSaveFilePath();

    /**
     * Backup save file
     */
    static bool backupSave(int slotIndex);

    /**
     * Restore save from backup
     */
    static bool restoreSave(int slotIndex);

private:
    static std::string getSaveDirectory();
    static bool ensureSaveDirectoryExists();
    static nlohmann::json saveDataToJson(const SaveData& data);
    static bool jsonToSaveData(const nlohmann::json& json, SaveData& outData);
    static bool validateSaveData(const nlohmann::json& json);
};

} // namespace Systems
} // namespace Penumbra
