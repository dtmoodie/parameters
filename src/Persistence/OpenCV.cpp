#include "Parameters.hpp"
#include <opencv2/core/base.hpp>

using namespace Parameters::Persistence::cv;

std::map<Loki::TypeInfo, std::pair<InterpreterRegistry::SerializerFunction, InterpreterRegistry::DeSerializerFunction >> InterpreterRegistry::registry;

void InterpreterRegistry::RegisterFunction(Loki::TypeInfo type, SerializerFunction serializer, DeSerializerFunction deserializer)
{
	registry[type] = std::make_pair(serializer, deserializer);
}

std::pair<InterpreterRegistry::SerializerFunction, InterpreterRegistry::DeSerializerFunction >& InterpreterRegistry::GetInterpretingFunction(Loki::TypeInfo type)
{
	if (registry.find(type) == registry.end())
		::cv::error(::cv::Error::StsAssert, "Datatype not registered to the registry", CV_Func, __FILE__, __LINE__);
	return registry[type];
}
void Parameters::Persistence::cv::Serialize(::cv::FileStorage* fs, Parameters::Parameter* param)
{
	InterpreterRegistry::GetInterpretingFunction(param->GetTypeInfo()).first(fs, param);
}
void Parameters::Persistence::cv::DeSerialize(::cv::FileNode* fs, Parameters::Parameter* param)
{
	InterpreterRegistry::GetInterpretingFunction(param->GetTypeInfo()).second(fs, param);
}
Parameters::Parameter* Parameters::Persistence::cv::DeSerialize(::cv::FileNode* fs)
{
	//TODO object factory based on serialized type
	return nullptr;
}
void Serializer<char, void>::Serialize(::cv::FileStorage* fs, char* param)
{
	(*fs) << "Data" << (signed char)*param;
}

void Serializer<char, void>::DeSerialize(::cv::FileNode* fs, char* param)
{
	signed char data;
	(*fs)["Data"] >> data;
	*param = (char)data;
}

void Serializer<std::string, void>::Serialize(::cv::FileStorage* fs, std::string* param)
{
	(*fs) << "Data" << *param;
}

void Serializer<std::string, void>::DeSerialize(::cv::FileNode* fs, std::string* param)
{
	*param = (std::string)(*fs)["Data"];
}
void Serializer<::cv::Mat, void>::Serialize(::cv::FileStorage* fs, ::cv::Mat* param)
{
	(*fs) << "Data" << *param;
}

void Serializer<::cv::Mat, void>::DeSerialize(::cv::FileNode* fs, ::cv::Mat* param)
{
	(*fs)["Data"] >> *param;
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
void Serializer<Parameters::EnumParameter, void>::DeSerialize(::cv::FileNode* fs, Parameters::EnumParameter* param)
{
	(*fs)["Values"] >> param->values;
	
	auto end = (*fs)["Enumerations"].end();
	param->enumerations.clear();
	for (auto itr = (*fs)["Enumerations"].begin(); itr != end; ++itr)
	{
		param->enumerations.push_back((std::string)(*itr));
	}
	(*fs)["Current value"] >> param->currentSelection;
}