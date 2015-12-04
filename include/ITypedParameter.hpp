#pragma once

#include "Parameter.hpp"
namespace cv
{
	namespace cuda
	{
		class Stream;
	}
}
namespace Parameters
{
	template<typename T> class ITypedParameter : public Parameter
	{
	public:
		typedef std::shared_ptr<ITypedParameter<T>> Ptr;

		virtual T* Data() = 0;
		virtual void UpdateData(T& data_, cv::cuda::Stream* stream = nullptr) = 0;
		virtual void UpdateData(const T& data_, cv::cuda::Stream* stream = nullptr) = 0;
		virtual void UpdateData(T* data_, cv::cuda::Stream* stream = nullptr) = 0;
		ITypedParameter(const std::string& name, const ParameterType& type = Parameter::Control, const std::string& tooltip = "") :
			Parameter(name, type, tooltip){}
		virtual Loki::TypeInfo GetTypeInfo()
		{
			return Loki::TypeInfo(typeid(T));
		}
		bool Update(Parameter::Ptr other, cv::cuda::Stream* stream = nullptr)
		{
            auto typedParameter = std::dynamic_pointer_cast<ITypedParameter<T>>(other);
			if (typedParameter)
			{
				auto ptr = typedParameter->Data();
				*Data() = *ptr;
				UpdateSignal(stream);
			}
			return false;
		}
		
	};
}
