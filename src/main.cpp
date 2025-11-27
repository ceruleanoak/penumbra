#include <SDL2/SDL.h>
#include <iostream>
#include <string>

// OpenGL headers - macOS specific
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#endif

// Global constants
constexpr int SCREEN_WIDTH = 1024;
constexpr int SCREEN_HEIGHT = 768;
constexpr const char* WINDOW_TITLE = "PENUMBRA";

/**
 * Initialize SDL2 and create window with OpenGL context
 *
 * @param window Output parameter for created window
 * @param context Output parameter for OpenGL context
 * @return true if initialization successful, false otherwise
 */
bool initSDL(SDL_Window** window, SDL_GLContext* context)
{
    // Initialize SDL2 video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return false;
    }

    // Request OpenGL 3.3 Core Profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Enable double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // macOS specific: request forward-compatible context
    #ifdef __APPLE__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    #endif

    // Create window
    *window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );

    if (*window == nullptr)
    {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    // Create OpenGL context
    *context = SDL_GL_CreateContext(*window);
    if (*context == nullptr)
    {
        std::cerr << "OpenGL context creation failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return false;
    }

    // Enable VSync
    SDL_GL_SetSwapInterval(1);

    // Print OpenGL version info
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

    return true;
}

/**
 * Handle input events
 *
 * @param event SDL event to process
 * @return false if quit requested, true otherwise
 */
bool handleEvents(const SDL_Event& event)
{
    switch (event.type)
    {
        case SDL_QUIT:
            return false;

        case SDL_KEYDOWN:
            // ESC key to quit
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                return false;
            }
            // TODO: Phase-specific input handling will be added here
            // - Player movement (arrow keys, WASD)
            // - Action keys (Space, Shift)
            // - Debug commands
            break;

        case SDL_MOUSEBUTTONDOWN:
            // TODO: Mouse interaction for menu/UI
            break;

        default:
            break;
    }

    return true;
}

/**
 * Update game state
 *
 * @param deltaTime Time elapsed since last frame (seconds)
 */
void update(float deltaTime)
{
    // TODO: Phase 1 - No game logic yet, just rendering
    // TODO: Phase 2 - Player physics and collision
    // TODO: Phase 3 - Enemy AI and systems
    // TODO: Phase 4 - Room transitions and state management
    (void)deltaTime; // Suppress unused parameter warning
}

/**
 * Render frame
 */
void render()
{
    // Clear screen to dark gray (temporary - will be replaced with actual rendering)
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // TODO: Phase 1 - Sprite batch rendering
    // - Render TileGrid
    // - Render Player sprite
    // - Render Platform sprites
    // - Render UI overlay

    // TODO: Phase 2 - Add camera following player

    // TODO: Phase 3 - Add enemy rendering

    // TODO: Phase 4 - Add room transition effects
}

/**
 * Main game loop
 */
int main(int argc, char* argv[])
{
    (void)argc; // Suppress unused parameter warnings
    (void)argv;

    SDL_Window* window = nullptr;
    SDL_GLContext context = nullptr;

    // Initialize SDL and OpenGL
    if (!initSDL(&window, &context))
    {
        return 1;
    }

    std::cout << "PENUMBRA initialized successfully" << std::endl;
    std::cout << "Press ESC to quit" << std::endl;

    // Game loop state
    bool running = true;
    Uint64 lastTime = SDL_GetPerformanceCounter();
    const Uint64 perfFreq = SDL_GetPerformanceFrequency();

    // Main game loop
    while (running)
    {
        // Calculate delta time
        Uint64 currentTime = SDL_GetPerformanceCounter();
        float deltaTime = static_cast<float>(currentTime - lastTime) / static_cast<float>(perfFreq);
        lastTime = currentTime;

        // Process events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (!handleEvents(event))
            {
                running = false;
            }
        }

        // Update game state
        update(deltaTime);

        // Render frame
        render();

        // Swap buffers
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    std::cout << "Shutting down..." << std::endl;
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
