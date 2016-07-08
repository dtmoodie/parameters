#pragma once
#include "parameters/Parameter_def.hpp"
#include "parameters/LokiTypeInfo.h"
#include "parameters/Types.hpp"
#include "Signals/signaler.h"

#ifdef HAVE_CEREAL
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/tuple.hpp>
#include <cereal/types/map.hpp>
#endif

#ifdef HAVE_OPENCV
#include <opencv2/core/types.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/cuda.hpp>
#endif

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
    template<class A, class T1, class T2> void serialize(A& ar, std::pair<T1, T2>& pair)
    {
        ar(pair.first);
        ar(pair.second);
    }
    template<class A> void serialize(A& ar, Parameters::ReadDirectory& m)
    {
        ar(m);
    }
    template<class A> void serialize(A& ar, Parameters::ReadFile& m)
    {
        ar(m);
    }
    template<class A> void serialize(A& ar, Parameters::WriteFile& m)
    {
        ar(m);
    }
    template<class A> void serialize(A& ar, Parameters::WriteDirectory& m)
    {
        ar(m);
    }
    template<class A> void save(A& ar, Parameters::EnumParameter const& m)
    {
        size_t num_elems = m.values.size();
        ar(num_elems);
        ar(m.currentSelection);
        for(int i = 0; i < num_elems; ++i)
        {
            ar(m.enumerations[i], m.values[i]);
        }
    }
    template<class A> void load(A& ar, Parameters::EnumParameter& m)
    {
        size_t num_elems;
        ar(num_elems);
        ar(m.currentSelection);
        m.enumerations.resize(num_elems);
        m.values.resize(num_elems);
        for(int i = 0; i < num_elems; ++i)
        {
            ar(m.enumerations[i], m.values[i]);
        }
    }
#ifdef HAVE_OPENCV
    template<class A> void serialize(A& ar, cv::Range& range)
    {
        ar(range.start);
        ar(range.end);
    }
    template<class A, class T> void serialize(A& ar, cv::Rect_<T>& rect)
    {
        ar(rect.x);
        ar(rect.y);
        ar(rect.width);
        ar(rect.height);
    }
    template<class A, class T> void serialize(A& ar, cv::Point_<T>& pt)
    {
        ar(pt.x);
        ar(pt.y);
    }
    template<class A, class T> void serialize(A& ar, cv::Point3_<T>& pt)
    {
        ar(pt.x);
        ar(pt.y);
        ar(pt.z);
    }
    template<class A, class T, int M, int N> void serialize(A& ar, cv::Matx<T,M, N>& mat)
    {
        for(int row = 0; row < M; ++row)
        {
            for(int col = 0; col < N; ++col)
            {
                ar(mat(row, col));
            }
        }
    }
    template<class A> void load(A& ar, cv::Mat& mat)
    {
        int channels, rows, cols, depth;
        ar(rows, cols, channels, depth);
        mat.create(rows, cols, CV_MAKETYPE(depth, channels));
        ar.loadBinaryValue(mat.data, mat.dataend - mat.datastart);
    }
    PARAMETER_EXPORTS void load(cereal::BinaryInputArchive& ar, cv::Mat& mat);
    template<class A> void save(A& ar, cv::Mat const& mat)
    {
        int channels = mat.channels();
        int depth = mat.depth();
        ar(mat.rows, mat.cols, channels, depth);
        ar.saveBinaryValue(mat.data, mat.dataend - mat.datastart);
    }
    PARAMETER_EXPORTS void save(cereal::BinaryOutputArchive ar, cv::Mat const & mat);
    template<class A> void load(A& ar, cv::cuda::HostMem& mat)
    {
        int channels, rows, cols, depth;
        ar(rows, cols, channels, depth);
        mat.create(rows, cols, CV_MAKETYPE(depth, channels));
        ar.loadBinaryValue(mat.data, mat.dataend - mat.datastart);
    }
    PARAMETER_EXPORTS void load(cereal::BinaryInputArchive& ar, cv::cuda::HostMem& mat);
    template<class A> void save(A& ar, cv::cuda::HostMem const& mat)
    {
        ar(mat.createMatHeader());
    }
    template<class A> void load(A& ar, cv::cuda::GpuMat& mat)
    {
        cv::Mat h_mat;
        load(ar, h_mat);
        mat.upload(h_mat);
    }
    template<class A> void save(A& ar, cv::cuda::GpuMat const& mat)
    {
        cv::Mat h_mat;
        mat.download(h_mat);
        save(ar, h_mat);
    }

#endif
    
} // namespace cereal

namespace Parameters
{
    class Parameter;
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
#ifdef HAVE_CEREAL
        template<class T> class PARAMETER_EXPORTS constructor
        {
        public:
            constructor()
            {
                Serializer::instance()->register_binary_serializer(
                    std::bind(&constructor<T>::serialize<cereal::BinaryInputArchive>, std::placeholders::_1, std::placeholders::_2),
                    std::bind(&constructor<T>::serialize<cereal::BinaryOutputArchive>, std::placeholders::_1, std::placeholders::_2),
                    Loki::TypeInfo(typeid(T)));

                Serializer::instance()->register_xml_serializer(
                    std::bind(&constructor<T>::serialize<cereal::XMLInputArchive>, std::placeholders::_1, std::placeholders::_2),
                    std::bind(&constructor<T>::serialize<cereal::XMLOutputArchive>, std::placeholders::_1, std::placeholders::_2),
                    Loki::TypeInfo(typeid(T)));

                Serializer::instance()->register_json_serializer(
                    std::bind(&constructor<T>::serialize<cereal::JSONInputArchive>, std::placeholders::_1, std::placeholders::_2),
                    std::bind(&constructor<T>::serialize<cereal::JSONOutputArchive>, std::placeholders::_1, std::placeholders::_2),
                    Loki::TypeInfo(typeid(T)));

            }
            template<typename Archive> static void serialize(Archive& ar, Parameter* param)
            {
                T* typed = dynamic_cast<T*>(param);
                if(typed)
                {
                    ar(*typed);
                }
            }
        };
        template<class T> class policy
        {
            static constructor<T> _cereal_constructor;
        public:
            policy()
            {
                (void)&_cereal_constructor;
            }
            };
        template<class T> constructor<T> policy<T>::_cereal_constructor;
#else
        template<class T> class policy {};
#endif
    } // namespace cereal
} // namespace parameters


