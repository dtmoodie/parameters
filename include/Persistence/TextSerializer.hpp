#pragma once

#include <map>
#include <functional>
#include <string>

#include <LokiTypeInfo.h>
#include <Parameter_def.hpp>
#include <Types.hpp>
#include <TypedParameter.hpp>
template<typename T> 
using is_vector = std::is_same<T, std::vector< typename T::value_type, typename T::allocator_type > >;

namespace cv
{
    namespace cuda
    {
        class GpuMat;
    }
}
namespace Parameters
{
	class Parameter;
    namespace Persistence
    {
        namespace Text
        {
            class Parameter_EXPORTS InterpreterRegistry
            {
            public:
                typedef std::function<void(::std::stringstream*, Parameters::Parameter*)> SerializerFunction;
                typedef std::function<void(::std::stringstream*, Parameters::Parameter*)> SSDeSerializerFunction;
                typedef std::function<void(::std::string*, Parameters::Parameter*)> DeSerializerFunction;
                typedef std::function<Parameters::Parameter::Ptr(const ::std::string&)> FactoryFunction;
                typedef std::tuple<SerializerFunction, SSDeSerializerFunction, DeSerializerFunction> InterpreterSet;
                static void RegisterFunction(Loki::TypeInfo type, SerializerFunction serializer, SSDeSerializerFunction ssdeserializer, DeSerializerFunction deserializer, FactoryFunction factory);
                static InterpreterSet& GetInterpretingFunction(Loki::TypeInfo type);

            private:
                // Mapping from Loki::typeinfo to file writing functors
				static std::map<Loki::TypeInfo, InterpreterSet>& registry();
				static std::map<std::string, FactoryFunction>& factory();
				friend Parameter_EXPORTS Parameters::Parameter::Ptr DeSerialize(::std::string* ss);
				friend Parameter_EXPORTS Parameters::Parameter::Ptr DeSerialize(::std::stringstream* ss);
				friend Parameter_EXPORTS void DeSerialize(::std::string* ss, Parameters::Parameter* param);
				friend Parameter_EXPORTS void DeSerialize(::std::stringstream* ss, Parameters::Parameter* param);
				friend Parameter_EXPORTS void Serialize(::std::stringstream* ss, Parameters::Parameter* param);
            };
            Parameter_EXPORTS void Serialize(::std::stringstream* ss, Parameters::Parameter* param);
            Parameter_EXPORTS void DeSerialize(::std::stringstream* ss, Parameters::Parameter* param);
            Parameter_EXPORTS void DeSerialize(::std::string* ss, Parameters::Parameter* param);
			Parameter_EXPORTS Parameters::Parameter::Ptr DeSerialize(::std::stringstream* ss);
			Parameter_EXPORTS Parameters::Parameter::Ptr DeSerialize(::std::string* ss);

            template<typename T, typename Enable = void> struct Serializer
            {
                static void Serialize(::std::stringstream* ss, T* param)
                {
                }
                static void DeSerialize(::std::stringstream* ss, T* param)
                {
                }
                static void DeSerialize(::std::string* ss, T* param)
                {
                }
            };
            template<typename T> struct Serializer<T, typename std::enable_if<
                std::is_floating_point<T>::value || 
                std::is_integral<T>::value ||
                std::is_same<T, bool>::value ||
                std::is_same<T, std::string>::value
            >::type>
            {
                static void Serialize(::std::stringstream* ss, T* param)
                {
                    (*ss) << *param;
                }
                static void DeSerialize(::std::stringstream* ss, T* param)
                {
                    std::string line;
					std::getline(*ss, line);
                    *param = boost::lexical_cast<T>(line);
                }
				static void DeSerialize(::std::string* str, T* param)
				{
					*param = boost::lexical_cast<T>(*str);
					return;
				}
            };

            template<typename T> struct Serializer<T, typename std::enable_if<
                std::is_class<T>::value && 
                !is_vector<T>::value && 
                !std::is_same<T, std::string>::value
            >::type>
            {
                static void Serialize(::std::stringstream* ss, T* param)
                {
                }
                static void DeSerialize(::std::stringstream* ss, T* param)
                {
                }
                static void DeSerialize(::std::string* ss, T* param)
                {
                }
            };
            template<> struct Parameter_EXPORTS Serializer<EnumParameter, void>
            {
                static void Serialize(::std::stringstream* ss, EnumParameter* param);
                static void DeSerialize(::std::stringstream* ss, EnumParameter* param);
                static void DeSerialize(::std::string* ss, EnumParameter* param);
            };
            template<> struct Parameter_EXPORTS Serializer<::cv::cuda::GpuMat, void>
            {
                static void Serialize(::std::stringstream* ss, ::cv::cuda::GpuMat* param);
                static void DeSerialize(::std::stringstream* ss, ::cv::cuda::GpuMat* param);
                static void DeSerialize(::std::string* ss, ::cv::cuda::GpuMat* param);
            };
            template<typename T> struct Serializer<std::vector<T>, void>: public Serializer<T>
            {
                static void Serialize(std::stringstream* ss, std::vector<T>* param)
                {
                    LOG_TRACE;
                    (*ss) << "<" << param->size() << ">";
                    for (int i = 0; i < param->size(); ++i)
                    {
                        if (i != 0)
                            (*ss) << ",";
                        Serializer<T>::Serialize(ss, &(*param)[i]);
                    }
                }
                static void DeSerialize(::std::stringstream* ss, std::vector<T>* param)
                {
					std::string len;
					std::getline(*ss, len, '>');
					const int size = boost::lexical_cast<size_t>(len.substr(1));
					param->resize(size);
					int i;
					for (i = 0; i < size - 1; ++i)
					{
						std::getline(*ss, len, ',');
						Serializer<T>::DeSerialize(&len, &(*param)[i]);
					}
					std::getline(*ss, len);
					Serializer<T>::DeSerialize(&len, &(*param)[i]);
                }
                static void DeSerialize(::std::string* ss, std::vector<T>* param)
                {
					
                }
            };


            template<typename T> struct SerializeWrapper
            {
                static void Write(::std::stringstream* ss, Parameter* param)
                {
                    LOG_TRIVIAL(info) << "Writing parameter with name " << param->GetName();
                    ITypedParameter<T>* typedParam = dynamic_cast<ITypedParameter<T>*>(param);
                    
                    if (typedParam)
                    {
                        (*ss) << "[" << param->GetTypeInfo().name() << "]";
						(*ss) << param->GetTreeName() << "#";
                        Serializer<T>::Serialize(ss, typedParam->Data());
                        (*ss) << "\n";
                    }
                }
                static void ssRead(::std::stringstream* ss, Parameter* param)
                {
                    LOG_TRIVIAL(info) << "Reading parameter with name " << param->GetName();
                    ITypedParameter<T>* typedParam = dynamic_cast<ITypedParameter<T>*>(param);
                    if (typedParam)
                    {
						Serializer<T>::DeSerialize(ss, typedParam->Data());
                    }
                }
                static void Read(::std::string* ss, Parameter* param)
                {
                    LOG_TRIVIAL(info) << "Reading parameter with name " << param->GetName();
                    ITypedParameter<T>* typedParam = dynamic_cast<ITypedParameter<T>*>(param);
                    if (typedParam)
                    {
						Serializer<T>::DeSerialize(ss, typedParam->Data());
                    }
                }
            };
            template<typename T> struct ParameterFactory
            {
                static Parameters::Parameter::Ptr create(const std::string& name)
                {
                    return Parameters::Parameter::Ptr(new Parameters::TypedParameter<T>(name));
                }
            };
            template<typename T> class PersistencePolicy
            {
            public:
                PersistencePolicy()
                {
                    InterpreterRegistry::RegisterFunction(Loki::TypeInfo(typeid(T)), 
                        std::bind(SerializeWrapper<T>::Write, std::placeholders::_1, std::placeholders::_2), 
                        std::bind(SerializeWrapper<T>::ssRead,  std::placeholders::_1, std::placeholders::_2),
                        std::bind(SerializeWrapper<T>::Read, std::placeholders::_1, std::placeholders::_2),
                        std::bind(ParameterFactory<T>::create, std::placeholders::_1));
                }
            };
        }
    }
}

