#include "Parameters.hpp"
#include <opencv2/core/base.hpp>

using namespace Parameters::Persistence::cv;

std::map<Loki::TypeInfo, std::pair<InterpreterRegistry::SerializerFunction, InterpreterRegistry::DeSerializerFunction >> InterpreterRegistry::registry;

void InterpreterRegistry::RegisterFunction(Loki::TypeInfo& type, SerializerFunction serializer, DeSerializerFunction deserializer)
{
	registry[type] = std::make_pair(serializer, deserializer);
}

std::pair<InterpreterRegistry::SerializerFunction, InterpreterRegistry::DeSerializerFunction >& InterpreterRegistry::GetInterpretingFunction(Loki::TypeInfo& type)
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
void Serializer<char, void>::Serialize(::cv::FileStorage* fs, Parameters::Parameter* param)
{
	Parameters::ITypedParameter<char>* typedParam = dynamic_cast<Parameters::ITypedParameter<char>*>(param);
	if (typedParam)
	{
		const std::string& toolTip = typedParam->GetTooltip();
		(*fs) << typedParam->GetName().c_str() << "{";
		(*fs) << "Data" << (signed char)*typedParam->Data();
		(*fs) << "Type" << typedParam->GetTypeInfo().name();
		if (toolTip.size())
			(*fs) << "ToolTip" << toolTip;
		(*fs) << "}";
	}
}

void Serializer<char, void>::DeSerialize(::cv::FileNode* fs, Parameters::Parameter* param)
{
	Parameters::ITypedParameter<char>* typedParam = dynamic_cast<Parameters::ITypedParameter<char>*>(param);
	if (typedParam)
	{
		::cv::FileNode myNode = (*fs)[param->GetName()];
		std::string type = (std::string)myNode["Type"];
		signed char data;
		if (type == param->GetTypeInfo().name())
		{
			myNode["Data"] >> data;
			*typedParam->Data() = (char)data;
		}
		else
			::cv::error(::cv::Error::StsAssert, "Datatype " + std::string(param->GetTypeInfo().name()) + " requested, but " + type + " found in file", CV_Func, __FILE__, __LINE__);
	}
}

void Serializer<std::string, void>::Serialize(::cv::FileStorage* fs, Parameters::Parameter* param)
{
	ITypedParameter<std::string>* typedParam = dynamic_cast<ITypedParameter<std::string>*>(param);
	if (typedParam)
	{
		const std::string& toolTip = typedParam->GetTooltip();
		(*fs) << typedParam->GetName().c_str() << "{";
		(*fs) << "Data" << *typedParam->Data();
		(*fs) << "Type" << typedParam->GetTypeInfo().name();
		if (toolTip.size())
			(*fs) << "ToolTip" << toolTip;
		(*fs) << "}";
	}
}

void Serializer<std::string, void>::DeSerialize(::cv::FileNode* fs, Parameters::Parameter* param)
{
	ITypedParameter<std::string>* typedParam = dynamic_cast<ITypedParameter<std::string>*>(param);
	if (typedParam)
	{
		::cv::FileNode myNode = (*fs)[param->GetName()];
		std::string type = (std::string)myNode["Type"];
		if (type == param->GetTypeInfo().name())
		{
			//(*typedParam->Data()) = ;
			typedParam->UpdateData((std::string)myNode["Data"]);
		}
			
		
	}
}
			