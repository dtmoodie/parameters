#pragma once


// Check if being built with OpenCV
#include <map>
#include <functional>
#include <string>

#include <opencv2/core.hpp>
//#include <opencv2/core/persistence.hpp>
//#include <opencv2/core/types.hpp>
//#include <opencv2/core/mat.hpp>


#include <boost/lexical_cast.hpp>
#include <boost/function.hpp>

#include <LokiTypeInfo.h>
#include <Parameter_def.hpp>
#include <ITypedParameter.hpp>
#include <Types.hpp>

//class ::cv::cuda::GpuMat;
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
				static void RegisterFunction(Loki::TypeInfo type, SerializerFunction serializer, DeSerializerFunction deserializer);
				static std::pair<SerializerFunction,	DeSerializerFunction >& GetInterpretingFunction(Loki::TypeInfo type);
			private:
				// Mapping from Loki::typeinfo to file writing functors
				static	std::map<Loki::TypeInfo, std::pair<SerializerFunction, DeSerializerFunction >> registry;
			};
			Parameter_EXPORTS void Serialize(::cv::FileStorage* fs, Parameters::Parameter* param);
			Parameter_EXPORTS void DeSerialize(::cv::FileNode* fs, Parameters::Parameter* param);
			Parameter_EXPORTS Parameters::Parameter* DeSerialize(::cv::FileNode* fs);

			template<typename T, typename Enable = void> struct Serializer
			{
				virtual void Serialize(::cv::FileStorage* fs, T* param)
				{
					std::cout << "Default non specialized serializer called for " << typeid(T).name();
				}
				virtual void DeSerialize(::cv::FileNode* fs, T* param)
				{
					std::cout << "Default non specialized DeSerializer called for " << typeid(T).name();
				}
			};
			template<> struct Parameter_EXPORTS Serializer<char, void>
			{
				virtual void Serialize(::cv::FileStorage* fs, char* param);
				virtual void DeSerialize(::cv::FileNode* fs, char* param);
			};
			template<> struct Parameter_EXPORTS Serializer < std::string, void >
			{
				virtual void Serialize(::cv::FileStorage* fs, std::string* param);
				virtual void DeSerialize(::cv::FileNode* fs, std::string* param);
			};
			template<> struct Parameter_EXPORTS Serializer<::cv::Mat, void>
			{
				virtual void Serialize(::cv::FileStorage* fs, ::cv::Mat* param);
				virtual void DeSerialize(::cv::FileNode* fs, ::cv::Mat* param);
			};
			template<> struct Parameter_EXPORTS Serializer<Parameters::EnumParameter, void>
			{
				virtual void Serialize(::cv::FileStorage* fs, Parameters::EnumParameter* param);
				virtual void DeSerialize(::cv::FileNode* fs, Parameters::EnumParameter* param);
			};
			template<typename T> struct Serializer<T,typename  std::enable_if<std::is_unsigned<T>::value, void>::type>
			{
				virtual void Serialize(::cv::FileStorage* fs, T* param)
				{
					(*fs) << "Data" << boost::lexical_cast<std::string>(*param);
				}
				virtual void DeSerialize(::cv::FileNode* fs, T* param)
				{
					*param = boost::lexical_cast<T>((std::string)(*fs)["Data"]);
				}
			}; 
			template<typename T> struct Serializer<T, typename std::enable_if<
				std::is_same<typename std::remove_cv<T>::type, Parameters::EnumParameter		>::value || 
				std::is_same<typename std::remove_cv<T>::type, ::cv::cuda::GpuMat				>::value ||
				std::is_same<typename std::remove_cv<T>::type, boost::function<void(void)>		>::value, void>::type>
			{
				virtual void Serialize(::cv::FileStorage* fs, T* param){}
				virtual void DeSerialize(::cv::FileNode* fs, T* param){}
			};
			template<typename T> struct Serializer < T, typename std::enable_if<std::is_integral<T>::value && !std::is_unsigned<T>::value, void>::type>
			{
				virtual void Serialize(::cv::FileStorage* fs, T* param)
				{
					(*fs) << "Data" << *param;
				}
				virtual void DeSerialize(::cv::FileNode* fs, T* param)
				{
					(*fs)["Data"] >> *param;
				}
			};
			template<typename T, int M, int N> struct Serializer<::cv::Matx<T, M, N>, void>
			{
				virtual void Serialize(::cv::FileStorage* fs, ::cv::Matx<T, M, N>* param)
				{
					(*fs) << "Rows" << M;
					(*fs) << "Cols" << N;
					(*fs) << "Data" << "[";
					for (int i = 0; i < M; ++i)
					{
						(*fs) << "[:";
						for (int j = 0; j < N; ++j)
						{
							(*fs) << (*param)(i, j);
						}
						(*fs) << "]";
					}
					(*fs) << "]";
				}
				virtual void DeSerialize(::cv::FileNode* fs, ::cv::Matx<T, M, N>* param)
				{
					if ((int)(*fs)["Rows"] != M || (int)(*fs)["Cols"] != N)
						return;
					auto rowItr = (*fs)["Data"].begin();
					for (int i = 0; i < M; ++i, ++rowItr)
					{
						auto colItr = (*rowItr).begin();
						for (int j = 0; j < N; ++j, ++colItr)
						{
							(*param)(i, j) = (T)(*colItr);
						}
					}
				}
			};
			template<typename T, int M> struct Serializer<::cv::Vec<T, M>, void> : public Serializer<::cv::Matx<T,M,1>>{};
			template<typename T> struct Serializer<::cv::Scalar_<T>, void> : public Serializer<::cv::Vec<T, 4>>{};
			template<typename T> struct Serializer<std::vector<T>, typename std::enable_if<std::is_floating_point<T>::value || std::is_integral<T>::value, void>::type>
			{
				virtual void Serialize(::cv::FileStorage* fs, std::vector<T>* param)
				{
					(*fs) << "Data" << *param;
				}
				virtual void DeSerialize(::cv::FileNode* fs, std::vector<T>* param)
				{
					(*fs)["Data"] >> *param;
				}
			};
			template<typename T> struct SerializeWrapper
			{
				static void Write(::cv::FileStorage* fs, Parameter* param)
				{
					ITypedParameter<T>* typedParam = dynamic_cast<ITypedParameter<T>*>(param);
					Serializer<T> serializer;
					if (typedParam)
					{
						const std::string& toolTip = typedParam->GetTooltip();
						(*fs) << typedParam->GetName().c_str() << "{";
						(*fs) << "TreeName" << typedParam->GetTreeName();
						serializer.Serialize(fs, typedParam->Data());
						(*fs) << "Type" << typedParam->GetTypeInfo().name();
						if (toolTip.size())
							(*fs) << "ToolTip" << toolTip;
						(*fs) << "}";
					}
				}
				static void Read(::cv::FileNode* fs, Parameter* param)
				{
					ITypedParameter<T>* typedParam = dynamic_cast<ITypedParameter<T>*>(param);
					Serializer<T> serializer;
					if (typedParam)
					{
						if (fs->name() == param->GetName())
						{
							std::string type = (std::string)(*fs)["Type"];
							if (type == param->GetTypeInfo().name())
							{
								serializer.DeSerialize(fs, typedParam->Data());
								typedParam->UpdateSignal();
							}
							else
								::cv::error(::cv::Error::StsAssert, "Datatype " + std::string(param->GetTypeInfo().name()) + " requested, but " + type + " found in file", CV_Func, __FILE__, __LINE__);
						}
						else
						{
							::cv::FileNode myNode = (*fs)[param->GetName()];
							std::string type = (std::string)myNode["Type"];
							if (type == param->GetTypeInfo().name())
							{
								serializer.DeSerialize(&myNode, typedParam->Data());
								typedParam->UpdateSignal();
							}
							else
								::cv::error(::cv::Error::StsAssert, "Datatype " + std::string(param->GetTypeInfo().name()) + " requested, but " + type + " found in file", CV_Func, __FILE__, __LINE__);
						}
					}
				}
			};
			template<typename T> class PersistencePolicy
			{
				//static const Registerer<T> registerer = Registerer<T>();
			public:
				PersistencePolicy()
				{
					InterpreterRegistry::RegisterFunction(Loki::TypeInfo(typeid(T)), std::bind(SerializeWrapper<T>::Write, std::placeholders::_1, std::placeholders::_2), std::bind(SerializeWrapper<T>::Read, std::placeholders::_1, std::placeholders::_2));
				}
			};
		}
	}
	
}
