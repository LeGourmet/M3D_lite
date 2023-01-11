#ifndef __VULKAN_ENGINE__
#define __VULKAN_ENGINE__

#include "SDL.h"
#include "utils/define.hpp"
#include "vulkan/vulkan.h"
#include <array>
#include <iostream>
#include <optional>
#include <vector>

namespace TutoVulkan
{
namespace Engine
{
    struct Texture
    {
        VkSampler sampler;
        VkImage image;
        VkDeviceMemory deviceMemory;
        VkImageView view;
        uint32_t width, height;
        uint32_t mipLevels;
    };

    struct Vertex
    {
        Vec3f position;
        Vec2f uv;
        Vec3f normal;
        Vec3f tangent;
        Vec3f bitangent;
        Vec3f ka;
        Vec3f kd;
        Vec3f ks;
        float s;

        static std::vector<VkVertexInputBindingDescription> getBindingDescription()
        {
            std::vector<VkVertexInputBindingDescription> bindingDescription;
            bindingDescription.resize(1);
            bindingDescription[0].binding = 0;
            bindingDescription[0].stride = sizeof(Vertex);
            bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions()
        {
            std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
            attributeDescriptions.resize(9);
            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, position);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, uv);

            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[2].offset = offsetof(Vertex, normal);

            attributeDescriptions[3].binding = 0;
            attributeDescriptions[3].location = 3;
            attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[3].offset = offsetof(Vertex, tangent);

            attributeDescriptions[4].binding = 0;
            attributeDescriptions[4].location = 4;
            attributeDescriptions[4].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[4].offset = offsetof(Vertex, bitangent);

            attributeDescriptions[5].binding = 0;
            attributeDescriptions[5].location = 5;
            attributeDescriptions[5].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[5].offset = offsetof(Vertex, ka);

            attributeDescriptions[6].binding = 0;
            attributeDescriptions[6].location = 6;
            attributeDescriptions[6].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[6].offset = offsetof(Vertex, kd);

            attributeDescriptions[7].binding = 0;
            attributeDescriptions[7].location = 7;
            attributeDescriptions[7].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[7].offset = offsetof(Vertex, ks);

            attributeDescriptions[8].binding = 0;
            attributeDescriptions[8].location = 8;
            attributeDescriptions[8].format = VK_FORMAT_R32_SFLOAT;
            attributeDescriptions[8].offset = offsetof(Vertex, s);

            return attributeDescriptions;
        }
    };

    // BEWARE OF BIT ALIGNEMENT
    struct UniformBufferObject
    {
        Mat4f model;
        Mat4f view;
        Mat4f proj;
        Mat4f normal;
    };

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() { return (graphicsFamily.has_value() && presentFamily.has_value()); }
    };

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    class VulkanEngine
    {

    public:
        VulkanEngine(){};
        ~VulkanEngine(){};

        void cleanup();
        void cleanupTexture(const Texture *p_texture) const;
        void cleanupScene();
        void cleanupSwapChain();

        void initRenderer(SDL_Window *p_window);
        void initImgui();

        void createInstance();
        void setupDebugMessenger();
        void createSurface(SDL_Window *p_window);
        void selectPhysicalDevice();
        void createLogicalDevice();
        void createSwapChain();
        void createImageViews();
        void createDescriptorSetLayout();
        void createPipeline();
        void createRenderPass();
        void createFramebuffers();
        void createCommandPool();
        void createDepthResources();
        void createVertexBuffer();
        void createIndexBuffer();
        void createUniformBuffers();
        void createDescriptorPool();
        void createDescriptorSets();
        void createCommandBuffers();
        void createSyncObjects();
        void drawFrame(const float p_deltaTime);

        void waitIdle();
        void recreateSwapChain();
        Texture createTexture(const std::string &p_path);

        void setClearColor(Vec3f p_color);
        void setVertexData(std::vector<Vertex> *p_vertices);
        void setIndexData(std::vector<uint32_t> *p_indices);

    private:
        SDL_Window *_window;

        VkInstance _instance;
        VkDebugUtilsMessengerEXT _debugMessenger;
        VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
        VkDevice _logicalDevice;

        QueueFamilyIndices _queueIndices;
        VkQueue _graphicsQueue;
        VkQueue _presentQueue;

        VkSurfaceKHR _surface;

        SwapChainSupportDetails _swapChainSupport;
        VkSwapchainKHR _swapChain;
        VkSurfaceFormatKHR _swapChainImageFormat;
        VkExtent2D _swapChainExtent;
        std::vector<VkImage> _swapChainImages;
        std::vector<VkImageView> _swapChainImageViews;
        std::vector<VkFramebuffer> _swapChainFramebuffers;

        VkPipelineLayout _pipelineLayout;
        VkRenderPass _renderPass;
        VkPipeline _graphicsPipeline;
        VkDescriptorSetLayout _descriptorSetLayout;
        VkDescriptorPool _descriptorPool;
        std::vector<VkDescriptorSet> _descriptorSets;

        VkCommandPool _commandPool;
        std::vector<VkCommandBuffer> _commandBuffers;

        std::vector<VkSemaphore> _imageAvailableSemaphores;
        std::vector<VkSemaphore> _renderFinishedSemaphores;
        std::vector<VkFence> _inFlightFences;
        std::vector<VkFence> _imagesInFlight;
        size_t _currentFrame = 0;

        VkBuffer _vertexBuffer;
        VkDeviceMemory _vertexBufferMemory;
        VkBuffer _indexBuffer;
        VkDeviceMemory _indexBufferMemory;

        VkImage _depthImage;
        VkDeviceMemory _depthImageMemory;
        VkImageView _depthImageView;

        std::vector<VkBuffer> _uniformBuffers;
        std::vector<VkDeviceMemory> _uniformBuffersMemory;

        std::vector<Vertex> *_vertices = nullptr;
        std::vector<uint32_t> *_indices = nullptr;
        std::array<VkClearValue, 2> _clearValues;

        bool _checkValidationLayerSupport();
        std::vector<const char *> _getRequiredExtensions();
        void _populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &p_createInfo);
        bool _isDeviceSuitable(VkPhysicalDevice p_device);
        bool _checkDeviceExtensionSupport(VkPhysicalDevice p_device);
        QueueFamilyIndices _findQueueFamilies(VkPhysicalDevice p_device);
        SwapChainSupportDetails _querySwapChainSupport(VkPhysicalDevice p_device);
        VkSurfaceFormatKHR _chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &p_availableFormats);
        VkPresentModeKHR _chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &p_availablePresentModes);
        VkExtent2D _chooseSwapExtent(const VkSurfaceCapabilitiesKHR &p_capabilities);
        VkShaderModule _createShaderModule(const std::vector<char> &p_code);
        uint32_t _findMemoryType(uint32_t p_typeFilter, VkMemoryPropertyFlags p_properties);
        void _createBuffer(VkDeviceSize p_size, VkBufferUsageFlags p_usage, VkMemoryPropertyFlags p_properties, VkBuffer &p_buffer, VkDeviceMemory &p_bufferMemory);
        void _copyBuffer(VkBuffer p_srcBuffer, VkBuffer p_dstBuffer, VkDeviceSize p_size);
        void _updateUniformBuffer(uint32_t p_currentImage, const float p_deltaTime);
        void _createImage(uint32_t p_width, uint32_t p_height, VkFormat p_format, VkImageTiling p_tiling, VkImageUsageFlags p_usage, VkMemoryPropertyFlags p_properties, VkImage &p_image, VkDeviceMemory &p_imageMemory);
        VkCommandBuffer _beginSingleTimeCommands(VkCommandPool &p_commandPool);
        void _endSingleTimeCommands(VkCommandPool &p_commandPool, VkCommandBuffer p_commandBuffer);
        void _transitionImageLayout(VkImage p_image, VkFormat p_format, VkImageLayout p_oldLayout, VkImageLayout p_newLayout);
        // /*todo change up by =>*/ void _insertImageMemoryBarrier(VkCommandBuffer p_cmdbuffer, VkImage p_image, VkAccessFlags p_srcAccessMask, VkAccessFlags p_dstAccessMask, VkImageLayout p_oldImageLayout, VkImageLayout p_newImageLayout, VkPipelineStageFlags p_srcStageMask, VkPipelineStageFlags p_dstStageMask, VkImageSubresourceRange p_subresourceRange);
        void _copyBufferToImage(VkBuffer p_srcBuffer, VkImage p_dstImage, uint32_t p_width, uint32_t p_height);
        VkImageView _createImageView(VkImage p_image, VkFormat p_format, VkImageAspectFlags p_aspectFlags);
        VkFormat _findDepthFormat();
        
        // ============================================= wrapper =============================================
        VkPipelineShaderStageCreateInfo _buildShader(const std::string &p_shaderPath, VkShaderModule &p_shaderModule, VkShaderStageFlagBits p_shaderFlag);
        void _createGenericPipeline(VkRenderPass p_renderPass,
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
                                            VkPipelineColorBlendStateCreateInfo p_colorBlending);
        
        void _buildAttachment(VkAttachmentDescription &p_attachments, VkFormat p_format, VkAttachmentStoreOp p_storeOp, VkAttachmentLoadOp p_loadOp, VkImageLayout p_imageLayout, VkImageLayout p_finalLayout);
        void _buildSubPassDescDepth(VkSubpassDescription &p_subpassDescriptions, VkAttachmentReference *p_depthReference);
        void _buildSubPassDescColor(VkSubpassDescription &subpassDescriptions, VkAttachmentReference *p_colorAttachments, uint32_t p_colorAttachmentCount);
        void _buildSubPassDependency(VkSubpassDependency &dependencies, uint32_t p_src, uint32_t p_dst, uint32_t p_srcStage, uint32_t p_dstStage, uint32_t p_srcAcces, uint32_t p_dstAcces);

        template <class T>
        void _copyToDeviceMem(const VkDeviceSize &p_bufferSize, T *p_data, VkDeviceMemory p_BufferMemory);

        template <typename T>
        void _stageVectorToDeviceMem(VkBuffer &p_dstBuffer, VkDeviceMemory &p_dstMemory, VkDeviceSize &p_bufferSize, std::vector<T> *p_data, VkBufferUsageFlagBits p_usageFlags);

        // ============================================= Imgui stuff =============================================
        VkDescriptorPool _imguiDescriptorPool;
        VkRenderPass _imguiRenderPass;
        VkCommandPool _imguiCommandPool;
        std::vector<VkCommandBuffer> _imguiCommandBuffers;
        std::vector<VkFramebuffer> _imguiFramebuffers;

        void _imguiCreateDescriptorPool();
        void _imguiCreateRenderPass();
        void _imguiCreateCommandPool();
        void _imguiCreateCommandBuffers();
        void _imguiCreateFrameBuffers();

        // ============================================= Debug stuff =============================================

        static VKAPI_ATTR VkBool32 VKAPI_CALL _debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT p_messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT p_messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *p_CallbackData,
            void *p_UserData)
        {

            std::string message = std::string(p_CallbackData->pMessage);
            switch (p_messageType)
            {
            case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
                message = "[Validation layer][General] " + message;
                break;
            case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
                message = "[Validation layer][Validation] " + message;
                break;
            case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
                message = "[Validation layer][Performance] " + message;
                break;
            }

            switch (p_messageSeverity)
            {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                std::cout << "[DEBUG] " << message << std::endl;
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                std::cout << "[DEBUG] " << message << std::endl;
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                std::cout << "[WARNING] " << message << std::endl;
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                std::cout << "[ERROR] " << message << std::endl;
                break;
            }

            return VK_FALSE;
        }
        VkResult _createDebugUtilsMessengerEXTWrapper(VkInstance p_instance, const VkDebugUtilsMessengerCreateInfoEXT *p_CreateInfo, const VkAllocationCallbacks *p_Allocator, VkDebugUtilsMessengerEXT *p_DebugMessenger);
        VkResult _destroyDebugUtilsMessengerEXTWrapper(VkInstance p_instance, VkDebugUtilsMessengerEXT p_debugMessenger, const VkAllocationCallbacks *p_Allocator);
    };
} // namespace Engine
} // namespace TutoVulkan

#endif