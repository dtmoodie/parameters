#pragma once
#ifdef HAVE_CEREAL
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/archives/json.hpp>
#ifdef HAVE_OPENCV
#include <opencv2/core/types.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/cuda.hpp>
#endif
namespace cereal
{
    template<class T, class A> void save(A& ar, std::vector<T> const& m)
    {
        size_t size;
        size = m.size();
        ar(size);
        for(auto& itr : m)
        {
            ar(itr);
        }
    }
    template<class T, class A> void load(A& ar, std::vector<T>& m)
    {
        size_t size;
        ar(size);
        m.resize(size);
        for(auto& itr : m)
        {
            ar(itr);
        }
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
        ar(cereal::binary_data(mat.data, mat.dataend - mat.datastart));
    }
    template<class A> void save(A& ar, cv::Mat const& mat)
    {
        int channels = mat.channels();
        int depth = mat.depth();
        ar(mat.rows, mat.cols, channels, depth);
        ar(cereal::binary_data(mat.data, mat.dataend - mat.datastart));
    }
    template<class A> void load(A& ar, cv::cuda::HostMem& mat)
    {
        int channels, rows, cols, depth;
        ar(rows, cols, channels, depth);
        mat.create(rows, cols, CV_MAKETYPE(depth, channels));
        ar(cereal::binary_data(mat.data, mat.dataend - mat.datastart));
    }
    template<class A> void save(A& ar, cv::cuda::HostMem const& mat)
    {
        int channels = mat.channels();
        int depth = mat.depth();
        ar(mat.rows, mat.cols, channels, depth);
        ar(cereal::binary_data(mat.data, mat.dataend - mat.datastart));
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
}
#endif
