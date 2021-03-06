#ifndef __VULKAN__H_
#define __VULKAN__H_

#pragma warning(push)
#pragma warning(disable : 4201)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>
#pragma warning(pop)

#ifdef WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#define PLATFORM_SURFACE_EXTENSION_NAME VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#define PlatformSurfaceCreateInfo VkWin32SurfaceCreateInfoKHR
#define PLATFORM_SURFACE_CREATE_INFO VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
#define PlatformCreateSurface vkCreateWin32SurfaceKHR
#include "vulkan/vulkan.h"
#else
#define VK_USE_PLATFORM_XLIB_KHR
#define PLATFORM_SURFACE_EXTENSION_NAME VK_KHR_XCB_SURFACE_EXTENSION_NAME
#define PlatformSurfaceCreateInfo VkXcbSurfaceCreateInfoKHR
#define PLATFORM_SURFACE_CREATE_INFO VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
#define PlatformCreateSurface vkCreateXcbSurfaceKHR
#include "xcb/xcb.h"
#include "vulkan/vulkan.h"
#include "vulkan/vulkan_xcb.h"
#endif

#define DEMO_TEXTURE_COUNT 1
#define VERTEX_BUFFER_BIND_ID 0

typedef struct _SwapchainBuffers
{
    VkImage image;
    VkCommandBuffer cmd;
    VkImageView view;
} SwapchainBuffers;

struct VulkanBuffers
{
    VkBuffer bufs[100];
    VkDeviceMemory mems[100];
    memory_index maxNum;
    memory_index count;
};

struct VulkanVertices
{
    VkBuffer buf;
    VkDeviceMemory mem;

    VkPipelineVertexInputStateCreateInfo vi;
    VkVertexInputBindingDescription viBindings[1];
    VkVertexInputAttributeDescription viAttrs[3];
};

struct UniformBufferObject
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
};

struct UniformObject
{
    VkBuffer buf;
    VkDeviceMemory mem;
    VkBufferCreateInfo bufInfo;
    VkDescriptorBufferInfo bufferInfo;
};

struct VulkanDepth
{
    VkFormat format;
    VkImage image;
    VkDeviceMemory mem;
    VkImageView view;
};

struct TextureObject
{
    VkSampler sampler;

    VkImage image;
    VkImageLayout imageLayout;

    VkDeviceMemory mem;
    VkImageView view;
    u32 texWidth, texHeight, texPitch;

    void *data;
    memory_index dataSize;

    char *imagePath;
};

struct VulkanDescriptorSetInfo
{
    VkDescriptorSet descSet;
    TextureObject *textureObj;
    u32 textureObjCount;
    UniformObject *uniformData;
    UniformObject *uniformDataFragment;
    char *name;

    char *imagePath;
};

struct VulkanContext
{
    VkDevice device;
    VkSurfaceKHR surface;
    VkPhysicalDevice gpu;

    VkSwapchainKHR swapchain;
    SwapchainBuffers *buffers;

    VkFramebuffer *framebuffers;

    VkQueue queue;
    VkCommandBuffer setupCmd;
    VkCommandBuffer drawCmd;  // Command Buffer for drawing commands
    VkCommandPool cmdPool;

    VkRenderPass renderPass;
    VkRenderPass renderPassWithNoClear;
    VkPipeline pipeline;
    VkPipeline pipeline2;
    VkPipelineLayout pipelineLayout;
    VkSemaphore presentCompleteSemaphore;

    VkFormat format;

    VulkanVertices vertices;

    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;

    VkDescriptorPool descPool;
    VkDescriptorSet *descSet;
    VkDescriptorSet secondDescSet;
    VkDescriptorSetLayout descLayout;

    PFN_vkCreateDebugReportCallbackEXT CreateDebugReportCallback;
    PFN_vkDestroyDebugReportCallbackEXT DestroyDebugReportCallback;
	PFN_vkDebugReportMessageEXT DebugReportMessage;
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR
        fpGetPhysicalDeviceSurfaceSupportKHR;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR
        fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR
        fpGetPhysicalDeviceSurfaceFormatsKHR;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR
        fpGetPhysicalDeviceSurfacePresentModesKHR;
    PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR;
    PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR;
    PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR;
    PFN_vkAcquireNextImageKHR fpAcquireNextImageKHR;
    PFN_vkQueuePresentKHR fpQueuePresentKHR;

    uint32_t currentBufferIndex;

    int32_t curFrame;
	int32_t frameCount;
    uint32_t width;
    uint32_t height;

    float depthStencil;
    float depthIncrement;

    b32 quit;

    UniformObject uniformData;
    UniformObject uniformDataFragment;
    VulkanDepth depth;
    TextureObject UITextures[DEMO_TEXTURE_COUNT];

    uint32_t swapchainImageCount;

    VkPhysicalDeviceMemoryProperties memoryProperties;

    VulkanDescriptorSetInfo *vdsi;
};

#endif
