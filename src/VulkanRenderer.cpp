//
// Created by Batur on 10/11/2024.
//

#include "VulkanRenderer.h"
#include <stdexcept>
#include <vector>

#include "DebugConfig.h"

VulkanRenderer::VulkanRenderer(SDL_Window* window)
    : window(window)
{
}

bool isValidationLayerSupported(const char* layerName)
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const auto& layerProperties : availableLayers)
    {
        if (strcmp(layerName, layerProperties.layerName) == 0)
        {
            return true;
        }
    }

    return false;
}

void VulkanRenderer::createInstance()
{
    if (instance != VK_NULL_HANDLE)
        return;

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Solid VulkanApplication";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Solid VulkanEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    const char* validationLayers[] = {"VK_LAYER_KHRONOS_validation"};
    if (DebugConfig::isDebug() && isValidationLayerSupported(validationLayers[0]))
    {
        createInfo.enabledLayerCount = 1;
        createInfo.ppEnabledLayerNames = validationLayers;
    }
    else
    {
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
    }

    std::vector<const char*> validationExtensions(1, VK_KHR_SURFACE_EXTENSION_NAME);
    if (DebugConfig::isDebug())
    {
        validationExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    createInfo.enabledExtensionCount = static_cast<uint32_t>(validationExtensions.size());
    createInfo.ppEnabledExtensionNames = validationExtensions.data();

    const VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Vulkan cannot be established! Vulkan Error");
    }
}

void VulkanRenderer::setupDebugUtils()
{
    bool extensionPresent = false;
    uint32_t extensionCount;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    for (const auto& extension : extensions)
    {
        if (strcmp(extension.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0)
        {
            extensionPresent = true;
            break;
        }
    }

    if (extensionPresent)
    {
        vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
        vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
        vkSetDebugUtilsObjectNameEXT = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(
            vkGetInstanceProcAddr(instance, "vkSetDebugUtilsObjectNameEXT"));

        debugUtilsSupported = (vkCreateDebugUtilsMessengerEXT != nullptr);

        if (debugUtilsSupported)
        {
            VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            createInfo.pfnUserCallback = DebugConfig::VulkanDebugCallback;
            createInfo.pUserData = nullptr;

            if (::vkCreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to set up debug messenger! Vulkan Error");
            }
        }
    }
    else
    {
        DebugConfig::warning("%s extension not present, debug utils are disabled. Vulkan Warning",
                             VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
}

VkInstance VulkanRenderer::getInstance()
{
    if (instance == VK_NULL_HANDLE)
    {
        createInstance();
    }
    return instance;
}

void VulkanRenderer::cleanVulkan()
{
    if (instance != VK_NULL_HANDLE)
    {
        vkDestroyInstance(instance, nullptr);
        instance = VK_NULL_HANDLE;
    }
}

VulkanRenderer::~VulkanRenderer()
{
    cleanVulkan();
}
