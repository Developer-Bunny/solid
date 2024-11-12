//
// Created by Batur on 10/11/2024.
//

#include "VulkanRenderer.h"
#include <stdexcept>
#include <vector>

#include "DebugConfig.h"

VulkanRenderer::VulkanRenderer()
= default;

VkInstance VulkanRenderer::getInstance() const
{
    return instance;
}

VkPhysicalDevice VulkanRenderer::getPhysicalDevice() const
{
    return physicalDevice;
}

VkDevice VulkanRenderer::getDevice() const
{
    return device;
}

VkQueue VulkanRenderer::getQueue() const
{
    return queue;
}

const VkAllocationCallbacks* VulkanRenderer::getAllocator() const
{
    return allocator;
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

bool VulkanRenderer::IsExtensionAvailable(const std::vector<VkExtensionProperties>& properties, const char* extension)
{
    for (const VkExtensionProperties& p : properties)
        if (strcmp(p.extensionName, extension) == 0)
        {
            DebugConfig::verbose("[Vulkan] Found extension %s", extension);
            return true;
        }
    DebugConfig::warning("[Vulkan] Extension not found %s", extension);
    return false;
}


void VulkanRenderer::createInstance(const std::vector<const char*>& requestedInstanceExtensions)
{
    if (instance != VK_NULL_HANDLE)
        return;

    // Get available extensionsProperties
    uint32_t extensionPropertiesCount;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionPropertiesCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionPropertiesCount);
    VkResult err = vkEnumerateInstanceExtensionProperties(nullptr, &extensionPropertiesCount,
                                                          availableExtensions.data());
    if (err != VK_SUCCESS)
    {
        throw std::runtime_error("[Vulkan] Failed to enumerate extension properties!");
    }
    DebugConfig::verbose("[Vulkan] Found Vulkan extensions: %d ", extensionPropertiesCount);

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Solid VulkanApplication";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 2, 0);
    appInfo.pEngineName = "Solid VulkanEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 2, 0);
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

    std::vector<const char*> instanceExtensions;
    for (const char* requestedExtension : requestedInstanceExtensions)
        if (IsExtensionAvailable(availableExtensions, requestedExtension))
            instanceExtensions.push_back(requestedExtension);

    if (IsExtensionAvailable(availableExtensions, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
        instanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

    if (DebugConfig::isDebug())
    {
        if (IsExtensionAvailable(availableExtensions, VK_EXT_DEBUG_REPORT_EXTENSION_NAME))
        {
            instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
    }
    createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
    createInfo.ppEnabledExtensionNames = instanceExtensions.data();
    DebugConfig::verbose("[Vulkan] Instance extensions available:  %d", instanceExtensions.size());

    err = vkCreateInstance(&createInfo, allocator, &instance);
    if (err != VK_SUCCESS)
    {
        throw std::runtime_error("[Vulkan] Vulkan cannot be established!");
    }
    DebugConfig::verbose("[Vulkan] Vulkan instance created");

    if (DebugConfig::isDebug())
        setupDebugUtils();

    setupDevices();
}

void VulkanRenderer::setupDevices()
{
    physicalDevice = selectPhysicalDevice();
    DebugConfig::verbose("[Vulkan] Physical device found");

    uint32_t queueCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, nullptr);
    auto* queues = static_cast<VkQueueFamilyProperties*>(malloc(sizeof(VkQueueFamilyProperties) * queueCount));
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, queues);
    for (uint32_t i = 0; i < queueCount; i++)
    {
        if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            queueFamily = i;
        }
    }
    free(queues);
    if (queueFamily == -1)
    {
        throw std::runtime_error("[Vulkan] Failed to find any graphics queue families!");
    }
    DebugConfig::verbose("[Vulkan] QueueFamily found: %d", queueFamily);

    // Logical device with 1 queue
    // Get available extensions
    uint32_t devicePropertiesCount;
    std::vector<VkExtensionProperties> deviceSupportedExtensions;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &devicePropertiesCount, nullptr);
    deviceSupportedExtensions.resize(devicePropertiesCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &devicePropertiesCount,
                                         deviceSupportedExtensions.data());


    // In case of needed extension on physical device
    std::vector<const char*> deviceExtensions;

    constexpr float queuePriority[] = {0.1f};
    VkDeviceQueueCreateInfo queueInfo[1] = {};
    queueInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo[0].queueFamilyIndex = queueFamily;
    queueInfo[0].queueCount = 1;
    queueInfo[0].pQueuePriorities = queuePriority;

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = sizeof(queueInfo) / sizeof(queueInfo[0]);
    createInfo.pQueueCreateInfos = queueInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();
    VkResult err = vkCreateDevice(physicalDevice, &createInfo, allocator, &device);
    if (err != VK_SUCCESS)
    {
        throw std::runtime_error("[Vulkan] Failed to create logical device!");
    }
    vkGetDeviceQueue(device, queueFamily, 0, &queue);
    DebugConfig::verbose("[Vulkan] Logical device created");

    // Descriptor
    VkDescriptorPoolSize descriptorPoolSizes[] = {
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1}
    };
    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolInfo.maxSets = 1;
    poolInfo.poolSizeCount = static_cast<uint32_t>(sizeof(descriptorPoolSizes) / sizeof(descriptorPoolSizes[0]));
    poolInfo.pPoolSizes = descriptorPoolSizes;
    err = vkCreateDescriptorPool(device, &poolInfo, allocator, &descriptorPool);
    if (err != VK_SUCCESS)
    {
        throw std::runtime_error("[Vulkan] Failed to create descriptor pool!");
    }
    DebugConfig::verbose("[Vulkan] Descriptor pool created");
}

VkPhysicalDevice VulkanRenderer::selectPhysicalDevice() const
{
    uint32_t deviceCount = 0;
    VkResult err = vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (err != VK_SUCCESS || deviceCount <= 0)
    {
        throw std::runtime_error("[Vulkan] Failed to find GPUs with Vulkan support!");
    }
    std::vector<VkPhysicalDevice> devices(deviceCount);

    err = vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
    if (err != VK_SUCCESS || deviceCount <= 0)
    {
        throw std::runtime_error("[Vulkan] Failed to retrieve devices with vulkan support!");
    }

    // Selection of discrete gpu, multiple gpus not implemented
    for (const VkPhysicalDevice& device : devices)
    {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            return device;
    }

    return devices[0];
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

            if (vkCreateDebugUtilsMessengerEXT(instance, &createInfo, allocator, &debugMessenger) != VK_SUCCESS)
            {
                throw std::runtime_error("[Vulkan] Failed to set up debug messenger!");
            }
            DebugConfig::verbose("[Vulkan] Debug messenger created");
        }
        else
        {
            DebugConfig::warning("[Vulkan] DebugUtils not supported");
        }
    }
    else
    {
        DebugConfig::warning("[Vulkan] %s extension not present, debug utils are disabled.",
                             VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
}

void VulkanRenderer::cleanVulkan()
{
    if (descriptorPool != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorPool(device, descriptorPool, allocator);
        descriptorPool = VK_NULL_HANDLE;
        DebugConfig::verbose("[Vulkan] Destroying Vulkan descriptor pool");
    }
    if (debugUtilsSupported && debugMessenger != VK_NULL_HANDLE)
    {
        vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, allocator);
        debugMessenger = VK_NULL_HANDLE;
        DebugConfig::verbose("[Vulkan] Destroying Vulkan debugutils");
    }
    if (device != VK_NULL_HANDLE)
    {
        vkDestroyDevice(device, allocator);
        device = VK_NULL_HANDLE;
        DebugConfig::verbose("[Vulkan] Destroying Vulkan device");
    }
    if (instance != VK_NULL_HANDLE)
    {
        vkDestroyInstance(instance, allocator);
        instance = VK_NULL_HANDLE;
        DebugConfig::verbose("[Vulkan] Destroying Vulkan instance");
    }
}

VulkanRenderer::~VulkanRenderer()
{
    cleanVulkan();
}
