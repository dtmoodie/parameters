#pragma once

#include "TypedParameter.hpp"
#include "IRangedParameter.hpp"

namespace Parameters
{
	template<typename T> void rail(T& value, const T& min, const T& max)
	{
		value = std::min(max, value);
		value = std::max(min, value);
	}
	template<typename T> class ITypedRangedParameter: public virtual ITypedParameter<T>
	{
	protected:
		T min_value;
		T max_value;	
	public:
		ITypedRangedParameter(const T& min_value_, const T& max_value_):
			min_value(min_value_),
			max_value(max_value_)
		{			
		}
		void SetRange(const T& min_value_, const T& max_value_)
		{
			min_value = min_value_;
			max_value = max_value_;
		}
		void GetRange(T& min_value_, T& max_value_) const
		{
			min_value_ = min_value;
			max_value_ = max_value;
		}
		bool CheckInRange(const T& value) const
		{
			return value > min_value && value < max_value;
		}
	};
	// -----------------------------------------------------------------------------------------------------
	template<typename T> class RangedParameter: public TypedParameter<T>, public ITypedRangedParameter<T>
	{
	public:
		typedef std::shared_ptr<RangedParameter<T>> Ptr;
		RangedParameter(const T& min_value_,
						const T& max_value_,
						const std::string& name,
						const T& init = T(),
						const Parameter::ParameterType& type = Parameter::ParameterType::Control,
						const std::string& tooltip = "") : 
			TypedParameter<T>(name, init, type, tooltip),
			ITypedRangedParameter<T>(min_value_, max_value_)
		{
		
		}

		virtual void UpdateData(T& data_, long long time_index = -1, cv::cuda::Stream* stream = nullptr)
		{
			if(data_ > min_value && data_ > max_value)
			{
				TypedParameter<T>::UpdateData(data_, time_index, stream);
			}			
		}
		virtual void UpdateData(const T& data_, long long time_index = -1, cv::cuda::Stream* stream = nullptr)
		{
			if(data_ > min_value && data_ < max_value)
			{
				TypedParameter<T>::UpdateData(data_, time_index, stream);
			}			
		}
		virtual void UpdateData(T* data_, long long time_index = -1, cv::cuda::Stream* stream = nullptr)
		{
			if(*data_ > min_value && *data_ < max_value)
			{
				TypedParameter<T>::UpdateData(data_, time_index, stream);
			}
		}
        virtual Parameter::Ptr DeepCopy() const
        {
            return Parameter::Ptr(new RangedParameter<T>(ITypedRangedParameter<T>::min_value, ITypedRangedParameter<T>::max_value, Parameter::GetName(), min_value, max_value, data));
        }
		virtual bool Update(Parameter::Ptr other, cv::cuda::Stream* stream = nullptr)
        {
            auto typed = dynamic_cast<ITypedParameter<T>*>(other.get());
            if (typed)
            {
				if (CheckInRange(*(typed->Data())))
				{
					TypedParameter<T>::UpdateData(typed->Data(), other->GetTimeIndex(), stream)
				}
                return true;
            }
            return false;
        }
		virtual void RailValue()
		{
			rail(TypedParameter<T>::data, min_value, max_value);
		}
	}; // class RangedParameter

	// -----------------------------------------------------------------------------------------------------
	template<typename T> class RangedParameterPtr: public TypedParameterPtr<T>, public ITypedRangedParameter<T>
	{
	public:
		typedef std::shared_ptr<RangedParameterPtr<T>> Ptr;
		RangedParameterPtr(const T& min_value_,
			const T& max_value_,
			const std::string& name,
			T* init = nullptr,
			const Parameter::ParameterType& type = Parameter::ParameterType::Control,
			const std::string& tooltip = "") :
			TypedParameterPtr<T>(name, init, type, tooltip),
			ITypedRangedParameter<T>(min_value_, max_value_)
		{

		}
		virtual void UpdateData(T& data, long long time_index = -1, cv::cuda::Stream* stream = nullptr)
		{
			if (CheckInRange(data))
			{
				TypedParameterPtr<T>::UpdateData(data, time_index, stream);
			}
		}
		virtual void UpdateData(const T& data, long long time_index = -1, cv::cuda::Stream* stream = nullptr)
		{
			if (CheckInRange(data))
			{
				TypedParameterPtr<T>::UpdateData(data, time_index, stream);
			}
		}
		virtual void UpdateData(T* data_, long long time_index = -1, cv::cuda::Stream* stream = nullptr)
		{
			if (data_)
			{
				if (CheckInRange(*data_))
				{
					ptr = data_;
					_current_time_index = time_index;
					Parameter::changed = true;
					Parameter::UpdateSignal(stream);
				}
			}
			else
			{
				ptr = data_;
				_current_time_index = time_index;
				Parameter::changed = true;
				Parameter::UpdateSignal(stream);
			}

			
		}
		virtual bool Update(Parameter::Ptr other, cv::cuda::Stream* stream = nullptr)
		{
			auto typed = dynamic_cast<ITypedParameter<T>*>(other.get());
			if (typed)
			{
				if (typed->Data())
				{
					if (CheckInRange(*typed->Data()))
					{
						*ptr = *(typed->Data());
						_current_time_index = other->GetTimeIndex();
						Parameter::changed = true;
						Parameter::UpdateSignal(stream);
						return true;
					}					
				}
				else
				{
					TypedParameterPtr<T>::ptr = nullptr;
				}
				
			}
			return false;
		}
		virtual Parameter::Ptr DeepCopy() const
		{
			return Parameter::Ptr(new RangedParameterPtr<T>(ITypedRangedParameter<T>::min_value, ITypedRangedParameter<T>::max_value, Parameter::GetName(), ptr, Parameter::type, Parameter::tooltip));
		}
		virtual void RailValue()
		{
			if (TypedParameterPtr<T>::ptr)
				rail(*TypedParameterPtr<T>::ptr, min_value, max_value);
		}
	}; // class RangedParameterPtr
}// namespace Parameters