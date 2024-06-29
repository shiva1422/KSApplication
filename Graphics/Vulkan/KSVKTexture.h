//
// Created by shivaaz on 4/16/24.
//

#ifndef PHOTOFX_KSVKTEXTURE_H
#define PHOTOFX_KSVKTEXTURE_H

typedef struct KSVKTexture {
    VkSampler sampler{VK_NULL_HANDLE};
    VkImage image{VK_NULL_HANDLE};
    VkImageLayout imageLayout;
    VkDeviceMemory deviceMemory;
    VkImageView view;
    int32_t height;
    int32_t width;
    uint32_t mipLevels{ 0 };
} KSVKTexture;


#endif //PHOTOFX_KSVKTEXTURE_H
