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

#ifdef OPENCV_FOUND
// Check if being built with OpenCV
#include <map>
#include <functional>
#include <string>
#include <vector>
#include <set>
#include <opencv2/core.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/function.hpp>

#include <parameters/LokiTypeInfo.h>
#include <parameters/Parameter_def.hpp>
#include <parameters/ITypedParameter.hpp>
#include <parameters/Types.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include "parameters/TypedParameter.hpp"

namespace Parameters
{
	class Parameter;
    template<typename T> class TypedParameter;

	namespace Persistence
	{
		namespace cv
		{

			class PARAMETER_EXPORTS InterpreterRegistry
			{
				
			public:
				typedef std::function<void(::cv::FileStorage*, Parameters::Parameter*)> SerializerFunction;
				typedef std::function<void(::cv::FileNode*, Parameters::Parameter*)> DeSerializerFunction;
                typedef std::function<Parameter*(::cv::FileNode*)> FactoryFunction;
				static void RegisterFunction(Loki::TypeInfo type, SerializerFunction serializer, DeSerializerFunction deserializer, FactoryFunction creator);
				static std::tuple<SerializerFunction, DeSerializerFunction, FactoryFunction >& GetInterpretingFunction(Loki::TypeInfo type);
                static std::tuple<SerializerFunction, DeSerializerFunction, FactoryFunction > GetInterpretingFunction(std::string type_string);
			private:
				// Mapping from Loki::typeinfo to file writing functors
				static	std::map<Loki::TypeInfo, std::tuple<SerializerFunction, DeSerializerFunction, FactoryFunction>>& registry();
			};
			PARAMETER_EXPORTS void Serialize(::cv::FileStorage* fs, Parameters::Parameter* param);
			PARAMETER_EXPORTS void DeSerialize(::cv::FileNode* fs, Parameters::Parameter* param);
			PARAMETER_EXPORTS Parameters::Parameter* DeSerialize(::cv::FileNode* fs);

			template<typename T, typename Enable = void> struct Serializer
			{
				static void Serialize(::cv::FileStorage* fs, T* param)
				{
					LOG_TRIVIAL(debug) << "Default non specialized serializer called for " << typeid(T).name();
					//std::cout << "Default non specialized serializer called for " << typeid(T).name();
				}
				template<typename U> static void DeSerialize(U fs, T* param)
				{
					LOG_TRIVIAL(debug) << "Default non specialized DeSerializer called for " << typeid(T).name();
				}
			};
			template<> struct PARAMETER_EXPORTS Serializer<char, void>
			{
				static void Serialize(::cv::FileStorage* fs, char* param);
				static void DeSerialize(::cv::FileNode& fs, char* param);
			};
			template<> struct PARAMETER_EXPORTS Serializer < std::string, void >
			{
				static void Serialize(::cv::FileStorage* fs, std::string* param);
				static void DeSerialize(::cv::FileNode& fs, std::string* param);
			};
			template<> struct PARAMETER_EXPORTS Serializer<::cv::Mat, void>
			{
				static void Serialize(::cv::FileStorage* fs, ::cv::Mat* param);
				static void DeSerialize(::cv::FileNode&  fs, ::cv::Mat* param);
			};
			template<> struct PARAMETER_EXPORTS Serializer<Parameters::EnumParameter, void>
			{
				static void Serialize(::cv::FileStorage* fs, Parameters::EnumParameter* param);
				static void DeSerialize(::cv::FileNode&  fs, Parameters::EnumParameter* param)
				{
					
					(fs)["Values"] >> param->values;

					auto end = (fs)["Enumerations"].end();
					param->enumerations.clear();
					for (auto itr = (fs)["Enumerations"].begin(); itr != end; ++itr)
					{
						param->enumerations.push_back((std::string)(*itr));
					}
					(fs)["Current value"] >> param->currentSelection;
				}
			};
			template<typename T> struct Serializer<T,typename  std::enable_if<std::is_unsigned<T>::value || std::is_same<T,long>::value || std::is_same<T, long long>::value, void>::type>
			{
				static void Serialize(::cv::FileStorage* fs, T* param)
				{
					
					(*fs) << boost::lexical_cast<std::string>(*param);
				}
				static void DeSerialize(::cv::FileNode& fs, T* param)
				{
					
					*param = boost::lexical_cast<T>((std::string)(fs));
				}
			}; 
			template<typename T> struct Serializer<T, typename std::enable_if<
				std::is_same<typename std::remove_cv<T>::type, Parameters::EnumParameter		>::value || 
				std::is_same<typename std::remove_cv<T>::type, ::cv::cuda::GpuMat				>::value ||
				std::is_same<typename std::remove_cv<T>::type, boost::function<void(void)>		>::value, void>::type>
			{
				static void Serialize(::cv::FileStorage* fs, T* param){}
				virtual void DeSerialize(::cv::FileNode& fs, T* param){}
			};
			template<typename T> struct Serializer < T, typename std::enable_if<
                std::is_floating_point<T>::value || (std::is_integral<T>::value && !std::is_unsigned<T>::value && !std::is_same<T, long>::value && !std::is_same<T, long long>::value), void>::type>
			{
				static void Serialize(::cv::FileStorage* fs, T* param)
				{
					
					(*fs) << *param;
				}
				static void DeSerialize(::cv::FileNode&  fs, T* param)
				{
					
					fs >> *param;
				}
			};
			template<typename T, int M, int N> struct Serializer<::cv::Matx<T, M, N>, void>
			{
				static void Serialize(::cv::FileStorage* fs, ::cv::Matx<T, M, N>* param)
				{
					
					(*fs) << "Rows" << M;
					(*fs) << "Cols" << N;
					(*fs) << "Elements" << "[";
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
				static void DeSerialize(::cv::FileNode& fs, ::cv::Matx<T, M, N>* param)
				{
					
					if ((int)(fs)["Rows"] != M || (int)(fs)["Cols"] != N)
						return;
					auto rowItr = (fs)["Elements"].begin();
					for (int i = 0; i < M; ++i, ++rowItr)
					{
						auto colItr = (*rowItr).begin();
						for (int j = 0; j < N; ++j, ++colItr)
						{
							(*param)(i, j) = (double)(*colItr);
						}
					}
				}

			};
			template<typename T, int M> struct Serializer<::cv::Vec<T, M>, void> : public Serializer<::cv::Matx<T,M,1>>{};
			template<typename T> struct Serializer<::cv::Scalar_<T>, void> : public Serializer<::cv::Vec<T, 4>>{};
			// TODO, this doesn't work for types like std::vector<unsigned int> because opencv doesn't natively support it.  Instead needs to be modified to manually handle serialization
			template<typename T> struct Serializer<std::vector<T>, void>: public Serializer<T>
			{
				static void Serialize(::cv::FileStorage* fs, std::vector<T>* param)
				{
					
                    (*fs) << "{";
                    (*fs) << "Size" << boost::lexical_cast<std::string>(param->size());
					(*fs) << "Elements" << "[";
					for (auto itr = param->begin(); itr != param->end(); ++itr)
					{
						Serializer<T>::Serialize(fs, &(*itr));
					}
					(*fs) << "]";
                    (*fs) << "}";
					
				}
				static void DeSerialize(::cv::FileNode&  fs, std::vector<T>* param)
				{
					
                    size_t size = boost::lexical_cast<size_t>((std::string)fs["Size"]);
                    param->reserve(size);
					auto element_node = fs["Elements"];

					for (auto itr = element_node.begin(); itr != element_node.end(); ++itr)
					{
						param->push_back(T());
                        auto node = *itr;
                        Serializer<T>::DeSerialize(node, &(*param)[param->size() - 1]);
					}
				}

			};
            template<typename T> struct Serializer<std::set<T>, void> : public Serializer<T>
            {
                static void Serialize(::cv::FileStorage* fs, std::set<T>* param)
                {

                    (*fs) << "{";
                    (*fs) << "Size" << boost::lexical_cast<std::string>(param->size());
                    (*fs) << "Elements" << "[";
                    for (auto itr = param->begin(); itr != param->end(); ++itr)
                    {
                        T data = *itr;
                        Serializer<T>::Serialize(fs, &data);
                    }
                    (*fs) << "]";
                    (*fs) << "}";

                }
                static void DeSerialize(::cv::FileNode&  fs, std::set<T>* param)
                {
                    auto element_node = fs["Elements"];

                    for (auto itr = element_node.begin(); itr != element_node.end(); ++itr)
                    {
                        T temp;
                        auto node = *itr;
                        Serializer<T>::DeSerialize(node, &temp);
                        param->insert(temp);
                    }
                }

            };

			template<typename T> struct SerializeWrapper
			{
				static void Write(::cv::FileStorage* fs, Parameter* param)
				{
					LOG_TRIVIAL(trace) << "Writing parameter with name " << param->GetName();
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
							LOG_TRIVIAL(debug) << "Invalid tree name for parameter: " << typedParam->GetTreeName() << " " << e.what();
							return;
						}						
                        (*fs) << "Data";
						serializer.Serialize(fs, typedParam->Data());
						(*fs) << "Type" << typedParam->GetTypeInfo().name();
						if (toolTip.size())
							(*fs) << "ToolTip" << toolTip;
						(*fs) << "}";
					}
				}
				static void Read(::cv::FileNode* fs, Parameter* param)
				{
					LOG_TRIVIAL(trace) << "Reading parameter with name " << param->GetName();
					ITypedParameter<T>* typedParam = dynamic_cast<ITypedParameter<T>*>(param);
					Serializer<T> serializer;
					if (typedParam)
					{
						if (fs->name() == param->GetName())
						{
							std::string type = (std::string)(*fs)["Type"];
							if (type == param->GetTypeInfo().name())
							{
                                auto node = (*fs)["Data"];
                                serializer.DeSerialize(node, typedParam->Data());
                                typedParam->changed = true;
								typedParam->OnUpdate(nullptr);
								LOG_TRIVIAL(trace) << "Successfully read " << param->GetName();
							}
							else
							{
								LOG_TRIVIAL(debug) << "Datatype " + std::string(param->GetTypeInfo().name()) + " requested, but " + type + " found in file";
								::cv::error(::cv::Error::StsAssert, "Datatype " + std::string(param->GetTypeInfo().name()) + " requested, but " + type + " found in file", CV_Func, __FILE__, __LINE__);
							}								
						}
						else
						{
							::cv::FileNode myNode = (*fs)[param->GetName()];
							std::string type = (std::string)myNode["Type"];
							if (type == param->GetTypeInfo().name())
							{
                                auto node = myNode["Data"];
                                serializer.DeSerialize(node, typedParam->Data());
                                typedParam->changed = true;
								typedParam->OnUpdate(nullptr);
								LOG_TRIVIAL(trace) << "Successfully read " << param->GetName();
							}
							else
							{
								LOG_TRIVIAL(debug) << "Datatype " + std::string(param->GetTypeInfo().name()) + " requested, but " + type + " found in file";
								::cv::error(::cv::Error::StsAssert, "Datatype " + std::string(param->GetTypeInfo().name()) + " requested, but " + type + " found in file", CV_Func, __FILE__, __LINE__);
							}	
						}
					}
				}
			    static Parameter* create(::cv::FileNode* fs)
                {
                    auto output = new TypedParameter<T>(fs->name(), T());
                    Read(fs, output);
                    return output;
                }
            };

			template<typename T> class Constructor
			{
			public:
				Constructor()
				{
					InterpreterRegistry::RegisterFunction(Loki::TypeInfo(typeid(T)), 
                        std::bind(SerializeWrapper<T>::Write, std::placeholders::_1, std::placeholders::_2), 
                        std::bind(SerializeWrapper<T>::Read, std::placeholders::_1, std::placeholders::_2), 
                        std::bind(SerializeWrapper<T>::create, std::placeholders::_1));
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
