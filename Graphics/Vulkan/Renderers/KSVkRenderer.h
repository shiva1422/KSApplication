//
// Created by shivaaz on 11/2/23.
//

#ifndef KSVULKAN_KSVULKANRENDERER_H
#define KSVULKAN_KSVULKANRENDERER_H

#include <assert.h>
#include "../../KSWindow.h"
#include "../KSVkDevice.h"

class KSVkRenderer{

public:

    KSVkRenderer() = delete;

    KSVkRenderer(KSWindow &window);

    ~KSVkRenderer();

    KSVkRenderer(const KSVkRenderer &) = delete;

    KSVkRenderer& operator=(const KSVkRenderer &) = delete;

    bool initialize();

    bool isFrameInProgress()const{return bFrameStarted;}

    VkCommandBuffer getCurrentCommandBuffer()
    {
        assert(bFrameStarted && "Cannot get command Buffer when in Progress");
        return commandBuffers[currentImgIndex];
    }

    VkCommandBuffer beginFrame();

    void endFrame();

    void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);

    void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

    bool shutdown();

private:

    void createCommandBuffers();

    void freeCommandBuffers();

    void recreateSwapChain();


    std::vector<VkCommandBuffer> commandBuffers;
    uint32_t currentImgIndex;
    bool bFrameStarted = false;//Is rendering started.

};


#endif //KSVULKAN_KSVULKANRENDERER_H
