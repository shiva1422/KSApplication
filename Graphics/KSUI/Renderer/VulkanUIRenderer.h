//
// Created by shivaaz on 9/10/23.
//

#ifndef PHOTOFX_VULKANUIRENDERER_H
#define PHOTOFX_VULKANUIRENDERER_H

#include "Renderer.h"
class VulkanUIRenderer : public Renderer{

public:

protected:

    virtual bool onRender() override;

    virtual bool init();


};


#endif //PHOTOFX_VULKANUIRENDERER_H
