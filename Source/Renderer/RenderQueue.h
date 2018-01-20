/*================================================================
Filename: RenderQueue.h
Date: 2018.1.16
Created by AirGuanZ
================================================================*/
#ifndef VW_RENDER_QUEUE_H
#define VW_RENDER_QUEUE_H

#include <iostream>
#include <vector>

#include "../Model/Model.h"

class RenderQueue
{
public:
    void AddModel(const Model *model)
    {
        models_.push_back(model);
    }

    void Render(void)
    {
        for(const Model *model : models_)
        {
            model->Bind();
            model->Draw();
            model->Unbind();
        }
        //std::cerr << models_.size() << std::endl;
        models_.clear();
    }

private:
    std::vector<const Model*> models_;
};

#endif //VW_RENDER_QUEUE_H
