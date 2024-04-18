//
// Created by shivaaz on 10/8/23.
//

#ifndef PHOTOFX_KSVULKAN_H
#define PHOTOFX_KSVULKAN_H

#include "vulkan_wrapper.h"
#include "KSVkDevice.h"
#include "KSVKTexture.h"
#include "Renderers/KSVkRenderer.h"

class KSVulkan {

public:

    bool initialize(ANativeWindow *window);
    void terminate(void);//TODO optimize
    void deleteSwapChain();//TODO Move to Swapchian
    void drawFrame();

private:

    KSVkRenderer *renderer;

    KSVkDevice device;
    KSSwapChain swapChain;
    KSRenderPass render;
    KSVKBufferInfo buffers;
    KSVKGfxPipelineInfo pipeline;
    const int textureCnt = 1;
    KSVKTexture texture;
    static const VkFormat textutreFormat = VK_FORMAT_R8G8B8A8_UNORM;

    // Global variables
   /* VkInstance vkInstance;
    VkPhysicalDevice vkGpu;
    VkDevice vkDevice;
    VkSurfaceKHR vkSurface;*/

    bool createFrameBuffers(VkImageView depthView = VK_NULL_HANDLE);

    bool createTexture();
    VkResult loadTexture(VkImageUsageFlags usage, VkFlags requiredProps);
    //vertex Buffers
    void createBuffers();


    bool createPipeline();
    VkResult createDescriptorSet();

    void deletePipeline();

    bool bInitialized = false;

    void setImageLayout(VkCommandBuffer cmdBuffer, VkImage image, VkImageLayout oldImageLayout,
                        VkImageLayout newImageLayout, VkPipelineStageFlags srcStages,
                        VkPipelineStageFlags destStages);


    VkResult AllocateMemoryTypeFromProperties(uint32_t typeBits,VkFlags requirements_mask,uint32_t* typeIndex);

};


#endif //PHOTOFX_KSVULKAN_H
