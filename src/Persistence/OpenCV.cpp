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
#ifdef HAVE_OPENCV
#include "parameters/Parameters.hpp"
#include <opencv2/core/base.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/attributes/named_scope.hpp>

using namespace Parameters::Persistence::cv;



void InterpreterRegistry::RegisterFunction(Loki::TypeInfo type, SerializerFunction serializer, DeSerializerFunction deserializer, FactoryFunction creator)
{    
    registry[type] = std::make_tuple(serializer, deserializer, creator);
}

std::tuple<InterpreterRegistry::SerializerFunction, InterpreterRegistry::DeSerializerFunction, InterpreterRegistry::FactoryFunction >& InterpreterRegistry::GetInterpretingFunction(Loki::TypeInfo type)
{
    
    if (registry.find(type) == registry.end())
    {
        LOG_TRIVIAL(debug) << type.name() << " not registered to the registry";
        ::cv::error(::cv::Error::StsAssert, "Datatype not registered to the registry", CV_Func, __FILE__, __LINE__);
    }
        
    return registry[type];
}

InterpreterRegistry* InterpreterRegistry::instance()
{
    static InterpreterRegistry inst;
    return &inst;
}
std::tuple<InterpreterRegistry::SerializerFunction, InterpreterRegistry::DeSerializerFunction, InterpreterRegistry::FactoryFunction > InterpreterRegistry::GetInterpretingFunction(std::string type_string)
{
    for(auto itr : registry)
    {
        if(itr.first.name() == type_string)
        {
            return itr.second;
        }
    }
    LOG_TRIVIAL(debug) << type_string << " not registered to the registry";
    ::cv::error(::cv::Error::StsAssert, "Datatype not registered to the registry", CV_Func, __FILE__, __LINE__);
    return std::tuple<InterpreterRegistry::SerializerFunction, InterpreterRegistry::DeSerializerFunction, InterpreterRegistry::FactoryFunction >();
}
void Parameters::Persistence::cv::Serialize(::cv::FileStorage* fs, Parameters::Parameter* param)
{
    
    std::get<0>(InterpreterRegistry::instance()->GetInterpretingFunction(param->GetTypeInfo()))(fs, param);
}
void Parameters::Persistence::cv::DeSerialize(::cv::FileNode* fs, Parameters::Parameter* param)
{
    
    std::get<1>(InterpreterRegistry::instance()->GetInterpretingFunction(param->GetTypeInfo()))(fs, param);
}
Parameters::Parameter* Parameters::Persistence::cv::DeSerialize(::cv::FileNode* fs)
{
    
    std::string type = (std::string)(*fs)["Type"];
    if(type.size())
    {
        try
        {
            return std::get<2>(InterpreterRegistry::instance()->GetInterpretingFunction(type))(fs);
        }catch(...)
        {
            return nullptr;
        }
    }
    return nullptr;
}
void Serializer<char, void>::Serialize(::cv::FileStorage* fs, char* param)
{
    
    
    (*fs) << (signed char)*param;
}

void Serializer<char, void>::DeSerialize(::cv::FileNode& fs, char* param)
{
    
    signed char data;
    fs >> data;
    *param = (char)data;
}

void Serializer<std::string, void>::Serialize(::cv::FileStorage* fs, std::string* param)
{
    
    (*fs) << *param;
}

void Serializer<std::string, void>::DeSerialize(::cv::FileNode& fs, std::string* param)
{
    
    *param = (std::string)(fs);
}
void Serializer<::cv::Mat, void>::Serialize(::cv::FileStorage* fs, ::cv::Mat* param)
{
    
    (*fs) << *param;
}

void Serializer<::cv::Mat, void>::DeSerialize(::cv::FileNode& fs, ::cv::Mat* param)
{
    
    (fs) >> *param;
}

void Serializer<Parameters::EnumParameter, void>::Serialize(::cv::FileStorage* fs, Parameters::EnumParameter* param)
{
    
    (*fs) << "Values" << param->values;
    (*fs) << "Enumerations" << "[:";
    for (int i = 0; i < param->enumerations.size(); ++i)
    {
        (*fs) << param->enumerations[i];
    }
    (*fs) << "]";
    (*fs) << "Current value" << param->currentSelection;
}
/*void Serializer<Parameters::EnumParameter, void>::DeSerialize(::cv::FileNode* fs, Parameters::EnumParameter* param)
{
    
    (*fs)["Values"] >> param->values;
    
    auto end = (*fs)["Enumerations"].end();
    param->enumerations.clear();
    for (auto itr = (*fs)["Enumerations"].begin(); itr != end; ++itr)
    {
        param->enumerations.push_back((std::string)(*itr));
    }
    (*fs)["Current value"] >> param->currentSelection;
}*/
#endif // HAVE_OPENCV
