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
    explicit VulkanRenderer(SDL_Window* window);
    ~VulkanRenderer();

    void createInstance();
    VkInstance getInstance();

    void setupDebugUtils();
private:
    SDL_Window* window;
    VkInstance instance{};
    void cleanVulkan();

    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT{ nullptr };
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT{ nullptr };
    PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT{ nullptr };

    VkDebugUtilsMessengerEXT debugMessenger;

    bool debugUtilsSupported = false;
};



#endif //VULKANRENDERER_H
