#pragma once

#ifdef OPENCV_FOUND
// Check if being built with OpenCV
#include <map>
#include <functional>
#include <string>

#include <opencv2/core.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/function.hpp>

#include <LokiTypeInfo.h>
#include <Parameter_def.hpp>
#include <ITypedParameter.hpp>
#include <Types.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/attributes/named_scope.hpp>

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
				static	std::map<Loki::TypeInfo, std::pair<SerializerFunction, DeSerializerFunction >>& registry();
			};
			Parameter_EXPORTS void Serialize(::cv::FileStorage* fs, Parameters::Parameter* param);
			Parameter_EXPORTS void DeSerialize(::cv::FileNode* fs, Parameters::Parameter* param);
			Parameter_EXPORTS Parameters::Parameter* DeSerialize(::cv::FileNode* fs);

			template<typename T, typename Enable = void> struct Serializer
			{
				static void Serialize(::cv::FileStorage* fs, T* param)
				{
					LOG_TRIVIAL(info) << "Default non specialized serializer called for " << typeid(T).name();
					//std::cout << "Default non specialized serializer called for " << typeid(T).name();
				}
				template<typename U> static void DeSerialize(U fs, T* param)
				{
					LOG_TRIVIAL(info) << "Default non specialized DeSerializer called for " << typeid(T).name();
				}
			};
			template<> struct Parameter_EXPORTS Serializer<char, void>
			{
				static void Serialize(::cv::FileStorage* fs, char* param);
				template<typename U> static void DeSerialize(U fs, char* param)
				{
					LOG_TRACE;
					signed char data;
					(*fs)["Data"] >> data;
					*param = (char)data;
				}
			};
			template<> struct Parameter_EXPORTS Serializer < std::string, void >
			{
				static void Serialize(::cv::FileStorage* fs, std::string* param);
				template<typename U> static void DeSerialize(U fs, std::string* param)
				{
					LOG_TRACE;
					*param = (std::string)(*fs)["Data"];
				}
			};
			template<> struct Parameter_EXPORTS Serializer<::cv::Mat, void>
			{
				static void Serialize(::cv::FileStorage* fs, ::cv::Mat* param);
				template<typename U> static void DeSerialize(U fs, ::cv::Mat* param)
				{
					LOG_TRACE;
					(*fs)["Data"] >> *param;
				}
			};
			template<> struct Parameter_EXPORTS Serializer<Parameters::EnumParameter, void>
			{
				static void Serialize(::cv::FileStorage* fs, Parameters::EnumParameter* param);
				template<typename U> static void DeSerialize(U fs, Parameters::EnumParameter* param)
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
				}
			};
			template<typename T> struct Serializer<T,typename  std::enable_if<std::is_unsigned<T>::value || std::is_same<T,long>::value || std::is_same<T, long long>::value, void>::type>
			{
				static void Serialize(::cv::FileStorage* fs, T* param)
				{
					LOG_TRACE;
					(*fs) << "Data" << boost::lexical_cast<std::string>(*param);
				}
				template<typename U> static void DeSerialize(U fs, T* param)
				{
					LOG_TRACE;
					*param = boost::lexical_cast<T>((std::string)(*fs)["Data"]);
				}
			}; 
			template<typename T> struct Serializer<T, typename std::enable_if<
				std::is_same<typename std::remove_cv<T>::type, Parameters::EnumParameter		>::value || 
				std::is_same<typename std::remove_cv<T>::type, ::cv::cuda::GpuMat				>::value ||
				std::is_same<typename std::remove_cv<T>::type, boost::function<void(void)>		>::value, void>::type>
			{
				static void Serialize(::cv::FileStorage* fs, T* param){}
				virtual void DeSerialize(::cv::FileNode* fs, T* param){}
			};
			template<typename T> struct Serializer < T, typename std::enable_if<std::is_integral<T>::value && !std::is_unsigned<T>::value && !std::is_same<T, long>::value && !std::is_same<T, long long>::value, void>::type>
			{
				static void Serialize(::cv::FileStorage* fs, T* param)
				{
					LOG_TRACE;
					(*fs) << "Data" << *param;
				}
				template<typename U> static void DeSerialize(U fs, T* param)
				{
					LOG_TRACE;
					(*fs)["Data"] >> *param;
				}
			};
			template<typename T, int M, int N> struct Serializer<::cv::Matx<T, M, N>, void>
			{
				static void Serialize(::cv::FileStorage* fs, ::cv::Matx<T, M, N>* param)
				{
					LOG_TRACE;
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
				template<typename U> static void DeSerialize(U fs, ::cv::Matx<T, M, N>* param)
				{
					LOG_TRACE;
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
			// TODO, this doesn't work for types like std::vector<unsigned int> because opencv doesn't natively support it.  Instead needs to be modified to manually handle serialization
			template<typename T> struct Serializer<std::vector<T>, typename std::enable_if<std::is_floating_point<T>::value || std::is_integral<T>::value, void>::type>: public Serializer<T>
			{
				static void Serialize(::cv::FileStorage* fs, std::vector<T>* param)
				{
					LOG_TRACE;
					//(*fs) << "Data" << *param;
					(*fs) << "Data" << "[";
					for (auto itr = param->begin(); itr != param->end(); ++itr)
					{
						Serializer<T>::Serialize(fs, &(*itr));
					}
					(*fs) << "]";
					
				}
				template<typename U> static void DeSerialize(U fs, std::vector<T>* param)
				{
					LOG_TRACE;
					//(*fs)["Data"] >> *param;
					
					for (auto itr = (*fs)["Data"].begin(); itr != (*fs)["Data"].end(); ++itr)
					{
						param->push_back(T());
						Serializer<T>::DeSerialize(itr, &(*param)[param->size() - 1]);
					}
				}

			};
			template<typename T> struct SerializeWrapper
			{
				static void Write(::cv::FileStorage* fs, Parameter* param)
				{
					LOG_TRIVIAL(info) << "Writing parameter with name " << param->GetName();
					ITypedParameter<T>* typedParam = dynamic_cast<ITypedParameter<T>*>(param);
					Serializer<T> serializer;
					if (typedParam)
					{
						const std::string& toolTip = typedParam->GetTooltip();
						try
						{
							(*fs) << typedParam->GetName().c_str() << "{";
							(*fs) << "TreeName" << typedParam->GetTreeName();
						}
						catch (::cv::Exception& e)
						{
							LOG_TRIVIAL(warning) << "Invalid tree name for parameter: " << typedParam->GetTreeName() << " " << e.what();
							return;
						}						
						serializer.Serialize(fs, typedParam->Data());
						(*fs) << "Type" << typedParam->GetTypeInfo().name();
						if (toolTip.size())
							(*fs) << "ToolTip" << toolTip;
						(*fs) << "}";
					}
				}
				static void Read(::cv::FileNode* fs, Parameter* param)
				{
					LOG_TRIVIAL(info) << "Reading parameter with name " << param->GetName();
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
                                typedParam->changed = true;
								typedParam->UpdateSignal(nullptr);
								LOG_TRIVIAL(info) << "Successfully read " << param->GetName();
							}
							else
							{
								LOG_TRIVIAL(error) << "Datatype " + std::string(param->GetTypeInfo().name()) + " requested, but " + type + " found in file";
								::cv::error(::cv::Error::StsAssert, "Datatype " + std::string(param->GetTypeInfo().name()) + " requested, but " + type + " found in file", CV_Func, __FILE__, __LINE__);
							}								
						}
						else
						{
							::cv::FileNode myNode = (*fs)[param->GetName()];
							std::string type = (std::string)myNode["Type"];
							if (type == param->GetTypeInfo().name())
							{
								serializer.DeSerialize(&myNode, typedParam->Data());
                                typedParam->changed = true;
								typedParam->UpdateSignal(nullptr);
								LOG_TRIVIAL(info) << "Successfully read " << param->GetName();
							}
							else
							{
								LOG_TRIVIAL(error) << "Datatype " + std::string(param->GetTypeInfo().name()) + " requested, but " + type + " found in file";
								::cv::error(::cv::Error::StsAssert, "Datatype " + std::string(param->GetTypeInfo().name()) + " requested, but " + type + " found in file", CV_Func, __FILE__, __LINE__);
							}	
						}
					}
				}
			};

			template<typename T> class Constructor
			{
			public:
				Constructor()
				{
					InterpreterRegistry::RegisterFunction(Loki::TypeInfo(typeid(T)), std::bind(SerializeWrapper<T>::Write, std::placeholders::_1, std::placeholders::_2), std::bind(SerializeWrapper<T>::Read, std::placeholders::_1, std::placeholders::_2));
				}
			};

			template<typename T> class PersistencePolicy
			{
				static Constructor<T> constructor;
			public:
				PersistencePolicy()
				{
					(void)&constructor;
				}
			};
			template<typename T> Constructor<T> PersistencePolicy<T>::constructor;
		} // namespace cv
	} // namespace Persistence
} // namespace Parameters
#else
namespace Parameters
{
	namespace Persistence
	{
		namespace cv
		{
			template<typename T> class PersistencePolicy
			{
			public:
				PersistencePolicy()
				{
					
				}
			};
		}
	}
}

#endif