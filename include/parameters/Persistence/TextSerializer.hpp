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

#include <map>
#include <functional>
#include <string>

#include <parameters/LokiTypeInfo.h>
#include <parameters/Parameter_def.hpp>
#include "parameters/TypedParameter.hpp"
#include <parameters/Types.hpp>

#include <boost/lexical_cast.hpp>

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
    template<typename T> class TypedParameter;
    namespace Persistence
    {
        namespace Text
        {
            class PARAMETER_EXPORTS InterpreterRegistry
            {
            public:
                typedef std::function<bool(::std::stringstream*, Parameters::Parameter*)> SerializerFunction;
                typedef std::function<bool(::std::stringstream*, Parameters::Parameter*)> SSDeSerializerFunction;
                typedef std::function<bool(::std::string*, Parameters::Parameter*)> DeSerializerFunction;
                typedef std::function<Parameters::Parameter::Ptr(const ::std::string&)> FactoryFunction;
                typedef std::tuple<SerializerFunction, SerializerFunction, SSDeSerializerFunction, DeSerializerFunction, bool> InterpreterSet;
                static void RegisterFunction(Loki::TypeInfo type, SerializerFunction serializer, SerializerFunction serializeValue, SSDeSerializerFunction ssdeserializer, DeSerializerFunction deserializer, FactoryFunction factory, bool is_default);
                static InterpreterSet& GetInterpretingFunction(Loki::TypeInfo type);

                // Return true if a properly formed serialization function exists for this type
                static bool Exists(Loki::TypeInfo type);
            private:
                // Mapping from Loki::typeinfo to file writing functors
				static std::map<Loki::TypeInfo, InterpreterSet>& registry();
				static std::map<std::string, FactoryFunction>& factory();
				friend PARAMETER_EXPORTS Parameters::Parameter::Ptr DeSerialize(::std::string* ss);
				friend PARAMETER_EXPORTS Parameters::Parameter::Ptr DeSerialize(::std::stringstream* ss);
				friend PARAMETER_EXPORTS bool DeSerialize(::std::string* ss, Parameters::Parameter* param);
				friend PARAMETER_EXPORTS bool DeSerialize(::std::stringstream* ss, Parameters::Parameter* param);
				friend PARAMETER_EXPORTS bool Serialize(::std::stringstream* ss, Parameters::Parameter* param);
            };
            PARAMETER_EXPORTS bool Serialize(::std::stringstream* ss, Parameters::Parameter* param);
            PARAMETER_EXPORTS bool DeSerialize(::std::stringstream* ss, Parameters::Parameter* param);
            PARAMETER_EXPORTS bool DeSerialize(::std::string* ss, Parameters::Parameter* param);
			PARAMETER_EXPORTS Parameters::Parameter::Ptr DeSerialize(::std::stringstream* ss);
			PARAMETER_EXPORTS Parameters::Parameter::Ptr DeSerialize(::std::string* ss);
            PARAMETER_EXPORTS bool SerializeValue(::std::stringstream* ss, Parameters::Parameter* param);

            template<typename T, typename Enable = void> struct Serializer
            {
                static bool Serialize(::std::stringstream* ss, T* param)
                {
                    return false;
                }
                static bool DeSerialize(::std::stringstream* ss, T* param)
                {
                    return false;
                }
                static bool DeSerialize(::std::string* ss, T* param)
                {
                    return false;
                }
                const static bool IS_DEFAULT = true;
            };
			template<typename T> struct Serializer<T, typename std::enable_if<
				std::is_same<T, Parameters::ReadDirectory>::value ||
				std::is_same<T, Parameters::ReadFile>::value ||
				std::is_same<T, Parameters::WriteDirectory>::value ||
				std::is_same<T, Parameters::WriteFile>::value>::type>
			{
				static bool Serialize(::std::stringstream* ss, T* param)
				{
					(*ss) << param->string();
                    return true;
				}
				static bool DeSerialize(::std::stringstream* ss, T* param)
				{
					std::string line;
					std::getline(*ss, line);
					*param = T(line);
                    return true;
				}
				static bool DeSerialize(::std::string* str, T* param)
				{
					*param = T(*str);
					return true;
				}
                const static bool IS_DEFAULT = false;
			};
            template<typename T> struct Serializer<T, typename std::enable_if<
                std::is_floating_point<T>::value || 
                std::is_integral<T>::value ||
                std::is_same<T, bool>::value ||
                std::is_same<T, std::string>::value
            >::type>
            {
                static bool Serialize(::std::stringstream* ss, T* param)
                {
                    (*ss) << *param;
                    return true;
                }
                static bool DeSerialize(::std::stringstream* ss, T* param)
                {
                    std::string line;
					std::getline(*ss, line);
					try
					{
						*param = boost::lexical_cast<T>(line);
					}catch(...)
					{
						return false;
					}
                    return true;
                }
				static bool DeSerialize(::std::string* str, T* param)
				{
                    try
                    {
                        *param = boost::lexical_cast<T>(*str);
                    }catch(...)
					{
						return false;
					}
					return true;
				}
                const static bool IS_DEFAULT = false;
            };

            template<typename T> struct Serializer<T, typename std::enable_if<
                std::is_class<T>::value && 
                !is_vector<T>::value && 
                !std::is_same<T, std::string>::value
            >::type>
            {
                static bool Serialize(::std::stringstream* ss, T* param)
                {
                    return false;
                }
                static bool DeSerialize(::std::stringstream* ss, T* param)
                {
                    return false;
                }
                static bool DeSerialize(::std::string* ss, T* param)
                {
                    return false;
                }
                const static bool IS_DEFAULT = true;
            };
            template<> struct PARAMETER_EXPORTS Serializer<EnumParameter, void>
            {
                static bool Serialize(::std::stringstream* ss, EnumParameter* param);
                static bool DeSerialize(::std::stringstream* ss, EnumParameter* param);
                static bool DeSerialize(::std::string* ss, EnumParameter* param);
                const static bool IS_DEFAULT = false;
            };
            template<> struct PARAMETER_EXPORTS Serializer<::cv::cuda::GpuMat, void>
            {
                static bool Serialize(::std::stringstream* ss, ::cv::cuda::GpuMat* param);
                static bool DeSerialize(::std::stringstream* ss, ::cv::cuda::GpuMat* param);
                static bool DeSerialize(::std::string* ss, ::cv::cuda::GpuMat* param);
                const static bool IS_DEFAULT = true;
            };
            template<typename T> struct Serializer<std::vector<T>, void>: public Serializer<T>
            {
                static bool Serialize(std::stringstream* ss, std::vector<T>* param)
                {
                    
                    (*ss) << "<" << param->size() << ">";
                    for (int i = 0; i < param->size(); ++i)
                    {
                        if (i != 0)
                            (*ss) << ",";
                        if(!Serializer<T>::Serialize(ss, &(*param)[i]))
                            return false;
                    }
                    return true;
                }
                static bool DeSerialize(::std::stringstream* ss, std::vector<T>* param)
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
                    return true;
                }
                static bool DeSerialize(::std::string* ss, std::vector<T>* param)
                {
					return true;
                }
                const static bool IS_DEFAULT = false;
            };


            template<typename T> struct SerializeWrapper
            {
                static bool Write(::std::stringstream* ss, Parameter* param)
                {
                    LOG_TRIVIAL(trace) << "Writing parameter with name " << param->GetName();
                    ITypedParameter<T>* typedParam = dynamic_cast<ITypedParameter<T>*>(param);
                    
                    if (typedParam && typedParam->Data())
                    {
                        (*ss) << "[" << param->GetTypeInfo().name() << "]";
						(*ss) << param->GetTreeName() << "#";
                        if(!Serializer<T>::Serialize(ss, typedParam->Data()))
                            return false;
                        (*ss) << "\n";
                    }
                    return true;
                }
                static bool WriteValue(::std::stringstream* ss, Parameter* param)
                {
                    ITypedParameter<T>* typedParam = dynamic_cast<ITypedParameter<T>*>(param);
                    if(typedParam && typedParam->Data())
                    {
                        if(!Serializer<T>::Serialize(ss, typedParam->Data()))
                            return false;
                    }
                    return true;
                }
                static bool ssRead(::std::stringstream* ss, Parameter* param)
                {
					LOG_TRIVIAL(trace) << "Reading parameter with name " << param->GetName();
                    ITypedParameter<T>* typedParam = dynamic_cast<ITypedParameter<T>*>(param);
                    if (typedParam && typedParam->Data())
                    {
						if(Serializer<T>::DeSerialize(ss, typedParam->Data()))
                        {
                            typedParam->changed = true;
                            typedParam->OnUpdate(nullptr);
							return true;
                        }
                    }
					return false;
                }
                static bool Read(::std::string* ss, Parameter* param)
                {
					LOG_TRIVIAL(trace) << "Reading parameter with name " << param->GetName();
                    ITypedParameter<T>* typedParam = dynamic_cast<ITypedParameter<T>*>(param);
                    if (typedParam)
                    {
						if(Serializer<T>::DeSerialize(ss, typedParam->Data()))
                        {
                            typedParam->changed = true;
                            typedParam->OnUpdate(nullptr);
							return true;
                        }
                    }
					return false;
                }
                const static bool IS_DEFAULT = Serializer<T>::IS_DEFAULT;
            };

            template<typename T> struct ParameterFactory
            {
				ParameterFactory()
				{
					InterpreterRegistry::RegisterFunction(Loki::TypeInfo(typeid(T)),
						std::bind(SerializeWrapper<T>::Write, std::placeholders::_1, std::placeholders::_2),
                        std::bind(SerializeWrapper<T>::WriteValue, std::placeholders::_1, std::placeholders::_2),
						std::bind(SerializeWrapper<T>::ssRead, std::placeholders::_1, std::placeholders::_2),
						std::bind(SerializeWrapper<T>::Read, std::placeholders::_1, std::placeholders::_2),
						std::bind(ParameterFactory<T>::create, std::placeholders::_1),
                        SerializeWrapper<T>::IS_DEFAULT);
				}
                static Parameters::Parameter::Ptr create(const std::string& name)
                {
                    //return Parameters::Parameter::Ptr(new Parameters::TypedParameter<T>(name));
                    return Parameters::Parameter::Ptr(new TypedParameter<T>(name));
                }
            };

            template<typename T> class PersistencePolicy
            {
				static ParameterFactory<T> factory;
            public:
                PersistencePolicy()
                {
					(void)&factory;
                }
            };
			template<typename T> ParameterFactory<T> PersistencePolicy<T>::factory;
        }
    }
}

