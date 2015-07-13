#pragma once


// Check if being built with OpenCV
#include <map>
#include <functional>

#include <opencv2/core/persistence.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/core/mat.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/function.hpp>

#include <LokiTypeInfo.h>
#include <Parameter_def.hpp>
#include <Types.hpp>

class ::cv::cuda::GpuMat;
namespace Parameters
{
	class Parameter;
	namespace Persistence
	{
		namespace cv
		{

			class Parameter_EXPORTS InterpreterRegistry
			{
				
			public:
				typedef std::function<void(::cv::FileStorage*, Parameters::Parameter*)> SerializerFunction;
				typedef std::function<void(::cv::FileNode*, Parameters::Parameter*)> DeSerializerFunction;
				static void RegisterFunction(Loki::TypeInfo& type, SerializerFunction serializer, DeSerializerFunction deserializer);
				static std::pair<SerializerFunction,	DeSerializerFunction >& GetInterpretingFunction(Loki::TypeInfo& type);
			private:
				// Mapping from Loki::typeinfo to file writing functors
				static	std::map<Loki::TypeInfo, std::pair<SerializerFunction, DeSerializerFunction >> registry;
			};
			Parameter_EXPORTS void Serialize(::cv::FileStorage* fs, Parameters::Parameter* param);
			Parameter_EXPORTS void DeSerialize(::cv::FileNode* fs, Parameters::Parameter* param);
			Parameter_EXPORTS Parameters::Parameter* DeSerialize(::cv::FileNode* fs);

			template<typename T, typename Enable = void> struct Serializer
			{
				static void Serialize(::cv::FileStorage* fs, Parameters::Parameter* param)
				{
					/*ITypedParameter<T>* typedParam = dynamic_cast<ITypedParameter<T>*>(param);
					if (typedParam)
					{
						const std::string& toolTip = typedParam->GetTooltip();
						(*fs) << typedParam->GetName().c_str() << "{";
						(*fs) << "Data" << *typedParam->Data();
						(*fs) << "Type" << typedParam->GetTypeInfo().name();
						if (toolTip.size())
							(*fs) << "ToolTip" << toolTip;
						(*fs) << "}";
					}*/
				}
				static void DeSerialize(::cv::FileNode* fs, Parameters::Parameter* param)
				{

				}
			};
			template<> struct Parameter_EXPORTS Serializer<char, void>
			{
				static void Serialize(::cv::FileStorage* fs, Parameters::Parameter* param);
				static void DeSerialize(::cv::FileNode* fs, Parameters::Parameter* param);
			};
			template<typename T> struct Serializer<T,typename  std::enable_if<std::is_unsigned<T>::value, void>::type>
			{
				static void Serialize(::cv::FileStorage* fs, Parameters::Parameter* param)
				{
					Parameters::ITypedParameter<T>* typedParam = dynamic_cast<Parameters::ITypedParameter<T>*>(param);
					if (typedParam)
					{
						const std::string& toolTip = typedParam->GetTooltip();
						(*fs) << typedParam->GetName().c_str() << "{";
						(*fs) << "Data" << (int)*typedParam->Data();
						(*fs) << "Type" << typedParam->GetTypeInfo().name();
						if (toolTip.size())
							(*fs) << "ToolTip" << toolTip;
						(*fs) << "}";
					}
				}
				static void DeSerialize(::cv::FileNode* fs, Parameters::Parameter* param)
				{
					ITypedParameter<T>* typedParam = dynamic_cast<ITypedParameter<T>*>(param);
					if (typedParam)
					{
						::cv::FileNode myNode = (*fs)[param->GetName()];
						std::string type = (std::string)myNode["Type"];
						int data;
						if (type == param->GetTypeInfo().name())
						{
							myNode["Data"] >> data; 
							*typedParam->Data() = data;
						}							
						else
							::cv::error(::cv::Error::StsAssert, "Datatype " + std::string(param->GetTypeInfo().name()) + " requested, but " + type + " found in file", CV_Func, __FILE__, __LINE__);
					}
				}
			}; 
				
			template<typename T> struct Serializer<T, typename std::enable_if<
				std::is_same<typename std::remove_cv<T>::type, Parameters::EnumParameter		>::value || 
				std::is_same<typename std::remove_cv<T>::type, ::cv::cuda::GpuMat				>::value ||
				std::is_same<typename std::remove_cv<T>::type, boost::function<void(void)>		>::value, void>::type>
			{
				static void Serialize(::cv::FileStorage* fs, Parameters::Parameter* param)
				{

				}
				static void DeSerialize(::cv::FileNode* fs, Parameters::Parameter* param)
				{

				}
			};
			template<typename T> struct Serializer < T, typename std::enable_if<std::is_integral<T>::value && !std::is_unsigned<T>::value, void>::type>
			{
				static void Serialize(::cv::FileStorage* fs, Parameters::Parameter* param)
				{
					ITypedParameter<T>* typedParam = dynamic_cast<ITypedParameter<T>*>(param);
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
				static void DeSerialize(::cv::FileNode* fs, Parameters::Parameter* param)
				{
					ITypedParameter<T>* typedParam = dynamic_cast<ITypedParameter<T>*>(param);
					if (typedParam)
					{
						::cv::FileNode myNode = (*fs)[param->GetName()];
						std::string type = (std::string)myNode["Type"];
						if (type == param->GetTypeInfo().name())
							myNode["Data"] >> *typedParam->Data();
						else
							::cv::error(::cv::Error::StsAssert, "Datatype " + std::string(param->GetTypeInfo().name()) + " requested, but " + type + " found in file", CV_Func, __FILE__, __LINE__);
					}
				}
			};
			
			template<typename T> struct Serializer<T, typename std::enable_if<std::is_same<T, ::cv::Mat>::value, void>::type>
			{
				static void Serialize(::cv::FileStorage* fs, Parameters::Parameter* param)
				{
					ITypedParameter<T>* typedParam = dynamic_cast<ITypedParameter<T>*>(param);
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
				static void DeSerialize(::cv::FileNode* fs, Parameters::Parameter* param)
				{
					ITypedParameter<T>* typedParam = dynamic_cast<ITypedParameter<T>*>(param);
					if (typedParam)
					{
						::cv::FileNode myNode = (*fs)[param->GetName()];
						std::string type = (std::string)myNode["Type"];
						if (type == param->GetTypeInfo().name())
							myNode["Data"] >> *typedParam->Data();
						else
							::cv::error(::cv::Error::StsAssert, "Datatype " + std::string(param->GetTypeInfo().name()) + " requested, but " + type + " found in file", CV_Func, __FILE__, __LINE__);
					}
				}
			};

			template<typename T> class PersistencePolicy
			{
				//static const Registerer<T> registerer = Registerer<T>();
			public:
				PersistencePolicy()
				{
					InterpreterRegistry::RegisterFunction(Loki::TypeInfo(typeid(T)), std::bind(Serializer<T>::Serialize, std::placeholders::_1, std::placeholders::_2), std::bind(Serializer<T>::DeSerialize, std::placeholders::_1, std::placeholders::_2));
				}
			};
		}
	}
	
}
