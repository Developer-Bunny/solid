#include <cstdio>
#include <stdexcept>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include "VulkanRenderer.h"

constexpr unsigned int SCREEN_WIDTH = 640;
constexpr unsigned int SCREEN_HEIGHT = 480;

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Window* window = nullptr;
    window = SDL_CreateWindow("SDL Tutorial", SCREEN_WIDTH, SCREEN_HEIGHT,
                              SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);
    if (!window)
    {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return -2;
    }

    uint32_t sdl_extensions_count = 0;
    const char* const* sdl_extensions = SDL_Vulkan_GetInstanceExtensions(&sdl_extensions_count);
    std::vector<const char*> extensions(sdl_extensions_count);
    for (uint32_t n = 0; n < sdl_extensions_count; n++)
        extensions[n] = sdl_extensions[n];

    auto* renderer = new VulkanRenderer();
    try
    {
        renderer->createInstance(extensions);
    }
    catch (std::exception& e)
    {
        fprintf(stderr, "Error: %s\n", e.what());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    VkSurfaceKHR surface = VK_NULL_HANDLE;
    if (!SDL_Vulkan_CreateSurface(window, renderer->getInstance(), renderer->getAllocator(), &surface))
    {
        fprintf(stderr, "Could not create Vulkan surface! SDL Error: %s\n", SDL_GetError());
        delete renderer;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -2;
    }

    int width, height;
    SDL_GetWindowSize(window, &width, &height);

    SDL_UpdateWindowSurface(window);
    SDL_Event e;
    bool quit = false;

    // Main loop
    while (!quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            switch (e.type)
            {
            case SDL_EVENT_QUIT:
                quit = true;
                break;
            default: ;
            }
        }
    }

    // Verificación antes de destruir `surface`
    if (surface != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(renderer->getInstance(), surface, renderer->getAllocator());
    }
    delete renderer;
    SDL_DestroyWindow(window);
    SDL_Quit();

    printf("Window destroyed\n");
    return 0;
}
