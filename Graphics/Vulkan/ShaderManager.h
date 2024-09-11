
#ifndef PHOTOFX_SHADERMANAGER_H
#define PHOTOFX_SHADERMANAGER_H



enum ShaderType { VERTEX_SHADER, FRAGMENT_SHADER };

class ShaderManager{

public:

    static VkResult loadShader(const char* filePath, VkShaderModule* shaderOut, ShaderType type, VkDevice device);
    //static VkResult loadShaderC(const char* filePath, VkShaderModule* shaderModule, ShaderType type, KSVkDevice *device);


};

#endif //PHOTOFX_SHADERMANAGER_H
