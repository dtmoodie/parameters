#include "Parameters.hpp"
#include "Persistence/TextSerializer.hpp"

using namespace Parameters::Persistence::Text;

std::map<Loki::TypeInfo, std::pair<InterpreterRegistry::SerializerFunction, InterpreterRegistry::DeSerializerFunction >>& InterpreterRegistry::registry()
{
    static std::map<Loki::TypeInfo, std::pair<InterpreterRegistry::SerializerFunction, InterpreterRegistry::DeSerializerFunction >> registry_instance;
    return registry_instance;
}
void InterpreterRegistry::RegisterFunction(Loki::TypeInfo type, SerializerFunction serializer, DeSerializerFunction deserializer)
{
    LOG_TRACE;
    registry()[type] = std::make_pair(serializer, deserializer);
}

std::pair<InterpreterRegistry::SerializerFunction, InterpreterRegistry::DeSerializerFunction >& InterpreterRegistry::GetInterpretingFunction(Loki::TypeInfo type)
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
    InterpreterRegistry::GetInterpretingFunction(param->GetTypeInfo()).first(ss, param);
}

void Parameters::Persistence::Text::DeSerialize(::std::stringstream* ss, Parameters::Parameter* param)
{
    LOG_TRACE;
    InterpreterRegistry::GetInterpretingFunction(param->GetTypeInfo()).second(ss, param);
}

Parameters::Parameter* DeSerialize(::std::stringstream* ss)
{
    LOG_TRACE;
    //TODO object factory based on serialized type
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

void Parameters::Persistence::Text::Serializer<::cv::cuda::GpuMat, void>::Serialize(::std::stringstream* ss, ::cv::cuda::GpuMat* param)
{

}
void Parameters::Persistence::Text::Serializer<::cv::cuda::GpuMat, void>::DeSerialize(::std::stringstream* ss, ::cv::cuda::GpuMat* param)
{

}