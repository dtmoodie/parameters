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
			virtual void SetSize(int size) = 0;
			virtual int GetSize() = 0;
			virtual void GetTimestampRange(long long& start, long long& end) = 0;
		};
		template<typename T> class ParameterBuffer : public ITypedParameter<T>, IParameterBuffer
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
			virtual void UpdateData(T& data_, long long time_index = -1, cv::cuda::Stream* stream = nullptr)
			{
				_data_buffer.push_back(std::pair<long long, T>(time_index, data_));
				Parameter::changed = true;
				Parameter::UpdateSignal(stream);
			}
			virtual void UpdateData(const T& data_, long long time_index = -1, cv::cuda::Stream* stream = nullptr)
			{
				_data_buffer.push_back(std::pair<long long, T>(time_index, data_));
				Parameter::changed = true;
				Parameter::UpdateSignal(stream);
			}
			virtual void UpdateData(T* data_, long long time_index = -1, cv::cuda::Stream* stream = nullptr)
			{
				_data_buffer.push_back(std::pair<long long, T>(time_index, *data_));
				Parameter::changed = true;
				Parameter::UpdateSignal(stream);
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
						_data_buffer.push_back(std::pair<long long, T>(typedParameter->GetTimeIndex(), *ptr));
						Parameter::changed = true;
						Parameter::UpdateSignal(stream);
					}
				}
				return false;
			}
			virtual void SetSize(int size)
			{
				_data_buffer.set_capacity(size);
			}
			virtual int GetSize()
			{
				return _data_buffer.capacity();
			}
			virtual void GetTimestampRange(long long& start, long long& end)
			{
				if (_data_buffer.size())
				{
					start = _data_buffer.back().first;
					end = _data_buffer.front().first;
				}
			}
		};

		template<typename T> class ParameterBufferProxy : public ParameterBuffer<T>
		{
			std::shared_ptr<ITypedParameter<T>> _input_parameter;
			Signals::connection::Ptr _input_connection;
		public:
			ParameterBufferProxy(std::shared_ptr<ITypedParameter<T>> input) :
				ParameterBuffer<T>("proxy for " + input->GetName())
			{
				if (_input_parameter = input)
				{
					_input_connection = _input_parameter->RegisterNotifier(std::bind(&ParameterBufferProxy::onInputUpdate, this, std::placeholders::_1));
				}
			}
			void setInput(std::shared_ptr<Parameter> param)
			{
				if (_input_parameter = std::dynamic_pointer_cast<ITypedParameter<T>>(param))
				{
					_input_connection = _input_parameter->RegisterNotifier(std::bind(&ParameterBufferProxy::onInputUpdate, this, std::placeholders::_1));
				}
			}
			void onInputUpdate(cv::cuda::Stream* stream)
			{
				if (auto data = _input_parameter->Data())
				{
					UpdateData(data, _input_parameter->GetTimeIndex(), stream);
				}
			}
			virtual Parameter::Ptr DeepCopy() const
			{
				return Parameter::Ptr(new ParameterBufferProxy(_input_parameter));
			}
		};
	}
}