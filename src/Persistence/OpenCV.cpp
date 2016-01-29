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
#ifdef OPENCV_FOUND
#include "Parameters.hpp"
#include <opencv2/core/base.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/attributes/named_scope.hpp>

using namespace Parameters::Persistence::cv;

std::map<Loki::TypeInfo, std::tuple<InterpreterRegistry::SerializerFunction, InterpreterRegistry::DeSerializerFunction, InterpreterRegistry::FactoryFunction >>& InterpreterRegistry::registry()
{
	static std::map<Loki::TypeInfo, std::tuple<InterpreterRegistry::SerializerFunction, InterpreterRegistry::DeSerializerFunction, InterpreterRegistry::FactoryFunction >> instance;
	return instance;
}

void InterpreterRegistry::RegisterFunction(Loki::TypeInfo type, SerializerFunction serializer, DeSerializerFunction deserializer, FactoryFunction creator)
{
	LOG_TRACE;
    
	registry()[type] = std::make_tuple(serializer, deserializer, creator);
}

std::tuple<InterpreterRegistry::SerializerFunction, InterpreterRegistry::DeSerializerFunction, InterpreterRegistry::FactoryFunction >& InterpreterRegistry::GetInterpretingFunction(Loki::TypeInfo type)
{
	LOG_TRACE;
	if (registry().find(type) == registry().end())
	{
		LOG_TRIVIAL(warning) << type.name() << " not registered to the registry";
		::cv::error(::cv::Error::StsAssert, "Datatype not registered to the registry", CV_Func, __FILE__, __LINE__);
	}
		
	return registry()[type];
}
std::tuple<InterpreterRegistry::SerializerFunction, InterpreterRegistry::DeSerializerFunction, InterpreterRegistry::FactoryFunction > InterpreterRegistry::GetInterpretingFunction(std::string type_string)
{
    for(auto itr : registry())
    {
        if(itr.first.name() == type_string)
        {
            return itr.second;
        }
    }
    std::tuple<InterpreterRegistry::SerializerFunction, InterpreterRegistry::DeSerializerFunction, InterpreterRegistry::FactoryFunction >();
}
void Parameters::Persistence::cv::Serialize(::cv::FileStorage* fs, Parameters::Parameter* param)
{
	LOG_TRACE;
	std::get<0>(InterpreterRegistry::GetInterpretingFunction(param->GetTypeInfo()))(fs, param);
}
void Parameters::Persistence::cv::DeSerialize(::cv::FileNode* fs, Parameters::Parameter* param)
{
	LOG_TRACE;
	std::get<1>(InterpreterRegistry::GetInterpretingFunction(param->GetTypeInfo()))(fs, param);
}
Parameters::Parameter* Parameters::Persistence::cv::DeSerialize(::cv::FileNode* fs)
{
	LOG_TRACE;
    std::string type = (std::string)(*fs)["Type"];
    return std::get<2>(InterpreterRegistry::GetInterpretingFunction(type))(fs);
	return nullptr;
}
void Serializer<char, void>::Serialize(::cv::FileStorage* fs, char* param)
{
	LOG_TRACE;
    
	(*fs) << (signed char)*param;
}

void Serializer<char, void>::DeSerialize(::cv::FileNode& fs, char* param)
{
	LOG_TRACE;
	signed char data;
	fs >> data;
	*param = (char)data;
}

void Serializer<std::string, void>::Serialize(::cv::FileStorage* fs, std::string* param)
{
	LOG_TRACE;
	(*fs) << *param;
}

void Serializer<std::string, void>::DeSerialize(::cv::FileNode& fs, std::string* param)
{
	LOG_TRACE;
	*param = (std::string)(fs);
}
void Serializer<::cv::Mat, void>::Serialize(::cv::FileStorage* fs, ::cv::Mat* param)
{
	LOG_TRACE;
	(*fs) << *param;
}

void Serializer<::cv::Mat, void>::DeSerialize(::cv::FileNode& fs, ::cv::Mat* param)
{
	LOG_TRACE;
	(fs) >> *param;
}

void Serializer<Parameters::EnumParameter, void>::Serialize(::cv::FileStorage* fs, Parameters::EnumParameter* param)
{
	LOG_TRACE;
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
	LOG_TRACE;
	(*fs)["Values"] >> param->values;
	
	auto end = (*fs)["Enumerations"].end();
	param->enumerations.clear();
	for (auto itr = (*fs)["Enumerations"].begin(); itr != end; ++itr)
	{
		param->enumerations.push_back((std::string)(*itr));
	}
	(*fs)["Current value"] >> param->currentSelection;
}*/
#endif // OPENCV_FOUND