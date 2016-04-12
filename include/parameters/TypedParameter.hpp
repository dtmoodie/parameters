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
#include "MetaParameter.hpp"
namespace Parameters
{
	template<typename T> class TypedParameter : public MetaTypedParameter < T >
	{
	protected:
		T data;
	public:
		typedef std::shared_ptr<TypedParameter<T>> Ptr;
		static std::shared_ptr<TypedParameter<T>> create(const T& init, const std::string& name, const Parameter::ParameterType& type = Parameter::ParameterType::Control, const std::string& tooltip = "")
		{
			return std::shared_ptr<TypedParameter<T>>(new TypedParameter<T>(name, init, type, tooltip));
		}
		TypedParameter(const std::string& name, 
			const T& init = T(), 
			const Parameter::ParameterType& type = Parameter::ParameterType::Control, 
			const std::string& tooltip = "") :
			MetaTypedParameter<T>(name, type, tooltip), data(init) 
		{}
		virtual T* Data(long long time_index = -1)
		{
			if(time_index != -1)
				LOGIF_NEQ(time_index, _current_time_index, trace);
			return &data;
		}
		virtual void UpdateData(T& data_, long long time_index = -1, cv::cuda::Stream* stream = nullptr)
		{
			_current_time_index = time_index;
			data = data_;
			Parameter::changed = true;
			Parameter::UpdateSignal(stream);
		}
		virtual void UpdateData(const T& data_, long long time_index = -1, cv::cuda::Stream* stream = nullptr)
		{
			_current_time_index = time_index;
			data = data_;
			Parameter::changed = true;
			ITypedParameter<T>::UpdateSignal(stream);
		}
		virtual void UpdateData(T* data_, long long time_index = -1, cv::cuda::Stream* stream = nullptr)
		{
			_current_time_index = time_index;
			data = *data_;
			Parameter::changed = true;
			Parameter::UpdateSignal(stream);
		}
        virtual Parameter::Ptr DeepCopy() const
        {
            return Parameter::Ptr(new TypedParameter<T>(Parameter::GetName(), data));
        }
		virtual bool Update(Parameter::Ptr other, cv::cuda::Stream* stream = nullptr)
        {
            auto typed = dynamic_cast<ITypedParameter<T>*>(other.get());
            if (typed)
            {
                data = *(typed->Data());
				Parameter::changed = true;
				Parameter::UpdateSignal(stream);
                return true;
            }
            return false;
        }
	};

	template<typename T> class TypedParameterPtr : public MetaTypedParameter < T >
	{
	protected:
		T* ptr;
		bool ownsData;
	public:
		typedef std::shared_ptr<TypedParameterPtr<T>> Ptr;
		static std::shared_ptr<TypedParameterPtr<T>> create(const std::string& name, T* ptr_ = nullptr,
			const Parameter::ParameterType& type = Parameter::ParameterType::Control, const std::string& tooltip = "")
		{
			return std::shared_ptr<TypedParameterPtr<T>>(new TypedParameterPtr(name, ptr_, type, tooltip));
		}

		TypedParameterPtr(const std::string& name, 
			T* ptr_ = nullptr,
			const Parameter::ParameterType& type = Parameter::ParameterType::Control, const std::string& tooltip = "", bool ownsData_ = false) :
			ptr(ptr_),
			MetaTypedParameter<T>(name, type, tooltip), ownsData(ownsData_)
		{}


		virtual T* Data(long long time_index)
		{
			LOGIF_NEQ(time_index, _current_time_index, trace);
			return ptr;
		}
		virtual void UpdateData(T& data, long long time_index = -1, cv::cuda::Stream* stream = nullptr)
		{
			ptr = &data;
			Parameter::changed = true;
			Parameter::UpdateSignal(stream);
		}
		virtual void UpdateData(const T& data, long long time_index = -1, cv::cuda::Stream* stream = nullptr)
		{
			if (ptr)
			{
				*ptr = data;
				_current_time_index = time_index;
				Parameter::changed = true;
				Parameter::UpdateSignal(stream);
			}				
		}
		virtual void UpdateData(T* data_, long long time_index = -1, cv::cuda::Stream* stream = nullptr)
		{
			ptr = data_;
			_current_time_index = time_index;
			Parameter::changed = true;
			Parameter::UpdateSignal(stream);
		}
		virtual bool Update(Parameter::Ptr other, cv::cuda::Stream* stream = nullptr)
        {
            auto typed = dynamic_cast<ITypedParameter<T>*>(other.get());
            if(typed)
            {
                *ptr = *(typed->Data());
				_current_time_index = other->GetTimeIndex();
				Parameter::changed = true;
				Parameter::UpdateSignal(stream);
                return true;
            }
            return false;
        }
        virtual Parameter::Ptr DeepCopy() const
        {
            return Parameter::Ptr(new TypedParameter<T>(Parameter::GetName(), *ptr));
        }
	};
}
