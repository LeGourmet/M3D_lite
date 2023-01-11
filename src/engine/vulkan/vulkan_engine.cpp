#include "vulkan_engine.hpp"
#include "SDL_vulkan.h"
#include "application.hpp"
#include "model/scene.hpp"
#include "controller/freefly_camera.hpp"
#include "engine/vk_error_helper.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_vulkan.h"
#include "utils/constant.hpp"
#include <algorithm>
#include <chrono>
#include <set>
#include <stb/stb_image.h>

namespace TutoVulkan
{
namespace Engine
{
    void VulkanEngine::waitIdle()
    {
        vkDeviceWaitIdle(_logicalDevice);
    }

    //--------------------------------------------------------------------------------------------------------------
    // Cleanup
    //------------------------------------------------------------------------------------------------------------*/

    void VulkanEngine::cleanupScene()
    {
        waitIdle();
        _vertices = nullptr;
        _indices = nullptr;
        vkDestroyBuffer(_logicalDevice, _indexBuffer, nullptr);
        vkFreeMemory(_logicalDevice, _indexBufferMemory, nullptr);

        vkDestroyBuffer(_logicalDevice, _vertexBuffer, nullptr);
        vkFreeMemory(_logicalDevice, _vertexBufferMemory, nullptr);
        
        //App::getInstance().getScene().cleanTextures();
    }

    void VulkanEngine::cleanupTexture(const Texture *p_texture) const
    {
        vkDestroySampler(_logicalDevice, p_texture->sampler, nullptr);
        vkDestroyImageView(_logicalDevice, p_texture->view, nullptr);

        vkDestroyImage(_logicalDevice, p_texture->image, nullptr);
        vkFreeMemory(_logicalDevice, p_texture->deviceMemory, nullptr);
    }

    void VulkanEngine::cleanupSwapChain()
    {

        for (VkFramebuffer framebuffer : _imguiFramebuffers)
        {
            vkDestroyFramebuffer(_logicalDevice, framebuffer, nullptr);
        }

        vkDestroyRenderPass(_logicalDevice, _imguiRenderPass, nullptr);

        vkFreeCommandBuffers(_logicalDevice, _imguiCommandPool, static_cast<uint32_t>(_imguiCommandBuffers.size()), _imguiCommandBuffers.data());
        vkDestroyCommandPool(_logicalDevice, _imguiCommandPool, nullptr);

        vkDestroyImageView(_logicalDevice, _depthImageView, nullptr);
        vkDestroyImage(_logicalDevice, _depthImage, nullptr);
        vkFreeMemory(_logicalDevice, _depthImageMemory, nullptr);

        for (VkFramebuffer framebuffer : _swapChainFramebuffers)
        {
            vkDestroyFramebuffer(_logicalDevice, framebuffer, nullptr);
        }

        vkFreeCommandBuffers(_logicalDevice, _commandPool, static_cast<uint32_t>(_commandBuffers.size()), _commandBuffers.data());
        vkDestroyPipeline(_logicalDevice, _graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(_logicalDevice, _pipelineLayout, nullptr);
        vkDestroyRenderPass(_logicalDevice, _renderPass, nullptr);

        for (int i = 0; i < _swapChainImageViews.size(); i++)
        {
            vkDestroyImageView(_logicalDevice, _swapChainImageViews[i], nullptr);
        }

        vkDestroySwapchainKHR(_logicalDevice, _swapChain, nullptr);

        for (size_t i = 0; i < _swapChainImages.size(); i++)
        {
            vkDestroyBuffer(_logicalDevice, _uniformBuffers[i], nullptr);
            vkFreeMemory(_logicalDevice, _uniformBuffersMemory[i], nullptr);
        }

        vkDestroyDescriptorPool(_logicalDevice, _descriptorPool, nullptr);
    }

    void VulkanEngine::cleanup()
    {

        cleanupSwapChain();

        vkDestroyDescriptorPool(_logicalDevice, _imguiDescriptorPool, nullptr);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vkDestroySemaphore(_logicalDevice, _renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(_logicalDevice, _imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(_logicalDevice, _inFlightFences[i], nullptr);
        }

        vkDestroyCommandPool(_logicalDevice, _commandPool, nullptr);

        vkDestroyDescriptorSetLayout(_logicalDevice, _descriptorSetLayout, nullptr);

        cleanupScene();

        vkDestroyDevice(_logicalDevice, nullptr);
        vkDestroySurfaceKHR(_instance, _surface, nullptr);

        if (enableValidationLayers)
        {
            _destroyDebugUtilsMessengerEXTWrapper(_instance, _debugMessenger, nullptr);
        }

        vkDestroyInstance(_instance, nullptr);
    }

    //--------------------------------------------------------------------------------------------------------------
    // Init renderer
    //------------------------------------------------------------------------------------------------------------*/

    void VulkanEngine::initRenderer(SDL_Window *p_window)
    { // some of the methods should probably be private, wait until we know for sure
        _window = p_window;
        createInstance();
        setupDebugMessenger();
        createSurface(p_window);
        selectPhysicalDevice();
        createLogicalDevice();
        createSwapChain();
        createImageViews();
        createRenderPass();
        createDescriptorSetLayout();
        createPipeline();
        createCommandPool();
        createDepthResources();
        createFramebuffers();
        createUniformBuffers();
        createDescriptorPool();
        createDescriptorSets();
        createCommandBuffers();
        createSyncObjects();
        initImgui();
    }

    void VulkanEngine::createInstance()
    {
        if (enableValidationLayers && !_checkValidationLayerSupport())
        {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello World";              // Should take from define
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0); // should take from define
        appInfo.pEngineName = "VulkanEngine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0); // should take from define
        appInfo.apiVersion = VK_API_VERSION_1_2;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        std::vector<const char *> extensions = _getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            _populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo; // create a validation error, need to dig on that
        }
        else
        {
            createInfo.enabledLayerCount = 0;

            createInfo.pNext = nullptr;
        }

        VkResult result = vkCreateInstance(&createInfo, nullptr, &_instance);
        if (result != VK_SUCCESS) { throw std::runtime_error("Failed to create Vulkan instance. Got: " + VkErrorHelper::stringify(result)); }
        std::cout << "[DEBUG] "
                  << "Vulkan initialized successfully." << std::endl;
        _clearValues = {{{CLEAR_COLOR.x, CLEAR_COLOR.y, CLEAR_COLOR.z, 1.0f}, {1.f, 0.f}}};
    }

    bool VulkanEngine::_checkValidationLayerSupport()
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char *layerName : validationLayers)
        {
            bool layerFound = false;

            for (const VkLayerProperties &layerProperties : availableLayers)
            {
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
            {
                return false;
            }
        }

        return true;
    }

    void VulkanEngine::_populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &p_createInfo)
    {
        p_createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        // this is the list of flags we want our callback function to handle;
        p_createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        // this is the list of types we want our callback function to handle;
        p_createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        p_createInfo.pfnUserCallback = _debugCallback;
        p_createInfo.pNext = nullptr;
        p_createInfo.flags = 0;
        p_createInfo.pUserData = nullptr; // Optional
    }

    void VulkanEngine::setupDebugMessenger()
    {
        if (!enableValidationLayers) return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        _populateDebugMessengerCreateInfo(createInfo);

        VkResult result = _createDebugUtilsMessengerEXTWrapper(_instance, &createInfo, nullptr, &_debugMessenger);
        if (result != VK_SUCCESS) { throw std::runtime_error("Failed to set up debug messenger!. Got: " + VkErrorHelper::stringify(result)); }
        std::cout << "[DEBUG] "
                  << "Debug messenger successfully created." << std::endl;
    }

    void VulkanEngine::createSurface(SDL_Window *p_window)
    {
        if (!SDL_Vulkan_CreateSurface(_window, _instance, &_surface)) { throw std::runtime_error("Failed to create window surface."); }
        std::cout << "[DEBUG] "
                  << "Surface successfully created." << std::endl;
    }

    void VulkanEngine::selectPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);
        if (deviceCount == 0)
        {
            throw std::runtime_error("Failed to find GPUs with Vulkan support.");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());

        for (const VkPhysicalDevice &device : devices)
        {
            if (_isDeviceSuitable(device))
            {
                _physicalDevice = device;
                break;
            }
        }

        if (_physicalDevice == VK_NULL_HANDLE)
        {
            throw std::runtime_error("Failed to find a suitable GPU.");
        }
    }

    bool VulkanEngine::_isDeviceSuitable(VkPhysicalDevice p_device)
    {
        _queueIndices = _findQueueFamilies(p_device);
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(p_device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(p_device, &deviceFeatures);

        bool extensionsSupported = _checkDeviceExtensionSupport(p_device);

        bool swapChainAdequate = false;
        if (extensionsSupported)
        {
            _swapChainSupport = _querySwapChainSupport(p_device);
            swapChainAdequate = !_swapChainSupport.formats.empty() && !_swapChainSupport.presentModes.empty();
        }

        return _queueIndices.isComplete() && extensionsSupported && swapChainAdequate && deviceFeatures.geometryShader; // device feature shoud be an array somewhere
    }

    std::vector<const char *> VulkanEngine::_getRequiredExtensions()
    {
        unsigned int count;
        if (!SDL_Vulkan_GetInstanceExtensions(_window, &count, nullptr))
        {
            throw std::runtime_error("Failed to query extensions for SDL");
        }

        std::vector<const char *> extensions = {};

        extensions.resize(count);

        if (!SDL_Vulkan_GetInstanceExtensions(_window, &count, extensions.data()))
        {
            throw std::runtime_error("Failed to query extensions for SDL");
        }

        if (enableValidationLayers)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); // we dont need to check if this extension is supported because its support is implicit when we check for validation layer support
        }

        return extensions;
    }

    bool VulkanEngine::_checkDeviceExtensionSupport(VkPhysicalDevice p_device)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(p_device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(p_device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(EXTENSIONS.begin(), EXTENSIONS.end());
        for (const VkExtensionProperties &extension : availableExtensions)
        {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    QueueFamilyIndices VulkanEngine::_findQueueFamilies(VkPhysicalDevice p_device)
    {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(p_device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(p_device, &queueFamilyCount, queueFamilies.data());

        VkBool32 presentSupport = false;
        int i = 0;
        for (const VkQueueFamilyProperties &queueFamily : queueFamilies)
        {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) { indices.graphicsFamily = i; }
            vkGetPhysicalDeviceSurfaceSupportKHR(p_device, i, _surface, &presentSupport);
            if (presentSupport) { indices.presentFamily = i; }

            if (indices.isComplete()) { break; }
            i++;
        }

        return indices;
    }

    SwapChainSupportDetails VulkanEngine::_querySwapChainSupport(VkPhysicalDevice p_device)
    {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(p_device, _surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(p_device, _surface, &formatCount, nullptr);

        if (formatCount != 0)
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(p_device, _surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(p_device, _surface, &presentModeCount, nullptr);

        if (presentModeCount != 0)
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(p_device, _surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    VkSurfaceFormatKHR VulkanEngine::_chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &p_availableFormats)
    {
        for (const VkSurfaceFormatKHR &availableFormat : p_availableFormats)
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }
        return p_availableFormats[0];
    }

    VkPresentModeKHR VulkanEngine::_chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &p_availablePresentModes)
    {
        for (const VkPresentModeKHR &availablePresentMode : p_availablePresentModes)
        {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            { // this mode basically render a maximum of frames but present only the most recent based on vsync
                return availablePresentMode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR; // this mode renders at vsync
    }

    VkExtent2D VulkanEngine::_chooseSwapExtent(const VkSurfaceCapabilitiesKHR &p_capabilities)
    {
        if (p_capabilities.currentExtent.width != UINT32_MAX) { return p_capabilities.currentExtent; } // if not in special mode, return current Extent else
        else
        { // build correct extent
            int width, height;
            SDL_Vulkan_GetDrawableSize(_window, &width, &height); // get size from SDL window

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)};

            actualExtent.width = std::clamp(actualExtent.width, p_capabilities.minImageExtent.width, p_capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, p_capabilities.minImageExtent.height, p_capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    void VulkanEngine::createLogicalDevice()
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {_queueIndices.graphicsFamily.value(), _queueIndices.presentFamily.value()};
        float queuePriority = 1.0f;

        for (uint32_t queueFamily : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        VkPhysicalDeviceFeatures deviceFeatures{}; // empty because we dont currently need special feature support
        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(EXTENSIONS.size());
        createInfo.ppEnabledExtensionNames = EXTENSIONS.data();

        VkResult result = vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_logicalDevice);
        if (result != VK_SUCCESS) { throw std::runtime_error("Failed to create logical device. Got: " + VkErrorHelper::stringify(result)); }
        std::cout << "[DEBUG] "
                  << "LogicalDevice successfully created." << std::endl;

        vkGetDeviceQueue(_logicalDevice, _queueIndices.graphicsFamily.value(), 0, &_graphicsQueue);
        vkGetDeviceQueue(_logicalDevice, _queueIndices.presentFamily.value(), 0, &_presentQueue);
    }

    void VulkanEngine::createSwapChain()
    {

        _swapChainImageFormat = _chooseSwapSurfaceFormat(_swapChainSupport.formats);
        VkPresentModeKHR presentMode = _chooseSwapPresentMode(_swapChainSupport.presentModes);
        _swapChainExtent = _chooseSwapExtent(_swapChainSupport.capabilities);

        uint32_t imageCount = _swapChainSupport.capabilities.minImageCount + 1;
        if (_swapChainSupport.capabilities.maxImageCount > 0 && imageCount > _swapChainSupport.capabilities.maxImageCount)
        {
            imageCount = _swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = _surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = _swapChainImageFormat.format;
        createInfo.imageColorSpace = _swapChainImageFormat.colorSpace;
        createInfo.imageExtent = _swapChainExtent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // we render directly to swapchain

        uint32_t queueFamilyIndices[] = {_queueIndices.graphicsFamily.value(), _queueIndices.presentFamily.value()}; // _queueIndices is filled in _isDeviceSuitable

        if (_queueIndices.graphicsFamily != _queueIndices.presentFamily)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT; // set to VK_SHARING_MODE_CONCURRENT for now to save us some time on ownership management
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;     // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        createInfo.preTransform = _swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE; // needed for resizing

        VkResult result = vkCreateSwapchainKHR(_logicalDevice, &createInfo, nullptr, &_swapChain);
        if (result != VK_SUCCESS) { throw std::runtime_error("Failed to create SwapChain. Got: " + VkErrorHelper::stringify(result)); }
        std::cout << "[DEBUG] "
                  << "SwapChain successfully created." << std::endl;

        vkGetSwapchainImagesKHR(_logicalDevice, _swapChain, &imageCount, nullptr);
        _swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(_logicalDevice, _swapChain, &imageCount, _swapChainImages.data());
    }

    void VulkanEngine::_createImage(uint32_t p_width, uint32_t p_height, VkFormat p_format, VkImageTiling p_tiling, VkImageUsageFlags p_usage, VkMemoryPropertyFlags p_properties, VkImage &p_image, VkDeviceMemory &p_imageMemory)
    {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = p_width;
        imageInfo.extent.height = p_height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = p_format;
        imageInfo.tiling = p_tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = p_usage;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkResult result = vkCreateImage(_logicalDevice, &imageInfo, nullptr, &p_image);
        if (result != VK_SUCCESS) { throw std::runtime_error("Failed to create image. Got: " + VkErrorHelper::stringify(result)); }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(_logicalDevice, p_image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = _findMemoryType(memRequirements.memoryTypeBits, p_properties);

        result = vkAllocateMemory(_logicalDevice, &allocInfo, nullptr, &p_imageMemory);

        if (result != VK_SUCCESS) { throw std::runtime_error("Failed to allocate image memory. Got: " + VkErrorHelper::stringify(result)); }

        vkBindImageMemory(_logicalDevice, p_image, p_imageMemory, 0);
    }

    VkImageView VulkanEngine::_createImageView(VkImage p_image, VkFormat p_format, VkImageAspectFlags p_aspectFlags)
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = p_image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = p_format;
        viewInfo.subresourceRange.aspectMask = p_aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VkImageView imageView;

        VkResult result = vkCreateImageView(_logicalDevice, &viewInfo, nullptr, &imageView);
        if (result != VK_SUCCESS) { throw std::runtime_error("Failed to construct an ImageView. Got: " + VkErrorHelper::stringify(result)); }

        return imageView;
    }

    VkCommandBuffer VulkanEngine::_beginSingleTimeCommands(VkCommandPool &p_commandPool)
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = p_commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(_logicalDevice, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }

    void VulkanEngine::_endSingleTimeCommands(VkCommandPool &p_commandPool, VkCommandBuffer p_commandBuffer)
    {
        vkEndCommandBuffer(p_commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &p_commandBuffer;

        vkQueueSubmit(_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(_graphicsQueue);

        vkFreeCommandBuffers(_logicalDevice, p_commandPool, 1, &p_commandBuffer);
    }

    void VulkanEngine::_transitionImageLayout(VkImage p_image, VkFormat p_format, VkImageLayout p_oldLayout, VkImageLayout p_newLayout)
    {
        VkCommandBuffer commandBuffer = _beginSingleTimeCommands(_commandPool);

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = p_oldLayout;
        barrier.newLayout = p_newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = p_image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = 0;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        // We need to define each tranfer explicitly
        if (p_oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && p_newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (p_oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && p_newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else
        {
            throw std::invalid_argument("Unsupported layout transition.");
        }

        // see https://vulkan-tutorial.com/Texture_mapping/Images#page_Layout-transitions for more info
        vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        _endSingleTimeCommands(_commandPool, commandBuffer);
    }

    void VulkanEngine::_copyBufferToImage(VkBuffer p_srcBuffer, VkImage p_dstImage, uint32_t p_width, uint32_t p_height)
    {
        VkCommandBuffer commandBuffer = _beginSingleTimeCommands(_commandPool);

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = {
            p_width,
            p_height,
            1};

        vkCmdCopyBufferToImage(
            commandBuffer,
            p_srcBuffer,
            p_dstImage,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region);
        _endSingleTimeCommands(_commandPool, commandBuffer);
    }

    void VulkanEngine::createImageViews()
    {
        _swapChainImageViews.resize(_swapChainImages.size());

        for (uint32_t i = 0; i < _swapChainImages.size(); i++)
        {
            _swapChainImageViews[i] = _createImageView(_swapChainImages[i], _swapChainImageFormat.format, VK_IMAGE_ASPECT_COLOR_BIT);
        }
        std::cout << "[DEBUG] "
                  << "Finished creating all ImageView." << std::endl;
    }

    void VulkanEngine::createDescriptorSetLayout()
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT; // at which state in the pipeline the uniform buffer object is accessible
        uboLayoutBinding.pImmutableSamplers = nullptr;            // Optional, for image sampling

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        // here we describe the array of layout bindings
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &uboLayoutBinding;

        VkResult result = vkCreateDescriptorSetLayout(_logicalDevice, &layoutInfo, nullptr, &_descriptorSetLayout);

        if (result != VK_SUCCESS) { throw std::runtime_error("Failed to create descriptorSetLayout. Got: " + VkErrorHelper::stringify(result)); }
        std::cout << "[DEBUG] "
                  << "Created descriptorSetLayout successfully." << std::endl;
    }

    VkPipelineShaderStageCreateInfo VulkanEngine::_buildShader(const std::string &p_shaderPath, VkShaderModule &p_shaderModule, const VkShaderStageFlagBits p_shaderFlag)
    {
        std::vector<char> vertexShader = readPathBinary(getShadersPath(p_shaderPath));
        p_shaderModule = _createShaderModule(vertexShader);
        VkPipelineShaderStageCreateInfo shaderStageInfo{};
        shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageInfo.stage = p_shaderFlag;
        shaderStageInfo.module = p_shaderModule;
        shaderStageInfo.pName = "main";
        return shaderStageInfo;
    }

    void VulkanEngine::_createGenericPipeline(VkRenderPass p_renderPass,
                                              VkPipelineLayout p_pipelinelayout,
                                              VkPipeline *p_pipeline,
                                              int p_subpassIndex,
                                              const std::string &p_vertPath, const std::string &p_fragPath, const std::string &p_geomPath,
                                              std::vector<VkVertexInputBindingDescription> p_inputBindingDescriptions,
                                              std::vector<VkVertexInputAttributeDescription> p_inputAttributeDescriptions,
                                              VkPrimitiveTopology p_primitiveTopology,
                                              bool p_depthTestEnable, bool p_depthWriteEnable,
                                              VkCullModeFlagBits p_cullModeFlag, VkFrontFace p_frontFaceFlag,
                                              VkViewport p_viewport, VkRect2D p_scissor,
                                              VkPipelineColorBlendStateCreateInfo p_colorBlending)
    {
        bool hasGeometryShader = !p_geomPath.empty();

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &p_viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &p_scissor;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = p_cullModeFlag;
        rasterizer.frontFace = p_frontFaceFlag;
        rasterizer.depthBiasEnable = VK_FALSE;

        std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

        VkShaderModule vertShaderModule = nullptr;
        VkShaderModule geomShaderModule = nullptr;
        VkShaderModule fragShaderModule = nullptr;

        VkPipelineShaderStageCreateInfo vertShaderStageInfo = _buildShader(p_vertPath, vertShaderModule, VK_SHADER_STAGE_VERTEX_BIT);
        shaderStages.push_back(vertShaderStageInfo);

        if (hasGeometryShader)
        {
            VkPipelineShaderStageCreateInfo geomShaderStageInfo = _buildShader(p_geomPath, geomShaderModule, VK_SHADER_STAGE_GEOMETRY_BIT);
            shaderStages.push_back(geomShaderStageInfo);
        }

        VkPipelineShaderStageCreateInfo fragShaderStageInfo = _buildShader(p_fragPath, fragShaderModule, VK_SHADER_STAGE_FRAGMENT_BIT);
        shaderStages.push_back(fragShaderStageInfo);

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        if (p_inputAttributeDescriptions.empty() && p_inputBindingDescriptions.empty())
        {
            vertexInputInfo.vertexBindingDescriptionCount = 0;
            vertexInputInfo.vertexAttributeDescriptionCount = 0;
            vertexInputInfo.pVertexBindingDescriptions = nullptr;
            vertexInputInfo.pVertexAttributeDescriptions = nullptr;
        }
        else
        {
            vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(p_inputBindingDescriptions.size());
            vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(p_inputAttributeDescriptions.size());
            vertexInputInfo.pVertexBindingDescriptions = p_inputBindingDescriptions.data();
            vertexInputInfo.pVertexAttributeDescriptions = p_inputAttributeDescriptions.data();
        }

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = p_primitiveTopology;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = p_depthTestEnable;
        depthStencil.depthWriteEnable = p_depthWriteEnable;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.stencilTestEnable = VK_FALSE;

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
        pipelineInfo.pStages = shaderStages.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.pColorBlendState = &p_colorBlending;
        pipelineInfo.layout = p_pipelinelayout;
        pipelineInfo.renderPass = p_renderPass;
        pipelineInfo.subpass = p_subpassIndex;

        VkResult result = vkCreateGraphicsPipelines(_logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, p_pipeline);
        if (result != VK_SUCCESS) { throw std::runtime_error("Failed to create GraphicsPipelines. Got: " + VkErrorHelper::stringify(result)); }

        vkDestroyShaderModule(_logicalDevice, vertShaderModule, nullptr);
        if (hasGeometryShader) { vkDestroyShaderModule(_logicalDevice, geomShaderModule, nullptr); }
        vkDestroyShaderModule(_logicalDevice, fragShaderModule, nullptr);
    }

    void VulkanEngine::createPipeline()
    {
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)_swapChainExtent.width;
        viewport.height = (float)_swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = _swapChainExtent;

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &_descriptorSetLayout;

        VkResult result = vkCreatePipelineLayout(_logicalDevice, &pipelineLayoutInfo, nullptr, &_pipelineLayout);

        if (result != VK_SUCCESS) { throw std::runtime_error("Failed to create PipelineLayout. Got: " + VkErrorHelper::stringify(result)); }
        std::cout << "[DEBUG] "
                  << "Successfully constructed the PipelineLayout." << std::endl;

        _createGenericPipeline(_renderPass, _pipelineLayout, &_graphicsPipeline, 0,
                               "vert.spv", "frag.spv", "",
                               Vertex::getBindingDescription(), Vertex::getAttributeDescriptions(),
                               VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_TRUE, VK_TRUE,
                               VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE,
                               viewport, scissor, colorBlending);
    }

    VkShaderModule VulkanEngine::_createShaderModule(const std::vector<char> &p_code)
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = p_code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t *>(p_code.data());

        VkShaderModule shaderModule;
        VkResult result = vkCreateShaderModule(_logicalDevice, &createInfo, nullptr, &shaderModule);
        if (result != VK_SUCCESS) { throw std::runtime_error("Failed to create ShaderModule. Got: " + VkErrorHelper::stringify(result)); }
        std::cout << "[DEBUG] "
                  << "Successfully created ShaderModule." << std::endl;

        return shaderModule;
    }

    void VulkanEngine::_buildSubPassDescDepth(VkSubpassDescription &p_subpassDescriptions, VkAttachmentReference *p_depthReference)
    {
        p_subpassDescriptions.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        if (p_depthReference)
            p_subpassDescriptions.pDepthStencilAttachment = p_depthReference;
    }

    void VulkanEngine::_buildSubPassDescColor(VkSubpassDescription &subpassDescriptions, VkAttachmentReference *p_colorAttachments, uint32_t p_colorAttachmentCount)
    {
        subpassDescriptions.colorAttachmentCount = p_colorAttachmentCount;
        subpassDescriptions.pColorAttachments = p_colorAttachments;
    }

    void VulkanEngine::_buildAttachment(VkAttachmentDescription &p_attachments, VkFormat p_format, VkAttachmentStoreOp p_storeOp, VkAttachmentLoadOp p_loadOp, VkImageLayout p_imageLayout, VkImageLayout p_finalLayout){
        p_attachments.format = p_format;
        p_attachments.samples = VK_SAMPLE_COUNT_1_BIT;
        p_attachments.loadOp = p_loadOp;
        p_attachments.storeOp = p_storeOp;
        p_attachments.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        p_attachments.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        p_attachments.initialLayout = p_imageLayout;
        p_attachments.finalLayout = p_finalLayout;
    }

    void VulkanEngine::_buildSubPassDependency(VkSubpassDependency &dependencies, uint32_t p_src, uint32_t p_dst, uint32_t p_srcStage, uint32_t p_dstStage, uint32_t p_srcAcces, uint32_t p_dstAcces)
    {
        dependencies.srcSubpass = p_src;
        dependencies.dstSubpass = p_dst;
        dependencies.srcStageMask = p_srcStage;
        dependencies.dstStageMask = p_dstStage;
        dependencies.srcAccessMask = p_srcAcces;
        dependencies.dstAccessMask = p_dstAcces;
        dependencies.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
    }

    VkFormat VulkanEngine::_findDepthFormat()
    {
        for (VkFormat format : {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT})
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(_physicalDevice, format, &props);

            if ((VK_IMAGE_TILING_OPTIMAL == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) ||
                (VK_IMAGE_TILING_OPTIMAL == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT))
                return format;
        }

        throw std::runtime_error("failed to find supported format!");
    }

    void VulkanEngine::createRenderPass()
    {
        VkAttachmentDescription colorAttachment{};
        _buildAttachment(colorAttachment, _swapChainImageFormat.format, VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription depthAttachment{};
        _buildAttachment(depthAttachment, _findDepthFormat(), VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        _buildSubPassDescDepth(subpass, &depthAttachmentRef);
        _buildSubPassDescColor(subpass, &colorAttachmentRef, 1);

        VkSubpassDependency dependency{};
        _buildSubPassDependency(dependency, VK_SUBPASS_EXTERNAL, 0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, {}, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);

        std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        VkResult result = vkCreateRenderPass(_logicalDevice, &renderPassInfo, nullptr, &_renderPass);

        if (result != VK_SUCCESS) { throw std::runtime_error("Failed to create RenderPass. Got: " + VkErrorHelper::stringify(result)); }
        std::cout << "[DEBUG] "
                  << "Successfully contructed RenderPass." << std::endl;
    }

    void VulkanEngine::createFramebuffers()
    {
        _swapChainFramebuffers.resize(_swapChainImageViews.size());

        for (size_t i = 0; i < _swapChainImageViews.size(); i++)
        {
            std::array<VkImageView, 2> attachments = {
                _swapChainImageViews[i],
                _depthImageView};

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = _renderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = _swapChainExtent.width;
            framebufferInfo.height = _swapChainExtent.height;
            framebufferInfo.layers = 1;

            VkResult result = vkCreateFramebuffer(_logicalDevice, &framebufferInfo, nullptr, &_swapChainFramebuffers[i]);

            if (result != VK_SUCCESS) { throw std::runtime_error("Failed to create FrameBuffer. Got: " + VkErrorHelper::stringify(result)); }
            std::cout << "[DEBUG] "
                      << "Successfully created FrameBuffer" << std::endl;
        }
    }

    void VulkanEngine::createDepthResources()
    {
        VkFormat depthFormat = _findDepthFormat();
        _createImage(_swapChainExtent.width, _swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _depthImage, _depthImageMemory);
        _depthImageView = _createImageView(_depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
    }

    void VulkanEngine::createCommandPool()
    {
        QueueFamilyIndices queueFamilyIndices = _findQueueFamilies(_physicalDevice);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        VkResult result = vkCreateCommandPool(_logicalDevice, &poolInfo, nullptr, &_commandPool);

        if (result != VK_SUCCESS) { throw std::runtime_error("Failed to create CommandPool. Got: " + VkErrorHelper::stringify(result)); }
        std::cout << "[DEBUG] "
                  << "Successfully created CommandPool." << std::endl;
    }

    void VulkanEngine::_createBuffer(VkDeviceSize p_size, VkBufferUsageFlags p_usage, VkMemoryPropertyFlags p_properties, VkBuffer &p_buffer, VkDeviceMemory &p_bufferMemory)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = p_size;
        bufferInfo.usage = p_usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkResult result = vkCreateBuffer(_logicalDevice, &bufferInfo, nullptr, &p_buffer);

        if (result != VK_SUCCESS) { throw std::runtime_error("Failed to create Buffer. Got: " + VkErrorHelper::stringify(result)); }
        std::cout << "[DEBUG] "
                  << "Successfully created Buffer." << std::endl;

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(_logicalDevice, p_buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = _findMemoryType(memRequirements.memoryTypeBits, p_properties);

        result = vkAllocateMemory(_logicalDevice, &allocInfo, nullptr, &p_bufferMemory);

        if (result != VK_SUCCESS) { throw std::runtime_error("Failed to allocate buffer memory. Got: " + VkErrorHelper::stringify(result)); }
        std::cout << "[DEBUG] "
                  << "Successfully allocated memory for buffer." << std::endl;

        vkBindBufferMemory(_logicalDevice, p_buffer, p_bufferMemory, 0);
    }

    uint32_t VulkanEngine::_findMemoryType(uint32_t p_typeFilter, VkMemoryPropertyFlags p_properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &memProperties);
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if ((p_typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & p_properties) == p_properties)
            {
                return i;
            }
        }

        throw std::runtime_error("Failed to find suitable memory type.");
    }

    template <typename T>
    void VulkanEngine::_copyToDeviceMem(const VkDeviceSize &p_bufferSize, T *p_data, VkDeviceMemory p_BufferMemory)
    {
        void *data;
        vkMapMemory(_logicalDevice, p_BufferMemory, 0, p_bufferSize, 0, &data);
        memcpy(data, p_data, p_bufferSize);
        vkUnmapMemory(_logicalDevice, p_BufferMemory);
    }

    template <typename T>
    void VulkanEngine::_stageVectorToDeviceMem(VkBuffer &p_dstBuffer, VkDeviceMemory &p_dstMemory, VkDeviceSize &p_bufferSize, std::vector<T> *p_data, VkBufferUsageFlagBits p_usageFlags)
    {
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        _createBuffer(p_bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        _copyToDeviceMem<T>(p_bufferSize, p_data->data(), stagingBufferMemory);

        _createBuffer(p_bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | p_usageFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, p_dstBuffer, p_dstMemory);

        _copyBuffer(stagingBuffer, p_dstBuffer, p_bufferSize);

        vkDestroyBuffer(_logicalDevice, stagingBuffer, nullptr);
        vkFreeMemory(_logicalDevice, stagingBufferMemory, nullptr);
    }

    void VulkanEngine::createVertexBuffer()
    {
        VkDeviceSize bufferSize = sizeof((*_vertices)[0]) * (*_vertices).size();
        _stageVectorToDeviceMem(_vertexBuffer, _vertexBufferMemory, bufferSize, _vertices, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    }

    void VulkanEngine::createIndexBuffer()
    {
        VkDeviceSize bufferSize = sizeof((*_indices)[0]) * (*_indices).size();
        _stageVectorToDeviceMem(_indexBuffer, _indexBufferMemory, bufferSize, _indices, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    }

    void VulkanEngine::createUniformBuffers()
    {
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);

        _uniformBuffers.resize(_swapChainImages.size());
        _uniformBuffersMemory.resize(_swapChainImages.size());

        for (size_t i = 0; i < _swapChainImages.size(); i++)
        {
            _createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _uniformBuffers[i], _uniformBuffersMemory[i]);
        }
    }

    void VulkanEngine::_copyBuffer(VkBuffer p_srcBuffer, VkBuffer p_dstBuffer, VkDeviceSize p_size)
    { // We could create a command pool dedicated to memory transfert, note for later
        VkCommandBuffer commandBuffer = _beginSingleTimeCommands(_commandPool);

        VkBufferCopy copyRegion{};
        copyRegion.size = p_size;
        vkCmdCopyBuffer(commandBuffer, p_srcBuffer, p_dstBuffer, 1, &copyRegion);

        _endSingleTimeCommands(_commandPool, commandBuffer);
    }

    void VulkanEngine::createDescriptorPool()
    {

        VkDescriptorPoolSize poolSize{};
        poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize.descriptorCount = static_cast<uint32_t>(_swapChainImages.size());

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.maxSets = static_cast<uint32_t>(_swapChainImages.size());

        VkResult result = vkCreateDescriptorPool(_logicalDevice, &poolInfo, nullptr, &_descriptorPool);

        if (result != VK_SUCCESS) { throw std::runtime_error("Failed to create DescriptorPool. Got: " + VkErrorHelper::stringify(result)); }
        std::cout << "[DEBUG] "
                  << "DescriptorPool successfully created" << std::endl;
    }

    void VulkanEngine::createDescriptorSets()
    {
        std::vector<VkDescriptorSetLayout> layouts(_swapChainImages.size(), _descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = _descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(_swapChainImages.size());
        allocInfo.pSetLayouts = layouts.data();

        _descriptorSets.resize(_swapChainImages.size());

        VkResult result = vkAllocateDescriptorSets(_logicalDevice, &allocInfo, _descriptorSets.data());
        if (result != VK_SUCCESS) { throw std::runtime_error("Failed to allocate descriptorSets. Got: " + VkErrorHelper::stringify(result)); }
        std::cout << "[DEBUG] "
                  << "Successfully allocated descriptorSets" << std::endl;

        for (size_t i = 0; i < _swapChainImages.size(); i++)
        {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = _uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = _descriptorSets[i];
            descriptorWrite.dstBinding = 0; // this depend of the binding chosen for the uniform buffer
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &bufferInfo;
            descriptorWrite.pImageInfo = nullptr;       // Optional
            descriptorWrite.pTexelBufferView = nullptr; // Optional

            vkUpdateDescriptorSets(_logicalDevice, 1, &descriptorWrite, 0, nullptr);
        }
    }

    void VulkanEngine::createCommandBuffers()
    {
        _commandBuffers.resize(_swapChainFramebuffers.size());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = _commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)_commandBuffers.size();

        VkResult result = vkAllocateCommandBuffers(_logicalDevice, &allocInfo, _commandBuffers.data());

        if (result != VK_SUCCESS) { throw std::runtime_error("Failed to allocate CommandBuffers. Got: " + VkErrorHelper::stringify(result)); }
        std::cout << "[DEBUG] "
                  << "Successfully allocated CommandBuffers." << std::endl;
    }

    void VulkanEngine::createSyncObjects()
    {
        _imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        _renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        _inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
        _imagesInFlight.resize(_swapChainImages.size(), VK_NULL_HANDLE);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {

            VkResult result1 = vkCreateSemaphore(_logicalDevice, &semaphoreInfo, nullptr, &_imageAvailableSemaphores[i]);
            VkResult result2 = vkCreateSemaphore(_logicalDevice, &semaphoreInfo, nullptr, &_renderFinishedSemaphores[i]);
            VkResult result3 = vkCreateFence(_logicalDevice, &fenceInfo, nullptr, &_inFlightFences[i]);

            if (result1 != VK_SUCCESS || result2 != VK_SUCCESS) { throw std::runtime_error("Failed to create synchronization objects for a frame. Got: " + VkErrorHelper::stringify(result1) + ", " + VkErrorHelper::stringify(result2) + " and " + VkErrorHelper::stringify(result3)); }
        }
        std::cout << "[DEBUG] "
                  << "Successfully created synchronization objects for a frame." << std::endl;
    }

    //--------------------------------------------------------------------------------------------------------------
    // Draw frame
    //------------------------------------------------------------------------------------------------------------*/

    void VulkanEngine::_updateUniformBuffer(uint32_t p_currentImage, const float p_deltaTime)
    {
        // for more perf => mvp, mv and normal
        UniformBufferObject ubo{};
        ubo.model = MAT4F_ID;
        ubo.view = App::getInstance().getCamera().getViewMatrix();
        ubo.proj = App::getInstance().getCamera().getProjectionMatrix();
        ubo.proj[1][1] *= -1; // this is nessesaty to make glm work with Vulkan
        ubo.normal = glm::transpose(glm::inverse(App::getInstance().getCamera().getViewMatrix() * ubo.model));

        _copyToDeviceMem(sizeof(ubo), &ubo, _uniformBuffersMemory[p_currentImage]);
    }

    void VulkanEngine::recreateSwapChain()
    {
        while (SDL_GetWindowFlags(_window) & SDL_WINDOW_MINIMIZED)
            SDL_WaitEvent(nullptr);

        /*Not the best way, we need to wait for all frames in flight to finish rendering before we recreate the swapChain,
         * by using the oldSwapChain field when building the swapChain, we should be able to rebuild without waiting
         * for the still rendering frame. Will need to research that. */
        waitIdle();

        cleanupSwapChain();

        /*This is here because we weirdly needs to update the _swapChainSupport.capabilities struct for the resizing to work proprely,
         * this problem appeared in commit ab151f38217443ac927face6a838baa2d4d9f5da when we added ImGui, so we know it is not technicaly necessary.
         * We don't know the performance penality of querrying the driver each resize so this will need to be checked again later
         * but for now this solution work */
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physicalDevice, _surface, &_swapChainSupport.capabilities);

        createSwapChain();
        createImageViews();
        createRenderPass();
        createPipeline();
        createDepthResources();
        createFramebuffers();
        createUniformBuffers();
        createDescriptorPool();
        createDescriptorSets();
        createCommandBuffers();

        _imguiCreateRenderPass();
        _imguiCreateCommandPool();
        _imguiCommandBuffers.resize(_swapChainImageViews.size());
        _imguiCreateCommandBuffers();
        _imguiCreateFrameBuffers();

        _imagesInFlight.resize(_swapChainImages.size(), VK_NULL_HANDLE);
    }

    void VulkanEngine::drawFrame(const float p_deltaTime)
    {
        vkWaitForFences(_logicalDevice, 1, &_inFlightFences[_currentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(_logicalDevice, _swapChain, UINT64_MAX, _imageAvailableSemaphores[_currentFrame], VK_NULL_HANDLE, &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        { // here we recreate only if the swapchain is out of date, we could also if suboptimal, but not for now
            recreateSwapChain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("Failed to acquire swap chain image. Got: " + VkErrorHelper::stringify(result));
        }

        if (_imagesInFlight[imageIndex] != VK_NULL_HANDLE)
        {
            vkWaitForFences(_logicalDevice, 1, &_imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
        }

        _imagesInFlight[imageIndex] = _inFlightFences[_currentFrame];

        _updateUniformBuffer(imageIndex, p_deltaTime);

        vkResetFences(_logicalDevice, 1, &_inFlightFences[_currentFrame]);
        result = vkResetCommandBuffer(_commandBuffers[imageIndex], 0);
        if (result != VK_SUCCESS) { throw std::runtime_error("Failed to reset CommandBuffer. Got: " + VkErrorHelper::stringify(result)); }

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        result = vkBeginCommandBuffer(_commandBuffers[imageIndex], &beginInfo);

        if (result != VK_SUCCESS) { throw std::runtime_error("Failed to begin recording CommandBuffer. Got: " + VkErrorHelper::stringify(result)); }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = _renderPass;
        renderPassInfo.framebuffer = _swapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = _swapChainExtent;
        renderPassInfo.clearValueCount = static_cast<uint32_t>(_clearValues.size());
        renderPassInfo.pClearValues = _clearValues.data();

        vkCmdBeginRenderPass(_commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(_commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline);

        if (_vertices != nullptr)
        {
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(_commandBuffers[imageIndex], 0, 1, &_vertexBuffer, offsets);

            vkCmdBindIndexBuffer(_commandBuffers[imageIndex], _indexBuffer, 0, VK_INDEX_TYPE_UINT32);

            vkCmdBindDescriptorSets(_commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, 0, 1, &_descriptorSets[imageIndex], 0, nullptr);

            vkCmdDrawIndexed(_commandBuffers[imageIndex], static_cast<uint32_t>((*_indices).size()), 1, 0, 0, 0);
        }
        vkCmdEndRenderPass(_commandBuffers[imageIndex]);

        result = vkEndCommandBuffer(_commandBuffers[imageIndex]);

        if (result != VK_SUCCESS) { throw std::runtime_error("Failed to record CommandBuffer. Got: " + VkErrorHelper::stringify(result)); }

        VkCommandBufferBeginInfo ImguicommandBufferInfo = {};
        ImguicommandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        ImguicommandBufferInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(_imguiCommandBuffers[imageIndex], &ImguicommandBufferInfo);

        VkRenderPassBeginInfo ImguirenderPassInfo = {};
        ImguirenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        ImguirenderPassInfo.renderPass = _imguiRenderPass;
        ImguirenderPassInfo.framebuffer = _imguiFramebuffers[imageIndex];
        ImguirenderPassInfo.renderArea.extent.width = _swapChainExtent.width;
        ImguirenderPassInfo.renderArea.extent.height = _swapChainExtent.height;
        ImguirenderPassInfo.clearValueCount = static_cast<uint32_t>(_clearValues.size());
        ImguirenderPassInfo.pClearValues = _clearValues.data();
        vkCmdBeginRenderPass(_imguiCommandBuffers[imageIndex], &ImguirenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), _imguiCommandBuffers[imageIndex]);

        vkCmdEndRenderPass(_imguiCommandBuffers[imageIndex]);
        vkEndCommandBuffer(_imguiCommandBuffers[imageIndex]);

        std::array<VkCommandBuffer, 2> submitCommandBuffers =
            {_commandBuffers[imageIndex], _imguiCommandBuffers[imageIndex]};

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &_imageAvailableSemaphores[_currentFrame];
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = static_cast<uint32_t>(submitCommandBuffers.size());
        submitInfo.pCommandBuffers = submitCommandBuffers.data();
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &_renderFinishedSemaphores[_currentFrame];

        result = vkQueueSubmit(_graphicsQueue, 1, &submitInfo, _inFlightFences[_currentFrame]);

        if (result != VK_SUCCESS) { throw std::runtime_error("Failed to submit draw CommandBuffer. Got: " + VkErrorHelper::stringify(result)); }

        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &_renderFinishedSemaphores[_currentFrame];

        VkSwapchainKHR swapChains[] = {_swapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(_presentQueue, &presentInfo);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        { // here we allways recreate, we want the best
            std::cout << "[DEBUG] "
                      << "Recreating swapChain. Got: " + VkErrorHelper::stringify(result) << std::endl;
            recreateSwapChain();
        }
        else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to present swap chain image. Got: " + VkErrorHelper::stringify(result));
        }

        _currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    //--------------------------------------------------------------------------------------------------------------
    // ImGui Stuff
    //------------------------------------------------------------------------------------------------------------*/

    void VulkanEngine::initImgui()
    {

        _imguiCreateDescriptorPool();
        _imguiCreateRenderPass();

        // Setup Platform/Renderer bindings
        ImGui_ImplSDL2_InitForVulkan(_window);
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = _instance;
        init_info.PhysicalDevice = _physicalDevice;
        init_info.Device = _logicalDevice;
        init_info.QueueFamily = _queueIndices.graphicsFamily.value();
        init_info.Queue = _graphicsQueue;
        init_info.PipelineCache = VK_NULL_HANDLE;
        init_info.DescriptorPool = _imguiDescriptorPool;
        init_info.Allocator = nullptr;
        init_info.MinImageCount = _swapChainImageViews.size();
        init_info.ImageCount = _swapChainImageViews.size();
        init_info.CheckVkResultFn = nullptr;
        ImGui_ImplVulkan_Init(&init_info, _imguiRenderPass);

        _imguiCreateCommandPool();
        _imguiCommandBuffers.resize(_swapChainImageViews.size());
        _imguiCreateCommandBuffers();
        _imguiCreateFrameBuffers();

        VkCommandBuffer command_buffer = _beginSingleTimeCommands(_imguiCommandPool);
        ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
        _endSingleTimeCommands(_imguiCommandPool, command_buffer);
        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }

    void VulkanEngine::_imguiCreateDescriptorPool()
    {
        VkDescriptorPoolSize pool_sizes[] =
            {
                {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};
        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
        pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;
        VkResult result = vkCreateDescriptorPool(_logicalDevice, &pool_info, nullptr, &_imguiDescriptorPool);
        if (result != VK_SUCCESS) { throw std::runtime_error("Failed to create ImgGui DescriptorPool. Got: " + VkErrorHelper::stringify(result)); }
        std::cout << "[DEBUG] "
                  << "ImGui DescriptorPool successfully created" << std::endl;
    }

    void VulkanEngine::_imguiCreateRenderPass()
    {
        VkAttachmentDescription colorAttachment = {};
        _buildAttachment(colorAttachment, _swapChainImageFormat.format,
                         VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_LOAD, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        VkAttachmentReference colorAttachmentRef = {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

        VkSubpassDescription subpass{};
        _buildSubPassDescDepth(subpass, {});
        _buildSubPassDescColor(subpass, &colorAttachmentRef, 1);

        VkSubpassDependency dependency = {};
        _buildSubPassDependency(dependency, VK_SUBPASS_EXTERNAL, 0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);


        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        VkResult result = vkCreateRenderPass(_logicalDevice, &renderPassInfo, nullptr, &_imguiRenderPass);

        if (result != VK_SUCCESS) { throw std::runtime_error("Failed to create ImGui RenderPass. Got: " + VkErrorHelper::stringify(result)); }
        std::cout << "[DEBUG] "
                  << "Successfully contructed ImGui RenderPass." << std::endl;
    }

    void VulkanEngine::_imguiCreateCommandPool()
    {
        VkCommandPoolCreateInfo commandPoolCreateInfo = {};
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.queueFamilyIndex = _queueIndices.graphicsFamily.value();
        commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        VkResult result = vkCreateCommandPool(_logicalDevice, &commandPoolCreateInfo, nullptr, &_imguiCommandPool);

        if (result != VK_SUCCESS) { throw std::runtime_error("Could not create ImGui command pool. Got: " + VkErrorHelper::stringify(result)); }
    }

    void VulkanEngine::_imguiCreateCommandBuffers()
    {
        VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
        commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandPool = _imguiCommandPool;
        commandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(_imguiCommandBuffers.size());
        vkAllocateCommandBuffers(_logicalDevice, &commandBufferAllocateInfo, _imguiCommandBuffers.data());
    }

    void VulkanEngine::_imguiCreateFrameBuffers()
    {
        _imguiFramebuffers.resize(_swapChainImageViews.size());

        for (size_t i = 0; i < _swapChainImageViews.size(); i++)
        {
            VkImageView attachments[] = {
                _swapChainImageViews[i]};

            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = _imguiRenderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = _swapChainExtent.width;
            framebufferInfo.height = _swapChainExtent.height;
            framebufferInfo.layers = 1;

            VkResult result = vkCreateFramebuffer(_logicalDevice, &framebufferInfo, nullptr, &_imguiFramebuffers[i]);

            if (result != VK_SUCCESS) { throw std::runtime_error("Failed to create Imgui FrameBuffer. Got: " + VkErrorHelper::stringify(result)); }
            std::cout << "[DEBUG] "
                      << "Successfully created Imgui FrameBuffer" << std::endl;
        }
    }

    //--------------------------------------------------------------------------------------------------------------
    // Utility
    //------------------------------------------------------------------------------------------------------------*/
    
    void VulkanEngine::setVertexData(std::vector<Vertex> *p_vertices)
    {
        _vertices = p_vertices;
        createVertexBuffer();
    }

    void VulkanEngine::setIndexData(std::vector<uint32_t> *p_indices)
    {
        _indices = p_indices;
        createIndexBuffer();
    }

    void VulkanEngine::setClearColor(Vec3f p_color)
    {
        _clearValues = {{{p_color.x, p_color.y, p_color.z, 1.0f}, {1.f, 0.f}}};
    }

    Texture VulkanEngine::createTexture(const std::string &p_path)
    {
        int texWidth, texHeight, texChannels;
        stbi_uc *pixels;
        pixels = stbi_load(p_path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        if (!pixels)
        {
            std::cout << stbi_failure_reason() << std::endl;
            throw std::runtime_error("failed to load texture image!");
        }
        VkDeviceSize imageSize = texWidth * texHeight * 4;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        _createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void *data;
        vkMapMemory(_logicalDevice, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, (size_t)imageSize);
        vkUnmapMemory(_logicalDevice, stagingBufferMemory);
        stbi_image_free(pixels);

        Texture texture{};

        _createImage(static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, texture.image, texture.deviceMemory);

        // transition for efficient copy
        _transitionImageLayout(texture.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        _copyBufferToImage(stagingBuffer, texture.image, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

        vkDestroyBuffer(_logicalDevice, stagingBuffer, nullptr);
        vkFreeMemory(_logicalDevice, stagingBufferMemory, nullptr);

        // transition for shader reading
        _transitionImageLayout(texture.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        texture.view = _createImageView(texture.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_FALSE; // We need to query for support so disabled for now
        samplerInfo.maxAnisotropy = 1.0f;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;

        VkResult result = vkCreateSampler(_logicalDevice, &samplerInfo, nullptr, &texture.sampler);
        if (result != VK_SUCCESS) { throw std::runtime_error("Failed to create FrameBuffer. Got: " + VkErrorHelper::stringify(result)); }
        std::cout << "[DEBUG] "
                  << "Successfully created FrameBuffer" << std::endl;

        return texture;
    }

    //--------------------------------------------------------------------------------------------------------------
    // Debug stuff
    //------------------------------------------------------------------------------------------------------------*/
    // This is a wrapper around a vk extention function, we need to get said function's address to execute it, which is the reason why whe built said wrapper, could probably be moved in the hpp
    VkResult VulkanEngine::_createDebugUtilsMessengerEXTWrapper(VkInstance p_instance, const VkDebugUtilsMessengerCreateInfoEXT *p_CreateInfo, const VkAllocationCallbacks *p_Allocator, VkDebugUtilsMessengerEXT *p_DebugMessenger)
    {
        PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(p_instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr){
            return func(p_instance, p_CreateInfo, p_Allocator, p_DebugMessenger);
        }
            return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    // This is a wrapper around a vk extention function, we need to get said function's address to execute it, which is the reason why whe built said wrapper, could probably be moved in the hpp
    VkResult VulkanEngine::_destroyDebugUtilsMessengerEXTWrapper(VkInstance p_instance, VkDebugUtilsMessengerEXT p_debugMessenger, const VkAllocationCallbacks *p_Allocator)
    {
        PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(p_instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            func(p_instance, p_debugMessenger, p_Allocator);
            return VK_SUCCESS;
        }
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }


} // namespace Engine
} // namespace TutoVulkan