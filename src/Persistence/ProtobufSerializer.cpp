

#include "parameters/Persistence/ProtobufSerializer.hpp"

using namespace Parameters;
using namespace Parameters::Persistence;
using namespace Parameters::Persistence::Protobuf;

SerializationFactory* SerializationFactory::instance()
{
    static SerializationFactory g_inst;
    return &g_inst;
}

void SerializationFactory::register_serializer(Loki::TypeInfo type, serializer serializer_)
{
    _registered_serializers[type] = serializer_;
}

void SerializationFactory::register_deserializer(Loki::TypeInfo type, deserializer deserializer_)
{
    _registered_deserializers[type] = deserializer_;
}

void SerializationFactory::register_both(Loki::TypeInfo type, serializer serializer_, deserializer deserializer_)
{
    _registered_serializers[type] = serializer_;
    _registered_deserializers[type] = deserializer_;
}

SerializationFactory::SerializationFactory()
{

}

SerializationFactory::SerializationFactory(const SerializationFactory& other)
{

}
