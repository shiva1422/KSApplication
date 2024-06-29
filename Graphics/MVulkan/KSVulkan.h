//
// Created by pspr1 on 4/25/2021.
//

#ifndef VULKAN_KVULKAN_H
#define VULKAN_KVULKAN_H

#include <KSWindow.h>
#include "vulkan_wrapper.h"
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_android.h>
#include <vector>


#define CALL_VK(func)                                                 \
  if (VK_SUCCESS != (func)) {                                         \
    __android_log_print(ANDROID_LOG_ERROR, "KSVulkan",               \
                        "Vulkan error. File[%s], line[%d]", __FILE__, \
                        __LINE__);                                    \
    assert(false);                                                    \
  }
/*
 * TODO later create separate classes for device ,instance and global to abstract best possible ways.
 */
class QueueFamilyIndices{
//TODO optimize
private:

    friend class KSVulkan;
    //TODO 0 indicates a valid queue value so its not good to decide if its valid number try set to max of use std::optional  c++17
    uint32_t graphicsFamily = UINT32_MAX;
    uint32_t presentFamily = UINT32_MAX;
};

class SwapChainInfo{
public:

    //min/max number of images in swapchain ,and width and height of images
    VkSurfaceCapabilitiesKHR capabilities;
    //pixel format, color space
    std::vector<VkSurfaceFormatKHR> formats;

    std::vector<VkPresentModeKHR> presentModes;
};

class KSVulkan{

public:

    //Follow this for get the order of creation.
     static KSVulkan *newInstance(KSWindow *window);

     bool configure();

private:

     KSVulkan(){};

     bool createInstance();

     bool createSurface();

     bool selectPhysicalDevice();

     bool isDeviceSuitable(VkPhysicalDevice device);

     bool findGFXFamilyQueues(VkPhysicalDevice device);

     SwapChainInfo getSwapChainInfo(VkPhysicalDevice device);

     bool checkDeviceExtensionSupport(VkPhysicalDevice device);

     bool createLogicalDevice();

     bool createSwapChain();

     VkSurfaceFormatKHR surfaceChooseSwapFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

     VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

     VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) ;

     bool createImageViews();

     bool  createRenderPass();

     bool createGraphicsPipeline();

     bool createFrameBuffers();










private:


    KSWindow *window = nullptr;

    bool bInit = false;

    /* A Vulkan Instance is an object that gathers the state of an application. It encloses information such as an application name, name and version of an engine used to create an application, or enabled instance-level
     * extensions and layers.
     */
    VkInstance vkInstance;
    VkPhysicalDevice vkGpu = VK_NULL_HANDLE;
    VkDevice vkDevice = VK_NULL_HANDLE;//logical device.
    VkSurfaceKHR vkSurface;

    VkQueue graphicsQueue;
    VkQueue presentQueue;
    QueueFamilyIndices indices;

    //TODO Swap chain Info later to create class
    VkSwapchainKHR vkSwapChain;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;

    //TODO RenderPass Info later to combine to struct.
    VkRenderPass renderPass;

    VkPipelineLayout pipelineLayout;

    VkPipeline graphicsPipeline;

    std::vector<VkFramebuffer> swapChainFramebuffers;



    // bool initialize(android_app *app);
     void destroy();






};


#endif //VULKAN_KVULKAN_H
