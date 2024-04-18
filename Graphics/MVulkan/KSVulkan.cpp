//
// Created by pspr1 on 4/25/2021.
//


#include "vector"
#include "KSVulkan.h"
#include "Commons.h"
#include <Logger/KSLog.h>
#include <string>
#include <set>

#define LOGTAG "KSVulkan"

//TODO move into device class later when device is abstracted
const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
};
bool KSVulkan::createInstance()
{

    VkApplicationInfo  appInfo={.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                                .pNext=nullptr,
                                .pApplicationName="Kalasoft",
                                .applicationVersion=VK_MAKE_VERSION(1,0,0),
                                .pEngineName="KSVulkanEngine",
                                .engineVersion=VK_MAKE_VERSION(1,0,0),
                                .apiVersion=VK_MAKE_VERSION(1,1,0),
                                };//TODO versions later

    //check and print extensions
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr , &extensionCount , nullptr);//check res
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr , &extensionCount ,extensions.data());//check res;

    KSLOGI(LOGTAG ,"available instance extensions");
    for(const auto& extension :extensions)
    {
        KSLOGI(LOGTAG,"extension - %s",extension.extensionName);
    }

    std::vector<const char*> instanceExt;
    instanceExt.push_back("VK_KHR_surface");
    instanceExt.push_back("VK_KHR_android_surface");

   //create vulkan Instance
    VkInstanceCreateInfo instanceCreateInfo{.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                            .pNext = nullptr,
                                            .pApplicationInfo = &appInfo,
                                            .enabledLayerCount = 0,//TODO enable global validation layers
                                            .ppEnabledLayerNames=nullptr,
                                            //TODO platform specific
                                            .enabledExtensionCount=static_cast<uint32_t>(instanceExt.size()),
                                            .ppEnabledExtensionNames = instanceExt.data(),
                                            };

    /*TODO ***
    //validation layers
    if (enableValidationLayers)
    {
        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        instanceCreateInfo.enabledLayerCount = 0;
    }

    //HDiff debugMessenger here
    */

    VkResult res ;
    if((res = vkCreateInstance(&instanceCreateInfo,nullptr,&vkInstance) ) != VK_SUCCESS)
    {
        KSLOGE(LOGTAG,"instance Creation failed - err -  %d",res);
        return false;
    }
    return true;
}

bool KSVulkan::createSurface()
{
    int res;
    KSLOGD(LOGTAG,"creating window");
    VkAndroidSurfaceCreateInfoKHR surfaceCreatInfo{.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,
                                                    .pNext = nullptr,
                                                    .flags = 0,
                                                    .window = this->window->get(),
    };
    if((res = vkCreateAndroidSurfaceKHR(vkInstance,&surfaceCreatInfo,nullptr,&vkSurface) ) != VK_SUCCESS)
    {
        KSLOGE(LOGTAG,"Surface Creation failed -err - %d",res);
        return false;
    }
    KSLOGD(LOGTAG,"window created");
    return true;
}

bool KSVulkan::selectPhysicalDevice()
{
    //Multiple gpu devices can be selected and used and device will be destroyed with instance so no need to cleanup

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);
    if (deviceCount == 0)
    {
        return false;
    }
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data());
    KSLOGD(LOGTAG,"physical device Count %u",deviceCount);
   //on android all are same so selecting first one would be enough
    for (const auto& device : devices)
    {
        if (isDeviceSuitable(device))
        {
            vkGpu = device;
            break;
        }
    }

    return vkGpu != VK_NULL_HANDLE;
}

bool KSVulkan::isDeviceSuitable(VkPhysicalDevice device)
{
    //To evaluate the suitability of a device some  properties like the name, type //and supported Vulkan version can be queried using vkGetPhysicalDeviceProperties.
    //also check if device supports drawing and presentation queues.
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;

    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    //The following capabilities are checked in get SwapChanInfo.So below can be removed only for debug purposes.
   /* KSLOGD(LOGTAG,"Physcial device name : %s",deviceProperties.deviceName);
    KSLOGD(LOGTAG,"Physical device info : apiVersion: %x \n\t driverVersion : %x",deviceProperties.apiVersion,deviceProperties.driverVersion);
    KSLOGD(LOGTAG,"API version supported: %d.%d.%d",VK_VERSION_MAJOR(deviceProperties.apiVersion),VK_VERSION_MINOR(deviceProperties.apiVersion),VK_VERSION_PATCH(deviceProperties.apiVersion));
    VkSurfaceCapabilitiesKHR surfaceCapabilities;//checkOther versions
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkGpu,vkSurface,&surfaceCapabilities);
    KSLOGD(LOGTAG,"Surface CVapabilities:.....");
    KSLOGD(LOGTAG,"\timage count: %u - %u", surfaceCapabilities.minImageCount, surfaceCapabilities.maxImageCount);
    KSLOGD(LOGTAG,"\tarray layers: %u", surfaceCapabilities.maxImageArrayLayers);
    KSLOGD(LOGTAG,"\timage size (now): %dx%d", surfaceCapabilities.currentExtent.width, surfaceCapabilities.currentExtent.height);//now?
    KSLOGD(LOGTAG,"\timage size (extent): %dx%d -%dx%d", surfaceCapabilities.minImageExtent.width, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.width, surfaceCapabilities.maxImageExtent.height);
    KSLOGD(LOGTAG,"\tusage: %x", surfaceCapabilities.supportedUsageFlags);
    KSLOGD(LOGTAG,"\tcurrent transform : %u", surfaceCapabilities.currentTransform);
    KSLOGD(LOGTAG,"\tallowed tranform : %x", surfaceCapabilities.supportedTransforms);
    KSLOGD(LOGTAG,"\tcomposite alpha flags: %u", surfaceCapabilities.supportedCompositeAlpha);//currentTransform in doc;
*/
    /*TODO
     *  return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
           deviceFeatures.geometryShader;/

         later score and pick the gpu with best possible score,dedicated or integrated based on the requirements.
     */

    bool res = findGFXFamilyQueues(device) && checkDeviceExtensionSupport(device);

    //only if swapChainExtension supported
    if(res)
    {
        SwapChainInfo info = getSwapChainInfo(device);
        res = !info.formats.empty() && !info.presentModes.empty();
    }


    KSLOGD(LOGTAG,"device Suitable %d",res);
    return res;
    ;
    //The support for optional features like texture compression, 64 bit floats and multi viewport rendering (useful for VR) can be queried using vkGetPhysicalDeviceFeatures:

    return true;//can check for various features and select based on the requirement;//just selecting first gpu now
}

bool KSVulkan::findGFXFamilyQueues(VkPhysicalDevice device)
{
    //For now only looking for a queue that supports graphics commands

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    /*
     * The VkQueueFamilyProperties struct contains some details about the queue family, including the type of operations that are supported and the number of queues that
     * can be created based on that family. now find at least one queue family that supports VK_QUEUE_GRAPHICS_BIT.
     */

    int i = 0;
    bool bGraphicsFam = false;
    bool bPresentFam = false;//presentation family

    for (const auto& queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
            // return true;
            bGraphicsFam = true;
        }

        // queue family that has the capability of presenting to our window surface.//Surface should be created here
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, vkSurface, &presentSupport);
        if(presentSupport)
        {
            bPresentFam = true;
            indices.presentFamily = i;
        }

        i++;

        /*Note that it's very likely that these end up being the same queue family after all, but throughout the program they are treated as if they were separate queues for a uniform approach
          Nevertheless, logic could be added explicitly to prefer a physical device that supports drawing and presentation in the same queue for improved performance.
         */
        if(bGraphicsFam && bPresentFam)
            break;
    }

    KSLOGD(LOGTAG,"graphicFam index %u presentFam index %u",indices.graphicsFamily,indices.presentFamily);
    return  bGraphicsFam && bPresentFam;//all required families are found
}

bool KSVulkan::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
    //TODO optimize
    for (const auto& extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    assert(requiredExtensions.empty());

    return requiredExtensions.empty();
}

SwapChainInfo KSVulkan::getSwapChainInfo(VkPhysicalDevice device)
{
    SwapChainInfo swapChainInfo;

    //check below func variants
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device,vkSurface,&swapChainInfo.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, vkSurface, &formatCount, nullptr);

    if (formatCount != 0)
    {
        swapChainInfo.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, vkSurface, &formatCount, swapChainInfo.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, vkSurface, &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        swapChainInfo.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, vkSurface, &presentModeCount, swapChainInfo.presentModes.data());
    }

    KSLOGD(LOGTAG,"Surface %u presentMode %u formats",presentModeCount,formatCount);
    assert(presentModeCount != 0 || formatCount != 0);
    return swapChainInfo;
}

bool KSVulkan::createLogicalDevice()
{
    //multiple logical devices can be created from same physical device.

    /*
     * The creation of a logical device involves specifying a bunch of details in structs again, of which the first one will be VkDeviceQueueCreateInfo.
     * This structure describes the number of queues required from a single family queue. right now interested only in graphic queue capabilites
     */

    assert(findGFXFamilyQueues(vkGpu));//multiple times
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    //below values should be valid and indited properly before this in findQueuefams
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily, indices.presentFamily};//TODO validity

    //    Vulkan lets to assign priorities to queues to influence the scheduling of command buffer execution
    //    If the queue families are the same, then  only need to pass its index once. Finally, add a call to retrieve the queue handle:
    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        KSLOGD("KSVulkan::createLogicalDevice","%u quefamilyIndex",queueFamily);
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }


    VkPhysicalDeviceFeatures deviceFeats{};//TODO should be queried?

    //Logical Device Creation
    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

    deviceCreateInfo.pEnabledFeatures = &deviceFeats;//TODO before creating , HDIFF

    //device extensions
    //deviceCreateInfo.enabledExtensionCount = 1;
    //const char* deviceExtension = "VK_KHR_portability_subset";//mac err



    //TODO validation check may not be required
    /*if (enableValidationLayers) {
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        deviceCreateInfo.enabledLayerCount = 0;
    }*/

    deviceCreateInfo.enabledLayerCount = 0;//todo remove after uncommenting the above

    // Using a swapchain requires enabling the VK_KHR_swapchain extension
    std::vector<const char*>  deviceExt;//fromCreateInstace
   // deviceExt.push_back("VK_KHR_swapchain");//already in the device extensions below.
    //deviceExt.push_back("VK_KHR_portability_subset");mac
    for(auto ext : deviceExtensions)
    {
        deviceExt.push_back(ext);
    }
    //deviceExt.push_back(deviceExtensions.)
    deviceCreateInfo.enabledExtensionCount = deviceExt.size();
    deviceCreateInfo.ppEnabledExtensionNames = deviceExt.data();

    if (vkCreateDevice(vkGpu, &deviceCreateInfo, nullptr, &vkDevice) != VK_SUCCESS) {
        KSLOGE(LOGTAG,"create logical device failed");//reason print
        return false;
    }

    /*
     * The queues are automatically created along with the logical device, but  there is no  handle to interface
     * with them yet.Device queues are implicitly cleaned up when the device is destroyed
     */

    // vkGetDeviceQueue function  can be used to retrieve queue handles for each queue family
    //TODO indices.graphicsFamily && presentFam should be inited correctly ,default intialize might have value that might be valid but not correct
    vkGetDeviceQueue(vkDevice, indices.graphicsFamily, 0, &graphicsQueue);
    vkGetDeviceQueue(vkDevice,indices.presentFamily,0,&presentQueue);

    //Now Graphics card can be used to do amazing stuff but not quite enough :)

    return true;
}

bool KSVulkan::createSwapChain()
{
    SwapChainInfo swapChainSupport = getSwapChainInfo(vkGpu);

    VkSurfaceFormatKHR surfaceFormat = surfaceChooseSwapFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;//atlest more than one minImage count
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }


    VkSwapchainCreateInfoKHR swapChainCreateInfo{};
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface = vkSurface;

    swapChainCreateInfo.minImageCount = imageCount;
    swapChainCreateInfo.imageFormat = surfaceFormat.format;
    swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapChainCreateInfo.imageExtent = extent;
    swapChainCreateInfo.imageArrayLayers = 1;
    swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // specify how to handle swap chain images that will be used across multiple queue families
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily,indices.presentFamily};
    if(indices.graphicsFamily != indices.presentFamily)//Should be set properly findingqueuefamilies
    {
        // An image is owned by one queue family at a time and ownership must be
        // explicitly transferred before using it in another queue family. This option offers the best performance.
        //concurrent mode requires you to specify at least two distinct queue families.
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapChainCreateInfo.queueFamilyIndexCount = 2;
        swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        //Images can be used across multiple queue families without explicit ownership transfers.
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        //swapChainCreateInfo.queueFamilyIndexCount = 0; // Optional HDIFF
        // swapChainCreateInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    //any tranform like 90 degrees
    swapChainCreateInfo.preTransform = swapChainSupport.capabilities.currentTransform;

    //The compositeAlpha field specifies if the alpha channel should be used for blending with other windows in the window system
    //ignoring here
    swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    swapChainCreateInfo.presentMode = presentMode;
    swapChainCreateInfo.clipped = VK_TRUE;//clipping enabled

    //swapChainCreateInfo.flags = 0;//TODO

    //when window resized or other swapchain needs recreated specifying old one for now null
    swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(vkDevice, &swapChainCreateInfo, nullptr, &vkSwapChain) != VK_SUCCESS) {
        KSLOGE(LOGTAG,"CreateSwapChain","failed");
        return false;
    }


    //retreive swapchain images

    vkGetSwapchainImagesKHR(vkDevice, vkSwapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(vkDevice, vkSwapChain, &imageCount, swapChainImages.data());

    KSLOGD(LOGTAG,"SwapChain Create Success","imageCount %u",imageCount);
    //validate if the created and selected or same later
    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
    return true;
}

VkSurfaceFormatKHR KSVulkan::surfaceChooseSwapFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
{
    //select required format if not available use first but check based on required later
    for(auto &fmt : availableFormats)
    {
        if(fmt.format == VK_FORMAT_B8G8R8A8_SRGB && fmt.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return fmt;
    }

    return availableFormats[0];
}

VkPresentModeKHR KSVulkan::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
{
    // return VK_PRESENT_MODE_MAILBOX_KHR;//TODO this is better?
    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D KSVulkan::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities)
{
    //The swap extent is the resolution of the swap chain images and it's almost always exactly equal to the resolution of the window that we're drawing to in pixels (

    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        KSLOGD(LOGTAG,"frameBuffer -SC size w-%u h-%u",capabilities.currentExtent.width,capabilities.currentExtent.height);
        return capabilities.currentExtent;
    }
    else
    {
        int width = window->getWidth(),height = window->getHeight();//TODO better get from vulkan surface than window
        KSLOGD(LOGTAG,"frameBuffer size w-%u h-%u",width,height);
        VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
        };

        //clamp
        if(actualExtent.width < capabilities.minImageExtent.width)
            actualExtent.width = capabilities.minImageExtent.width;
        if(actualExtent.width > capabilities.maxImageExtent.width)
            actualExtent.width = capabilities.maxImageExtent.width;

        if(actualExtent.height < capabilities.minImageExtent.height)
            actualExtent.height = capabilities.minImageExtent.height;
        if(actualExtent.height > capabilities.maxImageExtent.height)
            actualExtent.height = capabilities.maxImageExtent.height;

        return actualExtent;
    }
}

bool KSVulkan::createImageViews()
{
    //To use any VkImage, including those in the swap chain, in the render pipeline we have to create a VkImageView object
    swapChainImageViews.resize(swapChainImages.size());

    for(int i = 0;i< swapChainImages.size() ; ++i)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages[i];

        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;//1d,2d,3d,cubemaps
        createInfo.format = swapChainImageFormat;

        //no rgba swizzle like gbra
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        //The subresourceRange field describes what the image's purpose is and which part of the image should be accessed. Our images will be used as color targets without any mipmapping levels or multiple layers
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        createInfo.pNext = nullptr;
        //TODO createInfo.flags =0
        // createInfo.flags = 0;

        if( vkCreateImageView(vkDevice,&createInfo, nullptr,&swapChainImageViews[i]) != VK_SUCCESS)
            return false;
    }
    return true;
}

bool KSVulkan::createRenderPass()
{
    //Attachment description;//only one from swapchain images
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;//no multisampling yet so

    /*
     * The loadOp and storeOp determine what to do with the data in the attachment before rendering and after rendering. We have the following choices for loadOp:

    VK_ATTACHMENT_LOAD_OP_LOAD: Preserve the existing contents of the attachment
    VK_ATTACHMENT_LOAD_OP_CLEAR: Clear the values to a constant at the start
    VK_ATTACHMENT_LOAD_OP_DONT_CARE: Existing contents are undefined; we don't care about them
    There are only two possibilities for the storeOp:
    VK_ATTACHMENT_STORE_OP_STORE: Rendered contents will be stored in memory and can be read later
    VK_ATTACHMENT_STORE_OP_DONT_CARE: Contents of the framebuffer will be undefined after the rendering operation
     */

    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    /*
     * Textures and framebuffers in Vulkan are represented by VkImage objects with a certain pixel format, however the layout of the pixels in memory can change based on what you're trying to do with an image.
        Some of the most common layouts are:
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: Images used as color attachment
    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR: Images to be presented in the swap chain
    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: Images to be used as destination for a memory copy operation
     */
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


    //Subpasses and attachment references
    /*
     * A single render pass can consist of multiple subpasses. Subpasses are subsequent rendering operations that depend
     * on the contents of framebuffers in previous passes, for example a sequence of post-processing effects that are applied
     * one after another. If you group these rendering operations into one render pass, then Vulkan is able to reorder the
     * operations and conserve memory bandwidth for possibly better performance. For our very first triangle, however,
     * we'll stick to a single subpass.
     *
     * Every subpass references one or more of the attachments
     */

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    /*
     * The index of the attachment in this array is directly referenced from the fragment shader
     * with the layout(location = 0) out vec4 outColor
     * The following other types of attachments can be referenced by a subpass:
    pInputAttachments: Attachments that are read from a shader
    pResolveAttachments: Attachments used for multisampling color attachments
    pDepthStencilAttachment: Attachment for depth and stencil data
    pPreserveAttachments: Attachments that are not used by this subpass, but for which the data must be preserved
     */

    //With the above info renderpass can be created

    VkRenderPassCreateInfo renderPassInfo{};

    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    //Rendering and presentation subpass dependency
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;//refers to the implicit subpass before or after the render pass depending on whether it is specified in srcSubpass or dstSubpass
    dependency.dstSubpass = 0;//The index 0 refers to our subpass, which is the first and only one,dst shoud alway be higher than src unces it is EXTERNAL in aboveline

    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;

    //wait on swapchain to finish  read from image
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;


    if (vkCreateRenderPass(vkDevice, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
    {
        KSLOGE(LOGTAG,"failed to create render pass!");
        return false;
    }
    return true;
}

bool KSVulkan::createGraphicsPipeline()
{
    std::vector<char> vertShader,fragShader;
    //TODO
   // assert(ShaderManager::loadShader("trianglefrag.spv",fragShader) && ShaderManager::loadShader("trianglevert.spv",vertShader));


    //The compilation and linking of the SPIR-V bytecode to machine code for execution by the GPU doesn't happen until the graphics pipeline is created
    //TODO
    VkShaderModule vertShaderModule ;//= createShaderModule(vertShader);
    VkShaderModule fragShaderModule ;//= createShaderModule(fragShader);

    //Shader Stage Creation
    //To actually use the shaders we'll need to assign them to a specific pipeline stage through VkPipelineShaderStageCreateInfo

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";
    //vertShaderStageInfo.pSpecializationInfo

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    //vertex Input
    //since for now vertex data is directly in shader just setting to nullptr;
    /*
     * Bindings: spacing between data and whether the data is per-vertex or per-instance (see instancing)
     * Attribute descriptions: type of the attributes passed to the vertex shader, which binding to load them from and at which offset
     */
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    //vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    // vertexInputInfo.pVertexAttributeDescriptions = nullptr;


    //Input Assembly
    /*
     * The VkPipelineInputAssemblyStateCreateInfo struct describes two things: what kind of geometry will be drawn from the vertices and if primitive restart should be enabled.
     * The former is specified in the topology member and can have values like:
     * VK_PRIMITIVE_TOPOLOGY_POINT_LIST: points from vertices
        VK_PRIMITIVE_TOPOLOGY_LINE_LIST: line from every 2 vertices without reuse
        VK_PRIMITIVE_TOPOLOGY_LINE_STRIP: the end vertex of every line is used as start vertex for the next line
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST: triangle from every 3 vertices without reuse
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP: the second and third vertex of every triangle are used as first two vertices of the next triangle
     */

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;



    //ViewPort and Scissors WTF? sciss can be set as static part of pipeline or dynamic state in command buffer
    VkViewport viewport{};//FrameBuffer size which will be swapChain Images
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) swapChainExtent.width;//Pixels
    viewport.height = (float) swapChainExtent.height;
    viewport.minDepth = 0.0f;//Exper later
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapChainExtent;

    //fixed pipeline
    /*
    * While most of the pipeline state needs to be baked into the pipeline state, a limited amount of the state can actually
    * be changed without recreating the pipeline at draw time.
    * Examples are the size of the viewport, line width and blend constants.
    * If you want to use dynamic state and keep these properties out,
    * then you'll have to fill in a VkPipelineDynamicStateCreateInfo structure like this:
    */





    //The actual viewport(s) and scissor rectangle(s) will then later be set up at drawing time.
    // multiple viewports can be enable but requires enabling a GPU feature (see logical device creation).
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    // viewportState.pViewports = &viewport;//TODO as dynamice HDIFF
    viewportState.scissorCount = 1;
    // viewportState.pScissors = &scissor;



    //Rasterizer
    /*
     * The rasterizer takes the geometry that is shaped by the vertices from the vertex shader and turns it into fragments to be colored by the fragment shader.
     * It also performs depth testing, face culling and the scissor test,
     * and it can be configured to output fragments that fill entire polygons or just the edges
     * (wireframe rendering). All this is configured using the VkPipelineRasterizationStateCreateInfo structure.
     */

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;//fill,line,point
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    //rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    //rasterizer.depthBiasClamp = 0.0f; // Optional
    //rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

    /*
     * If depthClampEnable is set to VK_TRUE, then fragments that are beyond the near and far planes are clamped to them as opposed to discarding them.
     * This is useful in some special cases like shadow maps. Using this requires enabling a GPU feature.
     * If rasterizerDiscardEnable is set to VK_TRUE, then geometry never passes through the rasterizer stage.
     * This basically disables any output to the framebuffe
     */

    //MultiSampling://TODO later for now disabled
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    //multisampling.minSampleShading = 1.0f; // Optional
    //multisampling.pSampleMask = nullptr; // Optional
    //multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    //multisampling.alphaToOneEnable = VK_FALSE; // Optional

    //Depth and stencil testing TODO


    //Color blending
    /*
     * There are two types of structs to configure color blending. The first struct, VkPipelineColorBlendAttachmentState contains the configuration per attached framebuffer and the second struct, VkPipelineColorBlendStateCreateInfo contains the global color blending settings.
     * In our case we only have one framebuffer:
     */

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;//TODO HDIFF use below for VK_TRUE
    /* colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
     colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
     colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
     colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
     colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
     colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;*/

    /*
     * The second structure references the array of structures for all of the framebuffers and allows you
     * to set blend constants that you can use as blend factors in the aforementioned calculations.
     */
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 1.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 1.0f; // Optiona


    /*
   * This will cause the configuration of these values to be ignored and you will be able (and required)
   * to specify the data at drawing time
   */


    std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();


    // The uniform values need to be specified during pipeline creation by creating a VkPipelineLayout object and can be modified
    //without pipeline recreation .The structure also specifies push constants, which are another way of passing dynamic values to shaders
    //Pipeline Layout

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0; // Optional
    pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    if (vkCreatePipelineLayout(vkDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        KSLOGE(LOGTAG,"createGraphics Pipeline Layout failed");
        return false;
    }


    /*//createRenderPass
     * Before we can finish creating the pipeline, we need to tell Vulkan about the framebuffer attachments
     * that will be used while rendering. We need to specify how many color and depth buffers there will be,
     * how many samples to use for each of them and how their contents should be handled throughout the rendering operations.
     * All of this information is wrapped in a render pass object,
     */


    /*
     * With all the info pipelinecreate
     * Shader stages: the shader modules that define the functionality of the programmable stages of the graphics pipeline
    Fixed-function state: all of the structures that define the fixed-function stages of the pipeline, like input assembly, rasterizer, viewport and color blending
    Pipeline layout: the uniform and push values referenced by the shader that can be updated at draw time
    Render pass: the attachments referenced by the pipeline stages and their usage
     */


    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO ;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;

    //structures describing the fixed-function stage.
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr; // Optional
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;

    //Pipeline layout a vulkan handle rather than struct pointer
    pipelineInfo.layout = pipelineLayout;

    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;

    //Vulkan allows you to create a new graphics pipeline by deriving from an existing pipeline.
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    // pipelineInfo.basePipelineIndex = -1; //

    if (vkCreateGraphicsPipelines(vkDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
    {
        KSLOGE(LOGTAG,"failed to create graphics pipeline!");
        return false;
    }
    /*
     * The second parameter, for which we've passed the VK_NULL_HANDLE argument, references an optional VkPipelineCache
     * object. A pipeline cache can be used to store and reuse data relevant to pipeline creation across multiple calls to
     * vkCreateGraphicsPipelines and even across program executions if the cache is stored to a file.
     * This makes it possible to significantly speed up pipeline creation at a later time.
     */

    //TODO
    vkDestroyShaderModule(vkDevice, fragShaderModule, nullptr);
    vkDestroyShaderModule(vkDevice, vertShaderModule, nullptr);

    return true;
}

bool KSVulkan::createFrameBuffers()
{
    /*
 * The attachments specified during render pass creation are bound by wrapping them into a VkFramebuffer object. A framebuffer object references all of the VkImageView objects that represent the attachments
 * That means framebuffers should be created for each image in swapchain and use one that corresponds to a retrived image from swap chain during drawing
 */
    swapChainFramebuffers.resize(swapChainImageViews.size());

    for(int i =0; i <swapChainImageViews.size();++i)
    {
        VkImageView attachments[] = {swapChainImageViews[i]};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;//with which renderpass framebuffer will be used
        framebufferInfo.attachmentCount = 1;//just like gl drawBuffers
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(vkDevice, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS)
        {
            KSLOGE(LOGTAG,"createFrameBuffer failed");
            return false;
        }
    }

    return true;
}

void KSVulkan::destroy()
{
    vkDestroySurfaceKHR(vkInstance,vkSurface, nullptr);
    vkDestroyDevice(vkDevice, nullptr);
    vkDestroyInstance(vkInstance, nullptr);
}

KSVulkan *KSVulkan::newInstance(KSWindow *window){

    //a
    if(!InitVulkan())
    {
        KSLOGE(LOGTAG,"Vulkan Initialize failed,Vulkan might not be available");
        return nullptr;
    }

    KSVulkan *vulkan = new KSVulkan();
    vulkan->window = window;

    //TODO combile all below to vulkan->configure later.
    /*b
     * //DebugOnly
    if(enableValidationLayers && !checkValidationSupport())
        return false;

     */

    //c TODO

    //d
    bool res = vulkan->createInstance();
    //TODO setup debug messenger
    res = res && vulkan->createSurface();

    //e
    res = res && vulkan->selectPhysicalDevice();

    KSLOGI(LOGTAG,"selected physical device");
    //f
    res = res && vulkan->createLogicalDevice();

    //g
    res = res && vulkan->createSwapChain();

    //h
    res = res && vulkan->createImageViews();

    //i

    assert(res);
    return vulkan;
}

