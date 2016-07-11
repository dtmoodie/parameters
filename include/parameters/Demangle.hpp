#pragma once
#include "Parameter_def.hpp"
#include "LokiTypeInfo.h"

namespace Parameters
{
    class PARAMETER_EXPORTS Demangle
    {
    public:
        static std::string TypeToName(Loki::TypeInfo type);
        static void RegisterName(Loki::TypeInfo type, const char* name);
    };
}