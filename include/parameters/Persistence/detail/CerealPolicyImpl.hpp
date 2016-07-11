#pragma once

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
    template<class A> void save(A& ar, cv::Mat const& mat)
    {
        int channels = mat.channels();
        int depth = mat.depth();
        ar(mat.rows, mat.cols, channels, depth);
        ar.saveBinaryValue(mat.data, mat.dataend - mat.datastart);
    }
    template<class A> void load(A& ar, cv::cuda::HostMem& mat)
    {
        int channels, rows, cols, depth;
        ar(rows, cols, channels, depth);
        mat.create(rows, cols, CV_MAKETYPE(depth, channels));
        ar.loadBinaryValue(mat.data, mat.dataend - mat.datastart);
    }
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