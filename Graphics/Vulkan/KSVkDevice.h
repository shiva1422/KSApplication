//
// Created by shivaaz on 11/2/23.
//

#ifndef PHOTOFX_KSVKDEVICE_H
#define PHOTOFX_KSVKDEVICE_H

#include <vulkan/vulkan_core.h>
#include<vector>
#include"VKUtils/VulkanUtils.h"
#include "KSVkDevice.h"
#include "ShaderManager.h"

/*
 * TODO check innerclasses would be better?
 */
class ANativeWindow;

struct KSVkDevice {

public:

    KSVkDevice() = delete;
    KSVkDevice(const KSVkDevice&) = delete;
    KSVkDevice& operator=(const KSVkDevice& ) = delete;

    //creating Instance is not apt here;
    VkInstance vkInstance;
    VkPhysicalDevice vkGpu;
    VkDevice vkDevice;
    uint32_t queueFamilyIndex;
    VkPhysicalDeviceMemoryProperties gpuMemoryProperties;


    VkSurfaceKHR vkSurface;
    VkQueue queue;

    bool bInitialized = false;

    bool create(ANativeWindow *window, VkApplicationInfo *appInfo);

    VkResult loadShader(const char *filePath, VkShaderModule *shaderModule , ShaderType shaderType);

    void terminate();
};

struct KSSwapChain {

public:
    VkSwapchainKHR swapchain;
    uint32_t swapchainLength;
    VkExtent2D displaySize;
    VkFormat displayFormat;

    //array of frame buffers and views
    std::vector<VkImage> displayImages;
    std::vector<VkImageView> displayViews;
    std::vector<VkFramebuffer> framebuffers;

    bool create(const KSVkDevice &device);
};

struct KSRenderPass {

public:
    VkRenderPass renderPass;
    VkCommandPool cmdPool;
    VkCommandBuffer* cmdBuffer;
    uint32_t cmdBufferLen;
    VkSemaphore semaphore;
    VkFence fence;
};
//VulkanRenderInfo render;

struct KSVKBufferInfo {
    VkBuffer vertexBuf;
};

struct KSVKGfxPipelineInfo {
    VkDescriptorSetLayout dscLayout;
    VkDescriptorPool descPool;
    VkDescriptorSet descSet;
    VkPipelineLayout layout;
    VkPipelineCache cache;
    VkPipeline pipeline;
};

/*Descriptors :A descriptor is a way for shaders to freely access resources like buffers and images.
 * Specify a descriptor layout during pipeline creation
Allocate a descriptor set from a descriptor pool
Bind the descriptor set during rendering
 */

#endif //PHOTOFX_KSVKDEVICE_H
