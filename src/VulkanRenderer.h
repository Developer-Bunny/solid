//
// Created by Batur on 10/11/2024.
//

#ifndef VULKANRENDERER_H
#define VULKANRENDERER_H

#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>

class VulkanRenderer
{
    public:
    VulkanRenderer(SDL_Window *window);
    ~VulkanRenderer();

    VkInstance createInstance();

};


#endif //VULKANRENDERER_H
