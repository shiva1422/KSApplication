<h1>Steps</h1>
<pre>
 Vulkan functions can be divided into three levels, which are global, instance, and device. 
<ol>

<li>Device-level   : functions are used to perform typical operations such as drawing, shader- modules creation, image creation, or data copying. </li> 

<li>Instance-level :functions allow us to create logical devices. Instance-level functions are used mainly for operations on physical devices manipulating with physical devices, checking their properties, abilities and, creating logical devices. There are multiple instance-level functions, with 
    a.vkEnumeratePhysicalDevices()
    b.vkGetPhysicalDeviceProperties()
    c.vkGetPhysicalDeviceFeatures()
    d.vkGetPhysicalDeviceQueueFamilyProperties()
    e.vkCreateDevice(), 
    f.vkGetDeviceProcAddr(), 
    g.vkDestroyInstance() 
    h.vkEnumerateDeviceExtensionProperties etc
</li>

<li>Global-level:To do all above, and to load device and instance-level functions, we need to create an Instance. This operation is performed with global-level functions, which we need to load first.
 In Vulkan, there are only three global-level functions:
 a.vkEnumerateInstanceExtensionProperties() 
 b.vkEnumerateInstanceLayerProperties()
 c.vkCreateInstance().

They are used during Instance creation to check, what instance-level extensions and layers are available and to create the Instance itself.
</li>
</ol>
*All device-level functions have their first parameter of type VkDevice, VkQueue, or VkCommandBuffer.So, if a function doesn't have such a parameter and is not from the global level, it is from an instance level

</pre>

<pre>

To Check Vulkan Supported few things can be done.
 
1.Try to load the Vulkan shared library and check if exists.

2.Create an instance and enumerate the physical devices. If there's not at least one physical device, then it's not supported. 

3.Enumerate the available instance extensions (vkEnumerateInstanceExtensionProperties), and check that the minimum extensions are supported (e.g. VK_KHR_surface and the platform's surface extension.)  probably overkill to do all that work (which is platform-specific since it involves creating a window) on top of checking that the extensions are supported.

</pre>

<h1>Configuration</h1>

<h1>Steps</h1>
<pre>
a. Init KVulkan - Load KVulkan and retrieve vulkan api function pointers.Using Vulkan Wrapper on Android.
b. Create Instance
       *Fill VKApplicationInfo struct(option) with info about the app(may be for driver to optimize our specific application based on info)(OPTIONAL).
       *Tell KVulkan driver which global extension and validation layers we want to use(global means that they apply to entire program and not a specific device).(NOT OPTIONAL).
       *create vulkan instance using required extensions and appinfo.
c.Setup debug messenger ,need step b probably.

d.Create vulkan instance.
     create a surface - to create surface , surface extension is needed

e.Select Physical device(GPU):
     a.enumerate device ,check suitable device by checking the capabilites.On android every gpu is equal graphics/compute/preset,
     b.find graphics family Queues:
       *  almost every operation in Vulkan, anything from drawing to uploading textures, requires commands to be submitted to a queue.
       *  For a device there are different types of queues that originate from different queue families and each family of queues allows only a subset of commands.
       *  For example, there could be a queue family that only allows processing of compute commands ,graphics or one that only allows memory transfer related commands.
     c.check if the device supports required extensions like swapChainExtension for drawing.
          
f.Create a logical device from the selected device .Multiple logical devices can be created from same physical device.
    a.specify properties and extension required for device creations.
    b.queues will be created and destroyed with creation of logical devices.So no need explicity creation and clean up.
    c.get handles to the queues.

g.Create Swap Chain : basically creating  Swap chain from the logical devices and acquire swap chain images.Swap chain images are basically images used for presentation
    a.choose swap format ,rgba888 etc.
    b.choose present mode - FIFO,Concurrent etc.
    c.choose swapchain extent - resolution ,normally equal to framebuffer size.
    d.create swapchain from logical device.
    e.aquire images from swap chain.

h.Create Swap Chain ImageViews: view type can be 1d,2d,3d,cubemaps
    a.use any VkImage, including those in the swap chain, in the render pipeline we have to create a VkImageView object
    b.check working on a stereographic 3D application swapchain 3d images each image can have multiple imageViews for r,l eyes

i.Create RenderPass:Renderpass is the description of render process like what happens before and after rendering,where the redering goes(color attachments) etc.
    a.Specify color attachment with load and store operation etc
    b.create renderpass and also subpass dependencies can be added.

j.Create Graphics Pipeline:Create different states in pipeline along with shaders for each stage along with data.
    a.Create shader modules.
    b.create vertex and fragement create info stages with shader.
    c.Create PipelineVertexInputStateCreateInfo with vertex data
    d.InputAssemblyStateCreateInfo
    e.view port and scissor info , can be set as static part of pipeline or dynamic state in command buffer
    f.ipelineViewportStateCreateInfo
    g.rasterizer : * The rasterizer takes the geometry that is shaped by the vertices from the vertex shader and turns it into fragments to be colored by the fragment shader.  It also performs depth testing, face culling and the scissor test,
    h.Multisampling.
    i.Color blending
    j.dynamic states.
    k.create graphics pipeline from the logical device with all the above info VkGraphicsPipelineCreateInfo
    
k.Create Framebuffers : VkFramebuffer  
    a. The attachments specified during render pass creation are bound by wrapping them into a VkFramebuffer object. A framebuffer object references all of the VkImageView objects that represent the attachments
     That means framebuffers should be created for each image in swapchain and use one that corresponds to a retrived image from swap chain during drawing.

L.

TODO:
a.check working on a stereographic 3D application swapchain 3d images each image can have multiple imageViews for right and left eyes
</pre>