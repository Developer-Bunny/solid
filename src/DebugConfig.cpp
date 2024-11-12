//
// Created by Batur on 11/11/2024.
//
#include "DebugConfig.h"

#ifdef NDEBUG
bool DebugConfig::DEBUG = false;
#else
bool DebugConfig::DEBUG = true;
#endif

const std::unordered_map<VkDebugUtilsMessageSeverityFlagBitsEXT, const char*> DebugConfig::messageSeverityFlags = {
    {VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT, "Verbose"},
    {VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT, "Info"},
    {VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT, "Warning"},
    {VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, "Error"}
};

const std::unordered_map<VkDebugUtilsMessageTypeFlagsEXT, const char*> DebugConfig::messageTypeFlags = {
    {VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT, "General"},
    {VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT, "Validation"},
    {VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT, "Performance"}
};

void DebugConfig::setDebug(bool debug) {
    DEBUG = debug;
}

bool DebugConfig::isDebug() {
    return DEBUG;
}

const char* DebugConfig::getSeverityLabel(VkDebugUtilsMessageSeverityFlagBitsEXT severity) {
    const auto it = messageSeverityFlags.find(severity);
    return it != messageSeverityFlags.end() ? it->second : "Unknown";
}

const char* DebugConfig::getTypeLabel(VkDebugUtilsMessageTypeFlagsEXT type) {
    const auto it = messageTypeFlags.find(type);
    return it != messageTypeFlags.end() ? it->second : "Unknown";
}

VKAPI_ATTR VkBool32 VKAPI_CALL DebugConfig::VulkanDebugCallback(
    const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    const VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    std::printf("Vulkan_Renderer:%s:%s: %s\n", getSeverityLabel(messageSeverity), getTypeLabel(messageType), pCallbackData->pMessage);

    return VK_FALSE;
}


