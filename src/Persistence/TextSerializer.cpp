#include "Parameters.hpp"
#include "Persistence/TextSerializer.hpp"

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
    LOG_TRACE;
    registry()[type] = std::make_tuple(serializer, ssdeserializer, deserializer);
    factory()[type.name()] = factoryFunc;
}

InterpreterRegistry::InterpreterSet& InterpreterRegistry::GetInterpretingFunction(Loki::TypeInfo type)
{
    LOG_TRACE;
    if (registry().find(type) == registry().end())
    {
        LOG_TRIVIAL(warning) << type.name() << " not registered to the registry";
        throw std::exception("Datatype not registered to the registry", 0);
    }
    return registry()[type];
}


void Parameters::Persistence::Text::Serialize(::std::stringstream* ss, Parameters::Parameter* param)
{
    LOG_TRACE;
    std::get<0>(InterpreterRegistry::GetInterpretingFunction(param->GetTypeInfo())).operator()(ss, param);
    //InterpreterRegistry::GetInterpretingFunction(param->GetTypeInfo()).first(ss, param);
}

void Parameters::Persistence::Text::DeSerialize(::std::stringstream* ss, Parameters::Parameter* param)
{
    LOG_TRACE;
    std::get<1>(InterpreterRegistry::GetInterpretingFunction(param->GetTypeInfo())).operator()(ss, param);
}
void Parameters::Persistence::Text::DeSerialize(::std::string* ss, Parameters::Parameter* param)
{
    LOG_TRACE;
    std::get<2>(InterpreterRegistry::GetInterpretingFunction(param->GetTypeInfo())).operator()(ss, param);
}

Parameters::Parameter* DeSerialize(::std::stringstream* ss)
{
    LOG_TRACE;
    //TODO object factory based on serialized type
    std::string line;
    (*ss) >> line;

    return nullptr;
}

void Serializer<Parameters::EnumParameter, void>::Serialize(::std::stringstream* ss, Parameters::EnumParameter* param)
{
    LOG_TRACE;
    (*ss) << "Values:";
    for (int i = 0; i < param->enumerations.size(); ++i)
    {
        (*ss) << "(" << param->values[i] << "," << param->enumerations[i] << ")";
    }
}

void Serializer<Parameters::EnumParameter, void>::DeSerialize(::std::stringstream* ss, Parameters::EnumParameter* param)
{

}

void Serializer<Parameters::EnumParameter, void>::DeSerialize(::std::string* ss, Parameters::EnumParameter* param)
{

}

void Parameters::Persistence::Text::Serializer<::cv::cuda::GpuMat, void>::Serialize(::std::stringstream* ss, ::cv::cuda::GpuMat* param)
{

}
void Parameters::Persistence::Text::Serializer<::cv::cuda::GpuMat, void>::DeSerialize(::std::stringstream* ss, ::cv::cuda::GpuMat* param)
{

}
void Parameters::Persistence::Text::Serializer<::cv::cuda::GpuMat, void>::DeSerialize(::std::string* ss, ::cv::cuda::GpuMat* param)
{

}