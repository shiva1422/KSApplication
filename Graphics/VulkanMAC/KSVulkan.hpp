//
//  KSVulkan.hpp
//  Vulkan
//
//  Created by shiva.pandiri on 11/5/22.
//

#ifndef KSVulkan_hpp
#define KSVulkan_hpp

#include <stdio.h>
#include "vulkan/vulkan.h"
#include "vector"
#include "KSWindow.h"

/*
 TODO : Check Vulkan Supported
 I do a few things to determine whether the Vulkan backend of my graphics lib is supported:

 Try to load the Vulkan shared library -- if I can't even do that, then it's not supported.

 Create an instance and enumerate the physical devices. If there's not at least one physical device, then it's not supported. (Your #1)

 Enumerate the available instance extensions (vkEnumerateInstanceExtensionProperties), and check that the minimum extensions are supported (e.g. VK_KHR_surface and the platform's surface extension.) This is your #2. IMO it's probably overkill to do all that work (which is platform-specific since it involves creating a window) on top of checking that the extensions are supported.
 */



//TO store all indices of queuefamilies we nee
class QueueFamilyIndices{

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
    
    bool configure();//TODO later use config properties common to graphics folder.
    
    void release();

    void drawFrame();

private:

    //In Order

    bool createInstance();

    //TODO
    bool createSurface(void *window);
    //TODO Check previous android code
    bool findGFXFamilyQueues(VkPhysicalDevice device);

    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    bool isDeviceSuitable(VkPhysicalDevice device);

    bool selectPhysicalDevice();

    bool createLogicalDevice();

    bool createSwapChain();

    bool createImageViews();

    bool  createRenderPass();

    bool createGraphicsPipeline();

    bool createFrameBuffers();

    bool createCommandPool();

    bool createCommandBuffer();

    bool createSyncObjects();

    bool recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    bool checkValidationSupport();
    
    void setupDebugMessenger();


    SwapChainInfo getSwapChainInfo(VkPhysicalDevice device);

    VkSurfaceFormatKHR surfaceChooseSwapFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) ;



    VkSurfaceKHR& getVKSurface(){return vkSurface;}
     friend class KSVulkanApplication;

    void setWindowInterface(KSWindowVKInterface *interface)
    {
        this->vkSurfaceCreator = interface;
    }


private:


    
     bool bInit = false;
    
   /* A Vulkan Instance is an object that gathers the state of an application. It encloses information such as an application name, name and version of an engine used to create an application, or enabled instance-level extensions and layers.
    */

     VkInstance vkInstance;
     VkPhysicalDevice vkGpu = VK_NULL_HANDLE;
     VkDevice vkDevice = VK_NULL_HANDLE;
     VkQueue graphicsQueue;
     VkQueue presentQueue;
     QueueFamilyIndices indices;



     //TODO createLogicalDevice
     VkPhysicalDeviceFeatures deviceFeatures{};


     //The window surface needs to be created right after the instance creation, because it can actually influence the physical device selection
     VkSurfaceKHR vkSurface;

     VkSwapchainKHR vkSwapChain;

     std::vector<VkImage> swapChainImages;

    std::vector<VkImageView> swapChainImageViews;

    VkFormat swapChainImageFormat;

    VkExtent2D swapChainExtent;

    VkRenderPass renderPass;

    VkPipelineLayout pipelineLayout;

    VkPipeline graphicsPipeline;

    std::vector<VkFramebuffer> swapChainFramebuffers;

    //commands pools manage mem to store command buffers
    VkCommandPool commandPool;

    VkCommandBuffer commandBuffer;


    /*
    * semaphores are used to specify the execution order of operations on the GPU while fences are used to keep the CPU and GPU in sync with each-other.
    */
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;






    //debug
    VkDebugUtilsMessengerEXT debugMessenger;


    //
    KSWindowVKInterface *vkSurfaceCreator  = nullptr;
    
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif


    VkShaderModule createShaderModule(const std::vector<char> &code);
};


#endif /* KSVulkan_hpp */


