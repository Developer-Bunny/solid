//
// Created by Batur on 10/11/2024.
//

#ifndef VULKANRENDERER_H
#define VULKANRENDERER_H

#include <vector>
#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>

class VulkanRenderer
{
public:
    explicit VulkanRenderer();
    ~VulkanRenderer();

    void createInstance(const std::vector<const char*>& requestedInstanceExtensions);
    VkInstance getInstance() const;
    VkPhysicalDevice getPhysicalDevice() const;
    VkDevice getDevice() const;
    VkQueue getQueue() const;
    const VkAllocationCallbacks* getAllocator() const;

private:
    VkAllocationCallbacks* allocator = nullptr;
    VkInstance instance = VK_NULL_HANDLE;
    VkPipelineCache pipelineCache = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    uint32_t queueFamily = static_cast<uint32_t>(-1);
    VkQueue queue = VK_NULL_HANDLE;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;


    void setupDevices();
    VkPhysicalDevice selectPhysicalDevice() const;
    void setupDebugUtils();


    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT{ nullptr };
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT{ nullptr };
    PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT{ nullptr };

    VkDebugUtilsMessengerEXT debugMessenger{};

    bool debugUtilsSupported = false;

    static bool IsExtensionAvailable(const std::vector<VkExtensionProperties>& properties, const char* extension);
    void cleanVulkan();
};



#endif //VULKANRENDERER_H
