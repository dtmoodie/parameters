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
	template<typename T> void rail(std::vector<T>& value, const T& min, const T& max)
	{
		
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
		bool CheckInRange(const T& value) const
		{
			return value > min_value && value < max_value;
		}
	};
	
	template<typename T> class RangedParameter: public TypedParameter<T>, public ITypedRangedParameter<T>
	{
	public:
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
            return Parameter::Ptr(new RangedParameter<T>(Parameter::GetName(), min_value, max_value, data));
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
		virtual void RailValue()
		{
			rail(TypedParameter<T>::data, min_value, max_value);
		}
	}; // class RangedParameter
	
	template<typename T> class RangedParameterRef: public TypedParameterRef<T>
	{
	protected:
		T min_value;
		T max_value;
	public:
		RangedParameterRef
	}; // class RangedParameterRef
}// namespace Parameters