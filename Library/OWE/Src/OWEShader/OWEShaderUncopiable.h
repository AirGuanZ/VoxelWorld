/*================================================================
Filename: OWEShaderUncopiable.h
Date: 2017.11.24
Created by AirGuanZ
================================================================*/
#ifndef OWESHADER_UNCOPIABLE_H
#define OWESHADER_UNCOPIABLE_H

namespace OWEShaderAux
{
    class Uncopiable
    {
    public:
        Uncopiable(void) = default;
        Uncopiable(const Uncopiable &) = delete;
        Uncopiable operator=(const Uncopiable &) = delete;
    };
}

#endif //OWESHADER_UNCOPIABLE_H
