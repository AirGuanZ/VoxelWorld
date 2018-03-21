/*================================================================
Filename: RenderQueue.h
Date: 2018.1.16
Created by AirGuanZ
================================================================*/
#pragma once

#include <iostream>
#include <vector>

#include <World\Chunk\Model.h>

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
            model->Draw();
        models_.clear();
    }

private:
    std::vector<const Model*> models_;
};
