#include <cstdio>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include "VulkanRenderer.h"

constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 480;

const bool DEBUG = true;

int main()
{
    int current_screen_width = SCREEN_WIDTH, current_screen_height = SCREEN_HEIGHT;
    const char* dropped_file_name;

    SDL_Window* window = nullptr;
    SDL_Surface* screen_surface = nullptr;
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    window = SDL_CreateWindow("SDL Tutorial", current_screen_width, current_screen_height,
                              SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);
    if (!window)
    {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return -2;
    }

    VulkanRenderer renderer = VulkanRenderer(window);
    VkInstance vk_instance = renderer.getInstance();

    screen_surface = SDL_GetWindowSurface(window);
    if (!screen_surface)
    {
        fprintf(stderr, "Window surface could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -2;
    }


    const int rect_width = current_screen_width / 2, rectHeight = current_screen_height / 2;
    int posx = (current_screen_width - rect_width) / 2, posy = (current_screen_height - rectHeight) / 2;
    SDL_Rect rect = {posx, posy, rect_width, rectHeight};

    SDL_FillSurfaceRect(screen_surface, &rect, 0xfffff000);

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
            case SDL_EVENT_WINDOW_RESIZED:
                SDL_GetWindowSize(window, &current_screen_width, &current_screen_height);
                printf("Window resized HEIHT: %d, WIDTH: %d \n", current_screen_width, current_screen_height);
                break;
            case SDL_EVENT_DROP_FILE:
                dropped_file_name = e.drop.data;
                printf("Dropped file: %s\n", dropped_file_name);
                break;
            default: ;
            }
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    printf("Window destroyed\n");
    return 0;
}
