#pragma once

#include "map.hpp"

namespace Parameters
{
    namespace Buffer
    {
        template<typename T> class ConstMap: public Map<T>
        {
            int _size;
        public:
            ConstMap(const std::string& name,
                const T& init = T(), long long time_index = -1,
                const Parameter::ParameterType& type = Parameter::ParameterType::Control,
                const std::string& tooltip = "") :
                Map<T>(name, init, time_index, type, tooltip)
            {
                _size = 10;
            }
            virtual void UpdateData(T& data_, long long time_index = -1, cv::cuda::Stream* stream = nullptr)
            {
                Map<T>::UpdateData(data_, time_index, stream);
                clean();
            }
            virtual void UpdateData(const T& data_, long long time_index = -1, cv::cuda::Stream* stream = nullptr)
            {
                Map<T>::UpdateData(data_, time_index, stream);
                clean();
            }
            virtual void UpdateData(T* data_, long long time_index = -1, cv::cuda::Stream* stream = nullptr)
            {
                Map<T>::UpdateData(data_, time_index, stream);
                clean();
            }
            void clean()
            {
                while(_data_buffer.size() > _size)
                {
                    _data_buffer.erase(_data_buffer.begin());
                }
            }
            virtual void SetSize(long long size)
            {
                _size = size;
            }

        };
    }
}