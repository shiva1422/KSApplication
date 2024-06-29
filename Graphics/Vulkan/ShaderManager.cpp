
#include <vulkan/vulkan_core.h>
#include <assert.h>
#include "ShaderManager.h"
#include "KSIO/AssetManager.h"
#include <shaderc/shaderc.hpp>
#include <shaderc/status.h>

VkResult ShaderManager::loadShader(const char *filePath, VkShaderModule *shaderOut, ShaderType type, VkDevice device)
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
    stream->close();//TODO check destrutor

    VkShaderModuleCreateInfo shaderModuleCreateInfo{
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .codeSize = length,
            .pCode = (const uint32_t*)fileContent,
    };

    VkResult result = vkCreateShaderModule(
            device, &shaderModuleCreateInfo, nullptr, shaderOut);
    KSLOGD("Load Shader :", "read content %s", fileContent);


    assert(result == VK_SUCCESS);

    delete[] fileContent;

    return result;
}

/*VkResult
ShaderManager::loadShaderC(const char *filePath, VkShaderModule *shaderModule, ShaderType type,
                           KSVkDevice *device) {

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

    shaderc_shader_kind kind = (type == VERTEX_SHADER) ? shaderc_vertex_shader : shaderc_fragment_shader;

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

    VkResult res = vkCreateShaderModule(device->vkDevice, &shaderModuleCreateInfo, nullptr, shaderModule);


    assert(res == VK_SUCCESS);

    delete[] fileContent;

    KSLOGD("Shader Complie", " Success");

    return res;
}
*/