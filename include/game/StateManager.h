#pragma once

#include <memory>
#include <vector>
#include <string>

namespace Penumbra {
namespace Game {

/**
 * Game state types
 */
enum class GameStateType {
    MainMenu,
    Playing,
    Paused,
    GameOver,
    Loading
};

/**
 * Base class for game states
 */
class GameState {
public:
    virtual ~GameState() = default;

    /**
     * Called when state becomes active
     */
    virtual void onEnter() = 0;

    /**
     * Called when state becomes inactive
     */
    virtual void onExit() = 0;

    /**
     * Update state logic
     */
    virtual void update(float deltaTime) = 0;

    /**
     * Render state
     */
    virtual void render() = 0;

    /**
     * Handle input events
     */
    virtual void handleInput() = 0;

    /**
     * Get state type
     */
    virtual GameStateType getType() const = 0;

    /**
     * Check if state blocks updates to states below it in stack
     */
    virtual bool blocksUpdate() const { return true; }

    /**
     * Check if state blocks rendering of states below it in stack
     */
    virtual bool blocksRender() const { return true; }
};

/**
 * Manages game state stack and transitions
 */
class StateManager {
public:
    StateManager();

    /**
     * Initialize state manager
     */
    void initialize();

    /**
     * Update active states
     */
    void update(float deltaTime);

    /**
     * Render active states
     */
    void render();

    /**
     * Handle input for active states
     */
    void handleInput();

    /**
     * Push new state onto stack
     */
    void pushState(std::unique_ptr<GameState> state);

    /**
     * Pop current state from stack
     */
    void popState();

    /**
     * Pop all states and push new one (state change)
     */
    void changeState(std::unique_ptr<GameState> state);

    /**
     * Get current active state
     */
    GameState* getCurrentState() const;

    /**
     * Get state type of current state
     */
    GameStateType getCurrentStateType() const;

    /**
     * Check if state stack is empty
     */
    bool isEmpty() const { return states.empty(); }

    /**
     * Clear all states
     */
    void clear();

private:
    std::vector<std::unique_ptr<GameState>> states;
    std::vector<std::unique_ptr<GameState>> pendingPush;
    int pendingPops;
    bool shouldClear;

    void processPendingChanges();
};

} // namespace Game
} // namespace Penumbra
