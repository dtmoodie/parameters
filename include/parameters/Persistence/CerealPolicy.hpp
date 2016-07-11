#pragma once
#include "parameters/Types.hpp"
#include "cereal.hpp"
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

namespace cereal
{
    class BinaryInputArchive;
    class XMLInputArchive;
    class JSONInputArchive;

    class XMLOutputArchive;
    class BinaryOutputArchive;
    class JSONOutputArchive;
    template<class A, class T1, class T2> void serialize(A& ar, std::pair<T1, T2>& pair);
    template<class A> void serialize(A& ar, Parameters::ReadDirectory& m);
    template<class A> void serialize(A& ar, Parameters::ReadFile& m);
    template<class A> void serialize(A& ar, Parameters::WriteFile& m);
    template<class A> void serialize(A& ar, Parameters::WriteDirectory& m);
    template<class A> void save(A& ar, Parameters::EnumParameter const& m);
    template<class A> void load(A& ar, Parameters::EnumParameter& m);
#ifdef HAVE_OPENCV
    template<class A> void serialize(A& ar, cv::Range& range);
    template<class A, class T> void serialize(A& ar, cv::Rect_<T>& rect);
    template<class A, class T> void serialize(A& ar, cv::Point_<T>& pt);
    template<class A, class T> void serialize(A& ar, cv::Point3_<T>& pt);
    template<class A, class T, int M, int N> void serialize(A& ar, cv::Matx<T,M, N>& mat);
    template<class A> void load(A& ar, cv::Mat& mat);
    PARAMETER_EXPORTS void load(cereal::BinaryInputArchive& ar, cv::Mat& mat);
    template<class A> void load(A& ar, cv::cuda::HostMem& mat);
    PARAMETER_EXPORTS void load(cereal::BinaryInputArchive& ar, cv::cuda::HostMem& mat);
    template<class A> void load(A& ar, cv::cuda::GpuMat& mat);

    template<class A> void save(A& ar, cv::Mat const& mat);
    PARAMETER_EXPORTS void save(cereal::BinaryOutputArchive ar, cv::Mat const & mat);
    template<class A> void save(A& ar, cv::cuda::HostMem const& mat);
    template<class A> void save(A& ar, cv::cuda::GpuMat const& mat);
#endif
    
} // namespace cereal

#include "detail/CerealPolicyImpl.hpp"

namespace Parameters
{
    namespace Persistence
    {
        namespace Cereal
        {
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
        }
    }
}