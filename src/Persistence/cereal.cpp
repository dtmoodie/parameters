#include "parameters/Persistence/cereal.hpp"

using namespace Parameters;
using namespace Parameters::Cereal;
using namespace cereal;

Serializer::Serializer()
{
}

Serializer* Serializer::instance()
{
    static Serializer inst;
    return &inst;
}
void Serializer::register_binary_serializer(serialize_binary_f save, deserialize_binary_f load, Loki::TypeInfo type)
{
    _binary_serializers[type] = std::make_pair(save, load);
}
void Serializer::register_xml_serializer(serialize_xml_f save, deserialize_xml_f load,  Loki::TypeInfo type)
{
    _xml_serializers[type] = std::make_pair(save, load);
}
void Serializer::register_json_serializer(serialize_json_f save, deserialize_json_f load, Loki::TypeInfo type)
{
    _json_serializers[type] = std::make_pair(save, load);
}
void Serializer::Serialize(cereal::BinaryOutputArchive& ar, Parameter* param)
{
    auto itr = _binary_serializers.find(Loki::TypeInfo(typeid(*param)));
    ar(cereal::make_nvp("type", std::string(typeid(*param).name())));
    if(itr != _binary_serializers.end())
    {
        itr->second.second(ar, param);
    }
}
void Serializer::Serialize(cereal::XMLOutputArchive& ar, Parameter* param)
{
    auto itr = _xml_serializers.find(Loki::TypeInfo(typeid(*param)));
    ar(cereal::make_nvp("type", std::string(typeid(*param).name())));
    if(itr != _xml_serializers.end())
    {
        itr->second.second(ar, param);
    }
}
void Serializer::Serialize(cereal::JSONOutputArchive& ar, Parameter* param)
{
    auto itr = _json_serializers.find(Loki::TypeInfo(typeid(*param)));
    ar(cereal::make_nvp("type", std::string(typeid(*param).name())));
    if(itr != _json_serializers.end())
    {
        itr->second.second(ar, param);
    }
}

void Serializer::Deserialize(cereal::BinaryInputArchive& ar, Parameter* param)
{
    auto itr = _binary_serializers.find(Loki::TypeInfo(typeid(*param)));
    
    if(itr != _binary_serializers.end())
    {
        itr->second.first(ar, param);
    }
}
void Serializer::Deserialize(cereal::XMLInputArchive& ar, Parameter* param)
{
    auto itr = _xml_serializers.find(Loki::TypeInfo(typeid(*param)));
    
    if(itr != _xml_serializers.end())
    {
        itr->second.first(ar, param);
    }
}
void Serializer::Deserialize(cereal::JSONInputArchive& ar, Parameter* param)
{
    auto itr = _json_serializers.find(Loki::TypeInfo(typeid(*param)));
    if(itr != _json_serializers.end())
    {
        itr->second.first(ar, param);
    }
}

Parameter* Serializer::Deserialize(cereal::BinaryInputArchive& ar)
{
    return nullptr;
}
Parameter* Serializer::Deserialize(cereal::XMLInputArchive& ar)
{
    return nullptr;
}
Parameter* Serializer::Deserialize(cereal::JSONInputArchive& ar)
{
    return nullptr;
}
#ifdef HAVE_CEREAL
void cereal::load(cereal::BinaryInputArchive& ar, cv::Mat& mat)
{
    int channels, rows, cols, depth;
    ar(rows);
    ar(cols);
    ar(channels);
    ar(depth);            
    mat.create(rows, cols, CV_MAKETYPE(depth, channels));
    ar.loadBinary(mat.data, mat.dataend - mat.datastart);
}
void cereal::save(cereal::BinaryOutputArchive ar, cv::Mat const & mat)
{
    int channels = mat.channels();
    int depth = mat.depth();
    ar(mat.rows, mat.cols, channels, depth);
    ar.saveBinary(mat.data, mat.dataend - mat.datastart);
}
void cereal::load(cereal::BinaryInputArchive& ar, cv::cuda::HostMem& mat)
{
    int channels, rows, cols, depth;
    ar(rows);
    ar(cols);
    ar(channels);
    ar(depth);
    mat.create(rows, cols, CV_MAKETYPE(depth, channels));
    ar.loadBinary(mat.data, mat.dataend - mat.datastart);
}
#endif