#pragma once
#include "parameters/Parameter_def.hpp"
#include "parameters/LokiTypeInfo.h"

#include <utility>
#include <map>
namespace cereal
{
    class BinaryInputArchive;
    class XMLInputArchive;
    class JSONInputArchive;
    class XMLOutputArchive;
    class BinaryOutputArchive;
    class JSONOutputArchive;
}

namespace Parameters
{
    class Parameter;
    namespace Persistence
    {
        namespace Cereal
        {
            class PARAMETER_EXPORTS Serializer
            {
                Serializer();
            public:
                static Serializer* instance();

                void Serialize(cereal::BinaryOutputArchive& ar, Parameter* param);
                void Serialize(cereal::XMLOutputArchive& ar, Parameter* param);
                void Serialize(cereal::JSONOutputArchive& ar, Parameter* param);
            
                void Deserialize(cereal::BinaryInputArchive& ar, Parameter* param);
                void Deserialize(cereal::XMLInputArchive& ar, Parameter* param);
                void Deserialize(cereal::JSONInputArchive& ar, Parameter* param);

                Parameter* Deserialize(cereal::BinaryInputArchive& ar);
                Parameter* Deserialize(cereal::XMLInputArchive& ar);
                Parameter* Deserialize(cereal::JSONInputArchive& ar);
            
                typedef std::function<void(cereal::BinaryInputArchive& ar, Parameter*)> serialize_binary_f;
                typedef std::function<void(cereal::XMLInputArchive& ar, Parameter*)> serialize_xml_f;
                typedef std::function<void(cereal::JSONInputArchive& ar, Parameter*)> serialize_json_f;

                typedef std::function<void(cereal::BinaryOutputArchive& ar, Parameter*)> deserialize_binary_f;
                typedef std::function<void(cereal::XMLOutputArchive& ar, Parameter*)> deserialize_xml_f;
                typedef std::function<void(cereal::JSONOutputArchive& ar, Parameter*)> deserialize_json_f;

                void register_binary_serializer(serialize_binary_f save, deserialize_binary_f load, Loki::TypeInfo type);
                void register_xml_serializer(serialize_xml_f save, deserialize_xml_f load,  Loki::TypeInfo type);
                void register_json_serializer(serialize_json_f save, deserialize_json_f load, Loki::TypeInfo type);
            private:
                std::map<Loki::TypeInfo, std::pair<serialize_binary_f, deserialize_binary_f>> _binary_serializers;
                std::map<Loki::TypeInfo, std::pair<serialize_xml_f, deserialize_xml_f>> _xml_serializers;
                std::map<Loki::TypeInfo, std::pair<serialize_json_f, deserialize_json_f>> _json_serializers;
            };
        } // namespace cereal
    }
} // namespace parameters


