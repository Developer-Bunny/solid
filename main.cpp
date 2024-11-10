#include <cstdio>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>

constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 480;

int main()
{
    int currentScreenWidth = SCREEN_WIDTH, currentScreenHeight = SCREEN_HEIGHT;
    const char* droppedFileName;

    SDL_Window* window = nullptr;
    SDL_Surface* screenSurface = nullptr;
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    window = SDL_CreateWindow("SDL Tutorial", currentScreenWidth, currentScreenHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);
    if (!window)
    {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return -2;
    }




    screenSurface = SDL_GetWindowSurface(window);
    if (!screenSurface)
    {
        fprintf(stderr, "Window surface could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -2;
    }


    int rectWidth = currentScreenWidth / 2, rectHeight = currentScreenHeight / 2;
    int posx = (currentScreenWidth - rectWidth) / 2, posy = (currentScreenHeight - rectHeight) / 2;
    SDL_Rect rect = {posx, posy, rectWidth, rectHeight};

    SDL_FillSurfaceRect(screenSurface, &rect, 0xfffff000);

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
                SDL_GetWindowSize(window, &currentScreenWidth, &currentScreenHeight);
                printf("Window resized HEIHT: %d, WIDTH: %d \n", currentScreenWidth, currentScreenHeight);
                break;
            case SDL_EVENT_DROP_FILE:
                droppedFileName = e.drop.data;
                printf("Dropped file: %s\n", droppedFileName);
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
