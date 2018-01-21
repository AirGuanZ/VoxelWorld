/*================================================================
Filename: OWEShaderFatalError.h
Date: 2017.11.24
Created by AirGuanZ
================================================================*/
#ifndef OWESHADER_FATAL_ERROR_H
#define OWESHADER_FATAL_ERROR_H

#include <stdexcept>
#include <string>

namespace OWEShaderAux
{
    class OWEShaderError : public std::exception
    {
    public:
        OWEShaderError(const char *msg)
            : exception(msg)
        {

        }

        OWEShaderError(const std::string &msg)
            : exception(msg.c_str())
        {

        }

        const char *what(void) const
        {
            return std::exception::what();
        }
    };
}

#endif //OWESHADER_FATAL_ERROR_H
