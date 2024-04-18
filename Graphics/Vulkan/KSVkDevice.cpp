//
// Created by shivaaz on 11/2/23.
//

#include <android/native_window.h>
#include "vulkan_wrapper.h"
#include "KSVkDevice.h"
#include "LayersAndExtensions.h"
#include "KSIO/IKSStream.h"
#include "KSIO/AssetManager.h"
#include <Logger/KSLog.h>
#include <unistd.h>
#include <assert.h>
#include <shaderc/shaderc.h>
#include <shaderc/shaderc.hpp>

// Vulkan call wrapper
#define CALL_VK(func)                                                 \
  if (VK_SUCCESS != (func)) {                                         \
    __android_log_print(ANDROID_LOG_ERROR, "KSVULKAN ",               \
                        "Vulkan error. File[%s], line[%d]", __FILE__, \
                        __LINE__);                                    \
    assert(false);                                                    \
  }
static const char* LOGTAG = "KSVULKANDevice";

bool KSVkDevice::create(ANativeWindow* window, VkApplicationInfo* appInfo)
{


    // Enable validation and debug layer/extensions, together with other necessary
    // extensions
    //TODO validation layer
    //LayerAndExtensions layerUtil;
   /* std::vector<const char *> layers = {"VK_LAYER_KHRONOS_validation"};
    KSLOGD(LOGTAG,"getting into validaation check");
    for (auto layerName : layers)
    {
        sleep(1);
        // check vulkan sees the layers packed inside this app's APK. layers could also be
        // be pushed to Android with adb on command line, this sample does test that approach
        // in the sense: if you want to use the adb way, you want to use it to enable/disable too
        //               if you want to use the source code way, pack the layer into apk
        //    blending different ways might work, feel free to use and experiment if you prefer.
        KSLOGD(LOGTAG,"checking validation support");
        assert(layerUtil.isLayerSupported(layerName));
    }

    for (auto layerName : layers) {
        // check vulkan sees the layers packed inside this app's APK. layers could also be
        // be pushed to Android with adb on command line, this sample does test that approach
        // in the sense: if you want to use the adb way, you want to use it to enable/disable too
        //               if you want to use the source code way, pack the layer into apk
        //    blending different ways might work, feel free to use and experiment if you prefer.
        assert(layerUtil.isLayerSupported(layerName));
    }
    for (auto extName : instanceExt)
    {
        assert(layerUtil.isExtensionSupported(
                extName, VK_NULL_HANDLE, nullptr));
    }
    */

    // Find the supported debug callback extensions and its layers.TODO validation layer
  /*  std::pair<const char*, const char*> dbgExt = layerUtil.getDbgReportExtInfo();
    if(dbgExt.second) {
        instanceExt.push_back(dbgExt.second);
        if (strcmp(dbgExt.first, VULKAN_DRIVER)) {
            bool alreadyIn = false;
            for(auto& name : layers) {
                if (!strcmp(name, dbgExt.first)) {
                    alreadyIn = true;
                    break;
                }
            }
            if(!alreadyIn)
                layers.push_back(dbgExt.first);
        }
    }*/

    std::vector<const char*> instanceExt = {"VK_KHR_surface", "VK_KHR_android_surface"};

    //prepare necessary extensions: Vulkan on Android need these to function,create instance with all enabled layers/extensions.//available on the system
    std::vector<const char *>  deviceExt;
    // instanceExt.push_back("VK_KHR_surface");
    //  instanceExt.push_back("VK_KHR_android_surface");
    deviceExt.push_back("VK_KHR_swapchain");

    // Create the Vulkan instance
    VkInstanceCreateInfo instanceCreateInfo{
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .pApplicationInfo = appInfo,
            .enabledLayerCount = 0,//TODO
            .ppEnabledLayerNames = nullptr,//layers.data(),//TODO validation layer
            .enabledExtensionCount = static_cast<uint32_t>(instanceExt.size()),
            .ppEnabledExtensionNames = instanceExt.data(),
    };

    CALL_VK(vkCreateInstance(&instanceCreateInfo, nullptr, &vkInstance));
    //TODO debugmode and only if validation is enabled
    //layerUtil.hookDbgReportExt(vkInstance);TODO validation layer


    // if we create a surface, we need the surface extension
    VkAndroidSurfaceCreateInfoKHR createInfo{
            .sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .window = window};
    CALL_VK(vkCreateAndroidSurfaceKHR(vkInstance, &createInfo, nullptr,
                                      &vkSurface));
    //TODO select gpu optimal score
    // On Android, every GPU device is equal -- supporting graphics/compute/present
    uint32_t gpuCount = 0;
    CALL_VK(vkEnumeratePhysicalDevices(vkInstance, &gpuCount, nullptr));
    VkPhysicalDevice tmpGpus[gpuCount];
    CALL_VK(vkEnumeratePhysicalDevices(vkInstance, &gpuCount, tmpGpus));
    vkGpu = tmpGpus[0];  // Pick up the first GPU Device

    // check for vulkan info on this GPU device
    VkPhysicalDeviceProperties gpuProperties;
    vkGetPhysicalDeviceProperties(vkGpu, &gpuProperties);
    KSLOGI(LOGTAG,"Vulkan Physical Device Name: %s", gpuProperties.deviceName);
    KSLOGI(LOGTAG,"Vulkan Physical Device Info: apiVersion: %x \n\t driverVersion: %x",
           gpuProperties.apiVersion, gpuProperties.driverVersion);
    KSLOGI(LOGTAG,"API Version Supported: %d.%d.%d",
           VK_VERSION_MAJOR(gpuProperties.apiVersion),
           VK_VERSION_MINOR(gpuProperties.apiVersion),
           VK_VERSION_PATCH(gpuProperties.apiVersion));

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkGpu, vkSurface,
                                              &surfaceCapabilities);

    KSLOGI(LOGTAG,"Vulkan Surface Capabilities:\n");
    KSLOGI(LOGTAG,"\timage count: %u - %u\n", surfaceCapabilities.minImageCount,
           surfaceCapabilities.maxImageCount);
    KSLOGI(LOGTAG,"\tarray layers: %u\n", surfaceCapabilities.maxImageArrayLayers);
    KSLOGI(LOGTAG,"\timage size (now): %dx%d\n", surfaceCapabilities.currentExtent.width,
           surfaceCapabilities.currentExtent.height);
    KSLOGI(LOGTAG,"\timage size (extent): %dx%d - %dx%d\n",
           surfaceCapabilities.minImageExtent.width,
           surfaceCapabilities.minImageExtent.height,
           surfaceCapabilities.maxImageExtent.width,
           surfaceCapabilities.maxImageExtent.height);
    KSLOGI(LOGTAG,"\tusage: %x\n", surfaceCapabilities.supportedUsageFlags);
    KSLOGI(LOGTAG,"\tcurrent transform: %u\n", surfaceCapabilities.currentTransform);
    KSLOGI(LOGTAG,"\tallowed transforms: %x\n", surfaceCapabilities.supportedTransforms);
    KSLOGI(LOGTAG,"\tcomposite alpha flags: %u\n", surfaceCapabilities.currentTransform);

    vkGetPhysicalDeviceMemoryProperties(vkGpu,&gpuMemoryProperties);
    // Find a GFX queue family
    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(vkGpu, &queueFamilyCount, nullptr);
    assert(queueFamilyCount);
    std::vector<VkQueueFamilyProperties>  queueFamilyProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(vkGpu, &queueFamilyCount,
                                             queueFamilyProperties.data());

    uint32_t queueFamilyIndex;
    for (queueFamilyIndex=0; queueFamilyIndex < queueFamilyCount;
         queueFamilyIndex++) {
        if (queueFamilyProperties[queueFamilyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            break;
        }
    }
    assert(queueFamilyIndex < queueFamilyCount);

    // Create a logical device from GPU we picked
    float priorities[] = {
            1.0f,
    };
    VkDeviceQueueCreateInfo queueCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = queueFamilyIndex,
            .queueCount = 1,
            .pQueuePriorities = priorities,
    };

    VkDeviceCreateInfo deviceCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = nullptr,
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &queueCreateInfo,
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
            .enabledExtensionCount = static_cast<uint32_t>(deviceExt.size()),
            .ppEnabledExtensionNames = deviceExt.data(),
            .pEnabledFeatures = nullptr,
    };

    CALL_VK(vkCreateDevice(vkGpu, &deviceCreateInfo, nullptr, &vkDevice));
    vkGetDeviceQueue(vkDevice, queueFamilyIndex, 0, &queue);

    bInitialized = true;

    return bInitialized;
}

VkResult KSVkDevice::loadShader(const char *filePath, VkShaderModule *shaderModule , ShaderType shaderType)
{
    IKSStream *stream = AssetManager::openAsset(filePath);
    if(!stream)
    {
        KSLOGE("Load Shader :", "error openAsset %s", filePath);
        return VK_ERROR_DEVICE_LOST;
    }

    size_t length = stream->getSize();
    char* fileContent = new char[length];
    stream->read(fileContent,length);
    stream->close();

    shaderc_shader_kind kind = (shaderType == VERTEX_SHADER) ? shaderc_vertex_shader : shaderc_fragment_shader;

    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
    // Like -DMY_DEFINE=1
    //options.AddMacroDefinition("MY_DEFINE", "1");
    shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(fileContent, length, kind, filePath, options);

    if(module.GetNumErrors() != 0)
    {
        KSLOGE("Compile SHaderC","error %s",module.GetErrorMessage().c_str());
    }
    std::vector<uint32_t> result(module.cbegin(), module.cend());

    VkShaderModuleCreateInfo shaderModuleCreateInfo{
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .codeSize = result.size() * sizeof(uint32_t),
            .pCode = (const uint32_t*)result.data(),
    };

    VkResult res = vkCreateShaderModule(vkDevice, &shaderModuleCreateInfo, nullptr, shaderModule);

    assert(res == VK_SUCCESS);

    delete[] fileContent;

    KSLOGD("Shader Complie", " Success");

    return res;
}

void KSVkDevice::terminate()
{
    vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
    vkDestroyDevice(vkDevice, nullptr);
    vkDestroyInstance(vkInstance, nullptr);
}

bool KSSwapChain::create(const KSVkDevice &device)
{
    KSLOGD(LOGTAG,"->createSwapChain");
    memset(&swapchain, 0, sizeof(swapchain));

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.vkGpu, device.vkSurface,
                                              &surfaceCapabilities);
    // Query the list of supported surface format and choose one we like
    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device.vkGpu, device.vkSurface,
                                         &formatCount, nullptr);
    VkSurfaceFormatKHR* formats = new VkSurfaceFormatKHR[formatCount];
    vkGetPhysicalDeviceSurfaceFormatsKHR(device.vkGpu, device.vkSurface,
                                         &formatCount, formats);
    KSLOGD(LOGTAG,"Got %d formats", formatCount);

    uint32_t chosenFormat;//
    for (chosenFormat = 0; chosenFormat < formatCount; chosenFormat++) {
        if (formats[chosenFormat].format == VK_FORMAT_R8G8B8A8_UNORM) break;
    }
    assert(chosenFormat < formatCount);

    displaySize = surfaceCapabilities.currentExtent;
    displayFormat= formats[chosenFormat].format;

    // **********************************************************
    // Create a swap chain (here we choose the minimum available number of surface
    // in the chain)//TODO
    VkSwapchainCreateInfoKHR swapchainCreateInfo{
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = nullptr,
            .surface = device.vkSurface,
            .minImageCount = surfaceCapabilities.minImageCount,
            .imageFormat = formats[chosenFormat].format,
            .imageColorSpace = formats[chosenFormat].colorSpace,
            .imageExtent = surfaceCapabilities.currentExtent,
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 1,
            .pQueueFamilyIndices = &device.queueFamilyIndex,
            .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = VK_PRESENT_MODE_FIFO_KHR,
            .clipped = VK_FALSE,
            .oldSwapchain = VK_NULL_HANDLE,
    };
    CALL_VK(vkCreateSwapchainKHR(device.vkDevice, &swapchainCreateInfo, nullptr,
                                 &swapchain));

    //Get the length of the created swap chain
    CALL_VK(vkGetSwapchainImagesKHR(device.vkDevice, swapchain,
                                    &swapchainLength, nullptr));
    delete[] formats;
    KSLOGI(LOGTAG,"createSwapChain");

    return true;
}