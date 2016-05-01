/*
Copyright (c) 2015 Daniel Moodie.
All rights reserved.

Redistribution and use in source and binary forms are permitted
provided that the above copyright notice and this paragraph are
duplicated in all such forms and that any documentation,
advertising materials, and other materials related to such
distribution and use acknowledge that the software was developed
by the Daniel Moodie. The name of
Daniel Moodie may not be used to endorse or promote products derived
from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

https://github.com/dtmoodie/parameters
*/
#pragma once

#include "ITypedParameter.hpp"
#include <boost/circular_buffer.hpp>

namespace Parameters
{
	namespace Buffer
	{
		class IParameterBuffer
		{
		public:
			virtual ~IParameterBuffer()
			{

			}
			virtual void SetSize(int size) = 0;
			virtual int GetSize() = 0;
			virtual void GetTimestampRange(long long& start, long long& end) = 0;
		};
		template<typename T> class ParameterBuffer : public ITypedParameter<T>, public IParameterBuffer
		{
			boost::circular_buffer<std::pair<long long, T>> _data_buffer;
		public:
			ParameterBuffer(const std::string& name,
				const T& init = T(), long long time_index = -1,
				const Parameter::ParameterType& type = Parameter::ParameterType::Control,
				const std::string& tooltip = ""):
				ITypedParameter<T>(name)
			{
				_data_buffer.set_capacity(10);
				_data_buffer.push_back(std::make_pair(time_index, init));
			}
			virtual ~ParameterBuffer()
			{

			}
			virtual T* Data(long long time_index = -1)
			{
				if (time_index == -1 && _data_buffer.size())
					return &_data_buffer.back().second;

				for (auto& itr : _data_buffer)
				{
					if (itr.first == time_index)
					{
						return &itr.second;
					}
				}
				return nullptr;
			}
			virtual bool GetData(T& value, long long time_index = -1)
			{
                std::lock_guard<std::recursive_mutex> lock(Parameter::_mtx);
				if (time_index == -1 && _data_buffer.size())
				{
					value = _data_buffer.back().second;
					return true;
				}
				for (auto& itr : _data_buffer)
				{
					if (itr.first == time_index)
					{
						//return &itr.second;
						value = itr.second;
						return true;
					}
				}
				return false;
			}
			virtual void UpdateData(T& data_, long long time_index = -1, cv::cuda::Stream* stream = nullptr)
			{
                std::lock_guard<std::recursive_mutex> lock(Parameter::_mtx);
				_data_buffer.push_back(std::pair<long long, T>(time_index, data_));
				Parameter::changed = true;
				Parameter::OnUpdate(stream);
			}
			virtual void UpdateData(const T& data_, long long time_index = -1, cv::cuda::Stream* stream = nullptr)
			{
                std::lock_guard<std::recursive_mutex> lock(Parameter::_mtx);
				_data_buffer.push_back(std::pair<long long, T>(time_index, data_));
				Parameter::changed = true;
				Parameter::OnUpdate(stream);
			}
			virtual void UpdateData(T* data_, long long time_index = -1, cv::cuda::Stream* stream = nullptr)
			{
                std::lock_guard<std::recursive_mutex> lock(Parameter::_mtx);
				_data_buffer.push_back(std::pair<long long, T>(time_index, *data_));
				Parameter::changed = true;
				Parameter::OnUpdate(stream);
			}

			virtual Loki::TypeInfo GetTypeInfo()
			{
				return Loki::TypeInfo(typeid(T));
			}
			virtual bool Update(Parameter::Ptr other, cv::cuda::Stream* stream = nullptr)
			{
				auto typedParameter = std::dynamic_pointer_cast<ITypedParameter<T>>(other);
				if (typedParameter)
				{
					auto ptr = typedParameter->Data();
					if (ptr)
					{
                        std::lock_guard<std::recursive_mutex> lock(Parameter::_mtx);
						_data_buffer.push_back(std::pair<long long, T>(typedParameter->GetTimeIndex(), *ptr));
						Parameter::changed = true;
						Parameter::OnUpdate(stream);
					}
				}
				return false;
			}
			virtual void SetSize(int size)
			{
                std::lock_guard<std::recursive_mutex> lock(Parameter::_mtx);
				_data_buffer.set_capacity(size);
			}
			virtual int GetSize()
			{
                std::lock_guard<std::recursive_mutex> lock(Parameter::_mtx);
				return _data_buffer.capacity();
			}
			virtual void GetTimestampRange(long long& start, long long& end)
			{
				if (_data_buffer.size())
				{
                    std::lock_guard<std::recursive_mutex> lock(Parameter::_mtx);
					start = _data_buffer.back().first;
					end = _data_buffer.front().first;
				}
			}
		};

		template<typename T> class ParameterBufferProxy : public ParameterBuffer<T>
		{
			ITypedParameter<T>* _input_parameter;
			std::shared_ptr<Signals::connection> _input_update_connection;
			std::shared_ptr<Signals::connection> _input_delete_connection;
		public:
			ParameterBufferProxy(ITypedParameter<T>* input = nullptr) :
				ParameterBuffer<T>(std::string("proxy for ") + (input ? input->GetName() : std::string("NULL"))),
				_input_parameter(input)
			{
				if (input)
				{
					_input_update_connection = _input_parameter->RegisterNotifier(std::bind(&ParameterBufferProxy::onInputUpdate, this, std::placeholders::_1));
					_input_delete_connection = _input_parameter->RegisterDeleteNotifier(std::bind(&ParameterBufferProxy::onInputDelete, this));
					_input_parameter->subscribers++;
				}
			}
			~ParameterBufferProxy()
			{
				if (_input_parameter)
					_input_parameter->subscribers--;
				_input_parameter = nullptr;
				_input_update_connection.reset();
				_input_delete_connection.reset();
			}
			void setInput(Parameter* param)
			{
				if (_input_parameter = dynamic_cast<ITypedParameter<T>*>(param))
				{
					_input_update_connection = _input_parameter->RegisterNotifier(std::bind(&ParameterBufferProxy::onInputUpdate, this, std::placeholders::_1));
					_input_delete_connection = _input_parameter->RegisterDeleteNotifier(std::bind(&ParameterBufferProxy::onInputDelete, this));
				}
			}
			void onInputDelete()
			{
                std::lock_guard<std::recursive_mutex> lock(Parameter::_mtx);
				_input_update_connection.reset();
				_input_delete_connection.reset();
				_input_parameter = nullptr;
			}
			void onInputUpdate(cv::cuda::Stream* stream)
			{
                std::lock_guard<std::recursive_mutex> lock(Parameter::_mtx);
				if (_input_parameter)
				{
					auto time = _input_parameter->GetTimeIndex();
					if (auto data = _input_parameter->Data(time))
					{
                        ParameterBuffer<T>::UpdateData(data, time, stream);
					}
				}				
			}
			virtual Parameter::Ptr DeepCopy() const
			{
				return Parameter::Ptr(new ParameterBufferProxy(_input_parameter));
			}
		};
	}
}
