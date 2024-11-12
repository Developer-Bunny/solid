//
// Created by Batur on 10/11/2024.
//

#ifndef DEBUGCONFIG_H
#define DEBUGCONFIG_H
#include <ctime>
#include <cstdio>
#include <unordered_map>
#include <vulkan/vulkan_core.h>

class DebugConfig
{
public:
    static void setDebug(bool debug);
    static bool isDebug();

    template <typename... T>
    static void warning(const char* _format, T&&... args)
    {
        if (!DEBUG) return;

        const std::time_t t = std::time(nullptr);
        char timeStr[100];
        std::strftime(timeStr, sizeof(timeStr), "[%Y-%m-%d %H:%M:%S]", std::localtime(&t));

        std::printf("%s WARNING: ", timeStr);
        std::printf(_format, std::forward<T>(args)...);
        std::printf("\n");
    }

    // Vulkan Stuff
    static const std::unordered_map<VkDebugUtilsMessageSeverityFlagBitsEXT, const char*> messageSeverityFlags;
    static const std::unordered_map<VkDebugUtilsMessageTypeFlagsEXT, const char*> messageTypeFlags;

    static const char* getSeverityLabel(VkDebugUtilsMessageSeverityFlagBitsEXT severity);
    static const char* getTypeLabel(VkDebugUtilsMessageTypeFlagsEXT type);

    static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData
    );

private:
    static bool DEBUG;
};

#endif // DEBUGCONFIG_H
