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
#include "parameters/Parameters.hpp"
#include "parameters/Persistence/TextSerializer.hpp"

using namespace Parameters::Persistence::Text;

std::map<Loki::TypeInfo, InterpreterRegistry::InterpreterSet>& InterpreterRegistry::registry()
{
    static std::map<Loki::TypeInfo, InterpreterRegistry::InterpreterSet> registry_instance;
    return registry_instance;
}
std::map<std::string, InterpreterRegistry::FactoryFunction>& InterpreterRegistry::factory()
{
    static std::map<std::string, InterpreterRegistry::FactoryFunction> factory_registry;
    return factory_registry;
}
void InterpreterRegistry::RegisterFunction(Loki::TypeInfo type, SerializerFunction serializer, SSDeSerializerFunction ssdeserializer, DeSerializerFunction deserializer, FactoryFunction factoryFunc)
{
    
    registry()[type] = std::make_tuple(serializer, ssdeserializer, deserializer);
    factory()[type.name()] = factoryFunc;
}

InterpreterRegistry::InterpreterSet& InterpreterRegistry::GetInterpretingFunction(Loki::TypeInfo type)
{
    
    if (registry().find(type) == registry().end())
    {
        LOG_TRIVIAL(debug) << type.name() << " not registered to the registry";
        throw std::invalid_argument("Datatype not registered to the registry");
    }
    return registry()[type];
}


void Parameters::Persistence::Text::Serialize(::std::stringstream* ss, Parameters::Parameter* param)
{
    
    std::get<0>(InterpreterRegistry::GetInterpretingFunction(param->GetTypeInfo())).operator()(ss, param);
    //InterpreterRegistry::GetInterpretingFunction(param->GetTypeInfo()).first(ss, param);
}

void Parameters::Persistence::Text::DeSerialize(::std::stringstream* ss, Parameters::Parameter* param)
{
    
    std::get<1>(InterpreterRegistry::GetInterpretingFunction(param->GetTypeInfo())).operator()(ss, param);
}
void Parameters::Persistence::Text::DeSerialize(::std::string* ss, Parameters::Parameter* param)
{
    std::get<2>(InterpreterRegistry::GetInterpretingFunction(param->GetTypeInfo())).operator()(ss, param);
}

std::shared_ptr<Parameters::Parameter> Parameters::Persistence::Text::DeSerialize(::std::stringstream* ss)
{
    
    //TODO object factory based on serialized type
    std::string type;
	std::getline(*ss, type, ']');
	std::string name;
	std::getline(*ss, name, '#');
	auto pos = name.find_first_of(':');
	std::string root;
	if (pos != std::string::npos)
	{
		root = name.substr(0, pos);
		name = name.substr(pos+1);
	}
	auto param = InterpreterRegistry::factory()[type.substr(1)](name);
	param->SetTreeRoot(root);
	std::get<1>(InterpreterRegistry::registry()[param->GetTypeInfo()])(ss, param.get());
	return param;
}
std::shared_ptr<Parameters::Parameter> Parameters::Persistence::Text::DeSerialize(::std::string* str)
{
	std::stringstream ss;
	ss << *str;
	//TODO object factory based on serialized type
	std::string type;
	std::getline(ss, type, ']');
	std::string name;
	std::getline(ss, name, '#');
	auto pos = name.find_first_of(':');
	std::string root;
	if (pos != std::string::npos)
	{
		root = name.substr(0, pos);
		name = name.substr(pos + 1);
	}
	auto param = InterpreterRegistry::factory()[type.substr(1)](name);
	param->SetTreeRoot(root);
	std::get<1>(InterpreterRegistry::registry()[param->GetTypeInfo()])(&ss, param.get());
	return param;
}
void Serializer<Parameters::EnumParameter, void>::Serialize(::std::stringstream* ss, Parameters::EnumParameter* param)
{
    
    (*ss) << "Values:";
    for (int i = 0; i < param->enumerations.size(); ++i)
    {
        (*ss) << "(" << param->values[i] << "," << param->enumerations[i] << ")";
    }
}

bool Serializer<Parameters::EnumParameter, void>::DeSerialize(::std::stringstream* ss, Parameters::EnumParameter* param)
{
    std::string value;
    (*ss) >> value;
    int index = -1000;
    try
    {   
        index = boost::lexical_cast<int>(value);
        for(int i = 0; i < param->values.size(); ++i)
        {
            if(param->values[i] == index)
            {
                param->currentSelection = i;
                return true;
            }
        }
    }catch(...){}
    for(int i = 0; i < param->enumerations.size(); ++i)
    {
        if(param->enumerations[i] == value)
        {
            param->currentSelection = i;
            return true;
        }
    }
    return false;
}

bool Serializer<Parameters::EnumParameter, void>::DeSerialize(::std::string* ss, Parameters::EnumParameter* param)
{
    int index = -1000;
    try
    {   
        index = boost::lexical_cast<int>(*ss);
        if(index > 0 && index < param->enumerations.size())
        {
            param->currentSelection = index;
            return true;
        }
    }catch(...){}
    for(int i = 0; i < param->enumerations.size(); ++i)
    {
        if(param->enumerations[i] == *ss)
        {
            param->currentSelection = i;
            return true;
        }
    }
    return false;
}

void Parameters::Persistence::Text::Serializer<::cv::cuda::GpuMat, void>::Serialize(::std::stringstream* ss, ::cv::cuda::GpuMat* param)
{

}
bool Parameters::Persistence::Text::Serializer<::cv::cuda::GpuMat, void>::DeSerialize(::std::stringstream* ss, ::cv::cuda::GpuMat* param)
{
    return false;
}
bool Parameters::Persistence::Text::Serializer<::cv::cuda::GpuMat, void>::DeSerialize(::std::string* ss, ::cv::cuda::GpuMat* param)
{
    return false;
}
