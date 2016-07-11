/*
Copyright (c) 2015 Daniel Moodie.
All rights reserved.

Redistribution and use in source and binary forms are permitted
provided that the above copyright notice and this paragraph are
duplicated in all such forms and that any documentation,
advertising materials, and other materials related to such
distribution and use acknowledge that the software was developed
by the Daniel Moodie. The name of
Daniel Moodie may not be used to endorse or promote products derived
from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

https://github.com/dtmoodie/parameters
*/
#pragma once
#include "Persistence/Persistence.hpp"
#include "UI/UI.hpp"
#include <parameters/buffers/BufferProxy.hpp>
#include "Converters/ConverterPolicy.hpp"
#include "parameters/Demangle.hpp"


namespace Parameters
{
    template<typename T> class MetaTypedParameter
    {
    public:
        MetaTypedParameter()
        {
            static Persistence::PersistencePolicy<T> persistence;
            static UI::UiPolicy<T> ui;
            static Buffer::ParameterBufferPolicy<T> buffers;
            static Converters::ConverterPolicy<T> converters;
        }
    };
    template<typename T> struct RegisterDemangledName
    {
        RegisterDemangledName(const char* name)
        {
            Demangle::RegisterName(Loki::TypeInfo(typeid(T)), name);
        }
    };
    
}
#define COMBINE1(X,Y) X##Y  // helper macro
#define COMBINE(X,Y) COMBINE1(X,Y)

#define REGISTER_META_PARAMETER_(TYPE, N) \
static Parameters::MetaTypedParameter<TYPE> COMBINE(inst, N)\
static Parameters::RegisterDemangledName<T> COMBINE(demangle_inst, N)(#TYPE)

#define REGISTER_META_PARAMETER(TYPE) REGISTER_META_PARAMETER_(TYPE, __LINE__)