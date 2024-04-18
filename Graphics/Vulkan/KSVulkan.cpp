//
// Created by shivaaz on 10/8/23.
//

#include "KSVulkan.h"
#include <vector>
#include "cstring"
#include <Logger/KSLog.h>
#include <assert.h>
#include <shaderc/shaderc.h>
#include <shaderc/shaderc.hpp>
#include "LayersAndExtensions.h"
#include "ShaderManager.h"
#include "KSIO/IKSStream.h"
#include "KSIO/AssetManager.h"
#include "CMedia/KSImage.h"
#include "KSMedia/KSImageLoader.h"


// Android log function wrappers
#define LOGTAG  "KSVULKAN"


// Vulkan call wrapper
#define CALL_VK(func)                                                 \
  if (VK_SUCCESS != (func)) {                                         \
    __android_log_print(ANDROID_LOG_ERROR, "KSVULKAN ",               \
                        "Vulkan error. File[%s], line[%d]", __FILE__, \
                        __LINE__);                                    \
    assert(false);                                                    \
  }

// A macro to check value is VK_SUCCESS
// Used also for non-vulkan functions but return VK_SUCCESS
#define VK_CHECK(x) CALL_VK(x)

bool KSVulkan::initialize(ANativeWindow *window)
{
    // Load Android vulkan and retrieve vulkan API function pointers
    if (!InitVulkan())
    {
       // KSLOGE("Vulkan is unavailable, install vulkan and re-start");
        return false;
    }

    VkApplicationInfo appInfo = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .pApplicationName = "KSVulkanApp",
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .pEngineName = "KSVulkanEngine",
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion = VK_MAKE_VERSION(1, 1, 0),
    };

    assert(device.create(window,&appInfo));
    assert(swapChain.create(device));

    /************* create render pass*/
    VkAttachmentDescription attachmentDescriptions
    {
            .format = swapChain.displayFormat,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };

    VkAttachmentReference colourReference = {
            .attachment = 0, .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

    VkSubpassDescription subpassDescription{
            .flags = 0,
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .inputAttachmentCount = 0,
            .pInputAttachments = nullptr,
            .colorAttachmentCount = 1,
            .pColorAttachments = &colourReference,
            .pResolveAttachments = nullptr,
            .pDepthStencilAttachment = nullptr,
            .preserveAttachmentCount = 0,
            .pPreserveAttachments = nullptr,
    };
    VkRenderPassCreateInfo renderPassCreateInfo{
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .pNext = nullptr,
            .attachmentCount = 1,
            .pAttachments = &attachmentDescriptions,
            .subpassCount = 1,
            .pSubpasses = &subpassDescription,
            .dependencyCount = 0,
            .pDependencies = nullptr,
    };
    CALL_VK(vkCreateRenderPass(device.vkDevice, &renderPassCreateInfo, nullptr,
                               &render.renderPass));

    createFrameBuffers();

    createTexture();

    createBuffers();

    createPipeline();

    createDescriptorSet();


    //// -----------------------------------------------
    // Create a pool of command buffers to allocate command buffer from
    VkCommandPoolCreateInfo cmdPoolCreateInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = device.queueFamilyIndex,
    };
    CALL_VK(vkCreateCommandPool(device.vkDevice, &cmdPoolCreateInfo, nullptr,
                                &render.cmdPool));

    // 1 command buffer draw in 1 framebuffer
    // 2 command as there are  2 framebuffers
    render.cmdBufferLen = swapChain.swapchainLength;
    render.cmdBuffer = new VkCommandBuffer[swapChain.swapchainLength];
    for (int bufferIndex = 0; bufferIndex < swapChain.swapchainLength;bufferIndex++)
    {
        //  start by creating and declare the "beginning" our command buffer
        VkCommandBufferAllocateInfo cmdBufferCreateInfo{
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                .pNext = nullptr,
                .commandPool = render.cmdPool,
                .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                .commandBufferCount = 1,
        };
        CALL_VK(vkAllocateCommandBuffers(device.vkDevice, &cmdBufferCreateInfo,
                                         &render.cmdBuffer[bufferIndex]));

        VkCommandBufferBeginInfo cmdBufferBeginInfo{
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .pNext = nullptr,
                .flags = 0,
                .pInheritanceInfo = nullptr,
        };
        CALL_VK(vkBeginCommandBuffer(render.cmdBuffer[bufferIndex],
                                     &cmdBufferBeginInfo));
        // transition the display image to color attachment layout
        setImageLayout(render.cmdBuffer[bufferIndex],
                       swapChain.displayImages[bufferIndex],
                       VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                       VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                       VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                       VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
        // Now we start a renderpass. Any draw command has to be recorded in a
        // renderpass
        VkClearValue clearVals {.color = { .float32 = {0.0f, 0.34f, 0.90f,1.0f}}};

        VkRenderPassBeginInfo renderPassBeginInfo{
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                .pNext = nullptr,
                .renderPass = render.renderPass,
                .framebuffer = swapChain.framebuffers[bufferIndex],
                .renderArea = {  .offset ={.x = 0, .y = 0,}, .extent = swapChain.displaySize},
                .clearValueCount = 1,
                .pClearValues = &clearVals
        };

        vkCmdBeginRenderPass(render.cmdBuffer[bufferIndex], &renderPassBeginInfo,
                             VK_SUBPASS_CONTENTS_INLINE);
        // Bind what is necessary to the command buffer
        vkCmdBindPipeline(render.cmdBuffer[bufferIndex],
                          VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline);
        vkCmdBindDescriptorSets(
                render.cmdBuffer[bufferIndex], VK_PIPELINE_BIND_POINT_GRAPHICS,
                pipeline.layout, 0, 1, &pipeline.descSet, 0, nullptr);
        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(render.cmdBuffer[bufferIndex], 0, 1,
                               &buffers.vertexBuf, &offset);

        // Draw Triangle
        vkCmdDraw(render.cmdBuffer[bufferIndex], 3, 1, 0, 0);

        vkCmdEndRenderPass(render.cmdBuffer[bufferIndex]);
        // transition back to swapchain image to PRESENT_SRC_KHR
       // setImageLayout(render.cmdBuffer[bufferIndex],swapChain.displayImages[bufferIndex],VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
        CALL_VK(vkEndCommandBuffer(render.cmdBuffer[bufferIndex]));
    }

    // We need to create a fence to be able, in the main loop, to wait for our
    // draw command(s) to finish before swapping the framebuffers
    VkFenceCreateInfo fenceCreateInfo{
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
    };
    CALL_VK(vkCreateFence(device.vkDevice, &fenceCreateInfo, nullptr, &render.fence));

    // We need to create a semaphore to be able to wait, in the main loop, for our
    // framebuffer to be available for us before drawing.
    VkSemaphoreCreateInfo semaphoreCreateInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
    };
    CALL_VK(vkCreateSemaphore(device.vkDevice, &semaphoreCreateInfo, nullptr,
                              &render.semaphore));
    return true;
}

bool KSVulkan::createFrameBuffers(VkImageView depthView/* = VK_NULL_HANDLE*/)
{
    //creating 2 framebufferes now.
    // query display attachment to swapchain
    uint32_t SwapchainImagesCount = 0;
    CALL_VK(vkGetSwapchainImagesKHR(device.vkDevice, swapChain.swapchain,
                                    &SwapchainImagesCount, nullptr));
    swapChain.displayImages.resize(SwapchainImagesCount);
    CALL_VK(vkGetSwapchainImagesKHR(device.vkDevice, swapChain.swapchain,
                                    &SwapchainImagesCount,
                                    swapChain.displayImages.data()));

    // create image view for each swapchain image
    swapChain.displayViews.resize(SwapchainImagesCount);
    for (uint32_t i = 0; i < SwapchainImagesCount; i++)
    {
        VkImageViewCreateInfo viewCreateInfo ={
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .image = swapChain.displayImages[i],
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = swapChain.displayFormat,
                .components =
                        {
                                .r = VK_COMPONENT_SWIZZLE_R,
                                .g = VK_COMPONENT_SWIZZLE_G,
                                .b = VK_COMPONENT_SWIZZLE_B,
                                .a = VK_COMPONENT_SWIZZLE_A,
                        },
                .subresourceRange =
                        {
                                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                .baseMipLevel = 0,
                                .levelCount = 1,
                                .baseArrayLayer = 0,
                                .layerCount = 1,
                        },
        };
        CALL_VK(vkCreateImageView(device.vkDevice, &viewCreateInfo, nullptr,
                                  &swapChain.displayViews[i]));
    }

    // create a framebuffer from each swapchain image
    swapChain.framebuffers.resize(swapChain.swapchainLength);
    for (uint32_t i = 0; i < swapChain.swapchainLength; i++) {
        VkImageView attachments[2] = {
                swapChain.displayViews[i], depthView,
        };
        VkFramebufferCreateInfo fbCreateInfo{
                .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .pNext = nullptr,
                .renderPass = render.renderPass,
                .attachmentCount = 1,  // 2 if using depth
                .pAttachments = attachments,
                .width = static_cast<uint32_t>(swapChain.displaySize.width),
                .height = static_cast<uint32_t>(swapChain.displaySize.height),
                .layers = 1,
        };
        fbCreateInfo.attachmentCount = (depthView == VK_NULL_HANDLE ? 1 : 2);//TODO frameBuffer Count.

        CALL_VK(vkCreateFramebuffer(device.vkDevice, &fbCreateInfo, nullptr,
                                    &swapChain.framebuffers[i]));

    }
    return  true;
}

/*
 * setImageLayout():
 *    Helper function to transition color buffer layout
 */
void KSVulkan::setImageLayout(VkCommandBuffer cmdBuffer, VkImage image,VkImageLayout oldImageLayout, VkImageLayout newImageLayout,VkPipelineStageFlags srcStages,VkPipelineStageFlags destStages)
{
    VkImageMemoryBarrier imageMemoryBarrier = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .pNext = NULL,
            .srcAccessMask = 0,
            .dstAccessMask = 0,
            .oldLayout = oldImageLayout,
            .newLayout = newImageLayout,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = image,
            .subresourceRange =
                    {
                            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                            .baseMipLevel = 0,
                            .levelCount = 1,
                            .baseArrayLayer = 0,
                            .layerCount = 1,
                    },
    };



    switch (oldImageLayout)
    {
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_PREINITIALIZED:
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
            break;

        default:
            break;
    }

    switch (newImageLayout) {
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            break;

        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            break;

        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            imageMemoryBarrier.dstAccessMask =
                    VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            break;

        default:
            break;
    }

    vkCmdPipelineBarrier(cmdBuffer, srcStages, destStages, 0, 0, NULL, 0, NULL, 1,
                         &imageMemoryBarrier);
}

void KSVulkan::drawFrame()
{
    uint32_t nextIndex;
    // Get the framebuffer index we should draw in
    CALL_VK(vkAcquireNextImageKHR(device.vkDevice, swapChain.swapchain,
                                  UINT64_MAX, render.semaphore, VK_NULL_HANDLE,
                                  &nextIndex));
    CALL_VK(vkResetFences(device.vkDevice, 1, &render.fence));

    VkPipelineStageFlags waitStageMask =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submit_info = {.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &render.semaphore,
            .pWaitDstStageMask = &waitStageMask,
            .commandBufferCount = 1,
            .pCommandBuffers = &render.cmdBuffer[nextIndex],
            .signalSemaphoreCount = 0,
            .pSignalSemaphores = nullptr};
    CALL_VK(vkQueueSubmit(device.queue, 1, &submit_info, render.fence));
    CALL_VK(
            vkWaitForFences(device.vkDevice, 1, &render.fence, VK_TRUE, 100000000));

    VkResult result;
    VkPresentInfoKHR presentInfo{
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = nullptr,
            .waitSemaphoreCount = 0,
            .pWaitSemaphores = nullptr,
            .swapchainCount = 1,
            .pSwapchains = &swapChain.swapchain,
            .pImageIndices = &nextIndex,
            .pResults = &result,
    };
    vkQueuePresentKHR(device.queue, &presentInfo);

   // return true;//todo
}

void KSVulkan::createBuffers()//TODO destroyBuffer
{

    // Vertex positions
    // Vertex positions
    const float vertexData[] = {
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f,
            1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
    };

    // Create a vertex buffer
    VkBufferCreateInfo createBufferInfo{
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .size = sizeof(vertexData),
            .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 1,
            .pQueueFamilyIndices = &device.queueFamilyIndex,
    };

    CALL_VK(vkCreateBuffer(device.vkDevice, &createBufferInfo, nullptr,
                           &buffers.vertexBuf));
    VkMemoryRequirements memReq;
    vkGetBufferMemoryRequirements(device.vkDevice, buffers.vertexBuf, &memReq);

    VkMemoryAllocateInfo allocInfo{
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext = nullptr,
            .allocationSize = memReq.size,
            .memoryTypeIndex = 0,  // Memory type assigned in the next step
    };

    AllocateMemoryTypeFromProperties(memReq.memoryTypeBits,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,&allocInfo.memoryTypeIndex);

    VkDeviceMemory deviceMemory;
    CALL_VK(vkAllocateMemory(device.vkDevice, &allocInfo, nullptr, &deviceMemory));

    void* data;
    CALL_VK(vkMapMemory(device.vkDevice, deviceMemory, 0, allocInfo.allocationSize,
                        0, &data));
    memcpy(data, vertexData, sizeof(vertexData));
    vkUnmapMemory(device.vkDevice, deviceMemory);

    CALL_VK(vkBindBufferMemory(device.vkDevice, buffers.vertexBuf, deviceMemory, 0));

}

bool KSVulkan::createPipeline()
{
    memset(&pipeline, 0, sizeof(pipeline));

    const VkDescriptorSetLayoutBinding descriptorSetLayoutBinding{
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,//TODO can be multiple dynamic
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = nullptr,
    };
    const VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .bindingCount = 1,
            .pBindings = &descriptorSetLayoutBinding,
    };

    CALL_VK(vkCreateDescriptorSetLayout(device.vkDevice,&descriptorSetLayoutCreateInfo, nullptr,&pipeline.dscLayout));

    // Create pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .setLayoutCount = 1,
            .pSetLayouts = &pipeline.dscLayout,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr,
    };
    CALL_VK(vkCreatePipelineLayout(device.vkDevice, &pipelineLayoutCreateInfo,
                                   nullptr, &pipeline.layout));

    VkPipelineDynamicStateCreateInfo dynamicStateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .pNext = nullptr,
            .dynamicStateCount = 0,
            .pDynamicStates = nullptr};

    VkShaderModule vertexShader, fragmentShader;
    //result check

    device.loadShader("Shaders/textVert.glsl",&vertexShader,VERTEX_SHADER);
    device.loadShader("Shaders/textFrag.glsl",&fragmentShader,FRAGMENT_SHADER);


    // Specify vertex and fragment shader stages
    VkPipelineShaderStageCreateInfo shaderStages[2]{
            {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = 0,
                    .stage = VK_SHADER_STAGE_VERTEX_BIT,
                    .module = vertexShader,
                    .pName = "main",
                    .pSpecializationInfo = nullptr,
            },
            {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = 0,
                    .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                    .module = fragmentShader,
                    .pName = "main",
                    .pSpecializationInfo = nullptr,
            }};

    VkViewport viewports{
            .x = 0,
            .y = 0,
            .width = (float)swapChain.displaySize.width,
            .height = (float)swapChain.displaySize.height,
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
    };

    VkRect2D scissor = {
            .offset = { .x = 0, .y = 0,},
            .extent = swapChain.displaySize,
    };

    // Specify viewport info
    VkPipelineViewportStateCreateInfo viewportInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .viewportCount = 1,
            .pViewports = &viewports,
            .scissorCount = 1,
            .pScissors = &scissor,
    };

    // Specify multisample info
    VkSampleMask sampleMask = ~0u;
    VkPipelineMultisampleStateCreateInfo multisampleInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .pNext = nullptr,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable = VK_FALSE,
            .minSampleShading = 0,
            .pSampleMask = &sampleMask,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE,
    };

    // Specify color blend state
    VkPipelineColorBlendAttachmentState attachmentStates{
            .blendEnable = VK_FALSE,
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                              VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };

    VkPipelineColorBlendStateCreateInfo colorBlendInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_COPY,
            .attachmentCount = 1,
            .pAttachments = &attachmentStates,
    };

    // Specify rasterizer info
    VkPipelineRasterizationStateCreateInfo rasterInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .pNext = nullptr,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_NONE,
            .frontFace = VK_FRONT_FACE_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .lineWidth = 1,
    };

    // Specify input assembler state
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .pNext = nullptr,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE,
    };

    // Specify vertex input state
    VkVertexInputBindingDescription vertexInputBinding{
            .binding = 0,
            .stride = 3 * sizeof(float),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };
    VkVertexInputAttributeDescription vertexInputAttributes[2]{
            {
                    .location = 0,
                    .binding = 0,
                    .format = VK_FORMAT_R32G32B32_SFLOAT,
                    .offset = 0,
            },
            {
                    .location = 1,
                    .binding = 0,
                    .format = VK_FORMAT_R32G32_SFLOAT,
                    .offset = sizeof(float) * 3,
            }};

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .vertexBindingDescriptionCount = 1,
            .pVertexBindingDescriptions = &vertexInputBinding,
            .vertexAttributeDescriptionCount = 2,
            .pVertexAttributeDescriptions = vertexInputAttributes,
    };


    // Create the pipeline cache
    VkPipelineCacheCreateInfo pipelineCacheInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,  // reserved, must be 0
            .initialDataSize = 0,
            .pInitialData = nullptr,
    };

    CALL_VK(vkCreatePipelineCache(device.vkDevice, &pipelineCacheInfo, nullptr,
                                  &pipeline.cache));


    // Create the pipeline
    VkGraphicsPipelineCreateInfo pipelineCreateInfo{
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stageCount = 2,
            .pStages = shaderStages,
            .pVertexInputState = &vertexInputInfo,
            .pInputAssemblyState = &inputAssemblyInfo,
            .pTessellationState = nullptr,
            .pViewportState = &viewportInfo,
            .pRasterizationState = &rasterInfo,
            .pMultisampleState = &multisampleInfo,
            .pDepthStencilState = nullptr,
            .pColorBlendState = &colorBlendInfo,
            .pDynamicState = &dynamicStateInfo,
            .layout = pipeline.layout,
            .renderPass = render.renderPass,
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = 0,
    };

    VkResult pipelineResult = vkCreateGraphicsPipelines(
            device.vkDevice, pipeline.cache, 1, &pipelineCreateInfo, nullptr,
            &pipeline.pipeline);

    //  don't need the shaders anymore,  can release their memory
    vkDestroyShaderModule(device.vkDevice, vertexShader, nullptr);
    vkDestroyShaderModule(device.vkDevice, fragmentShader, nullptr);

    return pipelineResult;
}


void KSVulkan::terminate(void)
{
    vkFreeCommandBuffers(device.vkDevice, render.cmdPool, render.cmdBufferLen,
                         render.cmdBuffer);
    delete[] render.cmdBuffer;

    vkDestroyCommandPool(device.vkDevice, render.cmdPool, nullptr);
    vkDestroyRenderPass(device.vkDevice, render.renderPass, nullptr);
    deleteSwapChain();

    vkDestroyDevice(device.vkDevice, nullptr);
    vkDestroyInstance(device.vkInstance, nullptr);

    device.bInitialized = false;

    bInitialized = false;
}

void KSVulkan::deleteSwapChain()
{
    for (int i = 0; i < swapChain.swapchainLength; i++)
    {
        vkDestroyFramebuffer(device.vkDevice, swapChain.framebuffers[i], nullptr);
        vkDestroyImageView(device.vkDevice, swapChain.displayViews[i], nullptr);
    }
    vkDestroySwapchainKHR(device.vkDevice, swapChain.swapchain, nullptr);
}

void KSVulkan::deletePipeline()
{
    if (pipeline.pipeline == VK_NULL_HANDLE) return;
    vkDestroyPipeline(device.vkDevice, pipeline.pipeline, nullptr);
    vkDestroyPipelineCache(device.vkDevice, pipeline.cache, nullptr);
    vkDestroyPipelineLayout(device.vkDevice, pipeline.layout, nullptr);

}



bool KSVulkan::createTexture()
{
    loadTexture(VK_IMAGE_USAGE_SAMPLED_BIT,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    const VkSamplerCreateInfo sampler = {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .pNext = nullptr,
            .magFilter = VK_FILTER_NEAREST,
            .minFilter = VK_FILTER_NEAREST,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
            .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .mipLodBias = 0.0f,
            .maxAnisotropy = 1,
            .compareOp = VK_COMPARE_OP_NEVER,
            .minLod = 0.0f,
            .maxLod = 0.0f,
            .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
            .unnormalizedCoordinates = VK_FALSE,
    };
    VkImageViewCreateInfo view = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = texture.image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = textutreFormat,
            .components =
                    {
                            VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G,
                            VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A,
                    },
            .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
    };

    CALL_VK(vkCreateSampler(device.vkDevice, &sampler, nullptr,
                            &texture.sampler));
    CALL_VK(vkCreateImageView(device.vkDevice, &view, nullptr, &texture.view));
    return true;
}

VkResult KSVulkan::loadTexture(VkImageUsageFlags usage, VkFlags requiredProps)
{

    if (!(usage | requiredProps))
    {
        KSLOGE(LOGTAG,"Load Texture : no usage and required props");
        return VK_ERROR_FORMAT_NOT_SUPPORTED;
    }

    // Check for linear supportability
    VkFormatProperties props;
    bool needBlit = true;
    vkGetPhysicalDeviceFormatProperties(device.vkGpu, textutreFormat, &props);
    assert((props.linearTilingFeatures | props.optimalTilingFeatures) & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);

    if (props.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) {
        // linear format supporting the required texture
        needBlit = false;
    }

    KSImage image = KSImageLoader::loadFromAsset("Images/skin.jpg");
    //assert(n == 4); 4 channels

    texture.width = image.width;
    texture.height = image.height;

    // Allocate the linear texture so texture could be copied over
    VkImageCreateInfo imageCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = textutreFormat,
            .extent = {static_cast<uint32_t>(texture.width),
                       static_cast<uint32_t>(texture.height), 1},
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_LINEAR,
            .usage = (needBlit ? VK_IMAGE_USAGE_TRANSFER_SRC_BIT
                               : VK_IMAGE_USAGE_SAMPLED_BIT),
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 1,
            .pQueueFamilyIndices = &device.queueFamilyIndex,
            .initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED,
    };
    VkMemoryAllocateInfo memoryAllocateInfo = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext = nullptr,
            .allocationSize = 0,
            .memoryTypeIndex = 0,
    };

    VkMemoryRequirements memReqs;
    CALL_VK(vkCreateImage(device.vkDevice, &imageCreateInfo, nullptr,&texture.image));
    vkGetImageMemoryRequirements(device.vkDevice, texture.image, &memReqs);
    memoryAllocateInfo.allocationSize = memReqs.size;
    VK_CHECK(AllocateMemoryTypeFromProperties(memReqs.memoryTypeBits,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,&memoryAllocateInfo.memoryTypeIndex));
    CALL_VK(vkAllocateMemory(device.vkDevice, &memoryAllocateInfo, nullptr, &texture.deviceMemory));
    CALL_VK(vkBindImageMemory(device.vkDevice, texture.image, texture.deviceMemory, 0));

    if (requiredProps & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
    {
        const VkImageSubresource subres = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .mipLevel = 0, .arrayLayer = 0,};
        VkSubresourceLayout layout;
        void* data;

        vkGetImageSubresourceLayout(device.vkDevice, texture.image, &subres,&layout);
        CALL_VK(vkMapMemory(device.vkDevice, texture.deviceMemory, 0, memoryAllocateInfo.allocationSize, 0, &data));

        for (int32_t y = 0; y < image.height; y++) {
            unsigned char* row = (unsigned char*)((char*)data + layout.rowPitch * y);
            for (int32_t x = 0; x < image.width; x++) {
                row[x * 4] = image.data[(x + y * image.width) * 4];
                row[x * 4 + 1] = image.data[(x + y * image.width) * 4 + 1];
                row[x * 4 + 2] = image.data[(x + y * image.width) * 4 + 2];
                row[x * 4 + 3] = image.data[(x + y * image.width) * 4 + 3];
            }
        }

        vkUnmapMemory(device.vkDevice, texture.deviceMemory);
    }
    texture.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkCommandPoolCreateInfo cmdPoolCreateInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = device.queueFamilyIndex,
    };

    VkCommandPool cmdPool;
    CALL_VK(vkCreateCommandPool(device.vkDevice, &cmdPoolCreateInfo, nullptr,&cmdPool));

    VkCommandBuffer gfxCmd;
    const VkCommandBufferAllocateInfo cmd = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = cmdPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
    };

    CALL_VK(vkAllocateCommandBuffers(device.vkDevice, &cmd, &gfxCmd));
    VkCommandBufferBeginInfo cmd_buf_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pInheritanceInfo = nullptr};
    CALL_VK(vkBeginCommandBuffer(gfxCmd, &cmd_buf_info));

    // If linear is supported, we are done
    VkImage stageImage = VK_NULL_HANDLE;
    VkDeviceMemory stageMem = VK_NULL_HANDLE;
    if (!needBlit)
    {

        setImageLayout(gfxCmd, texture.image, VK_IMAGE_LAYOUT_PREINITIALIZED,
                       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                       VK_PIPELINE_STAGE_HOST_BIT,
                       VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
    } else
    {
        // save current image and mem as staging image and memory
        stageImage = texture.image;
        stageMem = texture.deviceMemory;
        texture.image = VK_NULL_HANDLE;
        texture.deviceMemory = VK_NULL_HANDLE;

        // Create a tile texture to blit into
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.usage =VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        CALL_VK(vkCreateImage(device.vkDevice, &imageCreateInfo, nullptr,&texture.image));
        vkGetImageMemoryRequirements(device.vkDevice, texture.image, &memReqs);

        memoryAllocateInfo.allocationSize = memReqs.size;
        VK_CHECK(AllocateMemoryTypeFromProperties(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,&memoryAllocateInfo.memoryTypeIndex));
        CALL_VK(vkAllocateMemory(device.vkDevice, &memoryAllocateInfo, nullptr, &texture.deviceMemory));
        CALL_VK(vkBindImageMemory(device.vkDevice, texture.image, texture.deviceMemory, 0));

        // transitions image out of UNDEFINED type
        setImageLayout(gfxCmd, stageImage, VK_IMAGE_LAYOUT_PREINITIALIZED,VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
        setImageLayout(gfxCmd, texture.image, VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
        VkImageCopy bltInfo{
                .srcSubresource {
                        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                        .mipLevel = 0,
                        .baseArrayLayer = 0,
                        .layerCount = 1,
                },
                .srcOffset { .x = 0, .y = 0, .z = 0 },
                .dstSubresource {
                        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                        .mipLevel = 0,
                        .baseArrayLayer = 0,
                        .layerCount = 1,
                },
                .dstOffset { .x = 0, .y = 0, .z = 0},
                .extent { .width = static_cast<uint32_t>(texture.width), .height = static_cast<uint32_t>(texture.height), .depth = 1,},
        };
        vkCmdCopyImage(gfxCmd, stageImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,texture.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,&bltInfo);

        setImageLayout(gfxCmd, texture.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,VK_PIPELINE_STAGE_TRANSFER_BIT,VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
    }

    CALL_VK(vkEndCommandBuffer(gfxCmd));
    VkFenceCreateInfo fenceInfo = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
    };
    VkFence fence;
    CALL_VK(vkCreateFence(device.vkDevice, &fenceInfo, nullptr, &fence));

    VkSubmitInfo submitInfo = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = 0,
            .pWaitSemaphores = nullptr,
            .pWaitDstStageMask = nullptr,
            .commandBufferCount = 1,
            .pCommandBuffers = &gfxCmd,
            .signalSemaphoreCount = 0,
            .pSignalSemaphores = nullptr,
    };
    CALL_VK(vkQueueSubmit(device.queue, 1, &submitInfo, fence) != VK_SUCCESS);
    CALL_VK(vkWaitForFences(device.vkDevice, 1, &fence, VK_TRUE, 100000000) !=
            VK_SUCCESS);
    vkDestroyFence(device.vkDevice, fence, nullptr);

    vkFreeCommandBuffers(device.vkDevice, cmdPool, 1, &gfxCmd);
    vkDestroyCommandPool(device.vkDevice, cmdPool, nullptr);
    if (stageImage != VK_NULL_HANDLE)
    {
        vkDestroyImage(device.vkDevice, stageImage, nullptr);
        vkFreeMemory(device.vkDevice, stageMem, nullptr);
    }

    return VK_SUCCESS;
}

VkResult KSVulkan::createDescriptorSet()
{
    const VkDescriptorPoolSize typeCount = {
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,//Texture count
    };

    const VkDescriptorPoolCreateInfo descriptorPool = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .pNext = nullptr,
            .maxSets = 1,
            .poolSizeCount = 1,
            .pPoolSizes = &typeCount,
    };

    CALL_VK(vkCreateDescriptorPool(device.vkDevice, &descriptorPool, nullptr,
                                   &pipeline.descPool));

    VkDescriptorSetAllocateInfo allocateInfo{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext = nullptr,
            .descriptorPool = pipeline.descPool,
            .descriptorSetCount = 1,
            .pSetLayouts = &pipeline.dscLayout
    };

    CALL_VK(vkAllocateDescriptorSets(device.vkDevice, &allocateInfo,&pipeline.descSet));

    VkDescriptorImageInfo texDsts[1];//texturecnt
    memset(texDsts, 0, sizeof(texDsts));
    for (int32_t idx = 0; idx < 1/*textureCnt)*/; idx++)
    {
        texDsts[idx].sampler = texture.sampler;//textures[i]
        texDsts[idx].imageView = texture.view;
        texDsts[idx].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    }
    VkWriteDescriptorSet writeDst{
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = nullptr,
            .dstSet = pipeline.descSet,
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,//textcnt
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .pImageInfo = texDsts,
            .pBufferInfo = nullptr,
            .pTexelBufferView = nullptr
    };

    vkUpdateDescriptorSets(device.vkDevice, 1, &writeDst, 0, nullptr);
    return VK_SUCCESS;



    return VK_ERROR_INITIALIZATION_FAILED;
}

// A help function to map required memory property into a VK memory type
// memory type is an index into the array of 32 entries; or the bit index
// for the memory type ( each BIT of an 32 bit integer is a type )
VkResult KSVulkan::AllocateMemoryTypeFromProperties(uint32_t typeBits, VkFlags requirements_mask,uint32_t *typeIndex)
{
    // Search memtypes to find first index with those properties
    for (uint32_t i = 0; i < 32; i++) {
        if ((typeBits & 1) == 1) {
            // Type is available, does it match user properties?
            if ((device.gpuMemoryProperties.memoryTypes[i].propertyFlags &
                 requirements_mask) == requirements_mask) {
                *typeIndex = i;
                return VK_SUCCESS;
            }
        }
        typeBits >>= 1;
    }
    // No memory types matched, return failure
    return VK_ERROR_MEMORY_MAP_FAILED;
}
