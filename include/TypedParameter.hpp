#pragma once
#include "MetaParameter.hpp"
namespace Parameters
{
	template<typename T> class TypedParameter : public MetaTypedParameter < T >
	{
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
		virtual T* Data()
		{
			return &data;
		}
		virtual void UpdateData(T& data_, cv::cuda::Stream* stream = nullptr)
		{
			data = data_;
			Parameter::changed = true;
			Parameter::UpdateSignal(stream);
		}
		virtual void UpdateData(const T& data_, cv::cuda::Stream* stream = nullptr)
		{
			data = data_;
			Parameter::changed = true;
			ITypedParameter<T>::UpdateSignal(stream);
		}
		virtual void UpdateData(T* data_, cv::cuda::Stream* stream = nullptr)
		{
			data = *data_;
			Parameter::changed = true;
			Parameter::UpdateSignal(stream);
		}
	};

	template<typename T> class TypedParameterRef : public MetaTypedParameter < T >
	{
		T& data;
	public:
		typedef std::shared_ptr<TypedParameterRef<T>> Ptr;
		TypedParameterRef(const std::string& name, const Parameter::ParameterType& type = Parameter::ParameterType::Control, const std::string& tooltip = "") :
			MetaTypedParameter<T>(name, type, tooltip){}
		
		virtual T* Data()
		{
			return &data;
		}
		virtual void UpdateData(const T& data_, cv::cuda::Stream* stream = nullptr)
		{
			data = data_;
			Parameter::changed = true;
			Parameter::UpdateSignal(stream);
		}
		virtual void UpdateData(T& data_, cv::cuda::Stream* stream = nullptr)
		{
			data = data_;
			Parameter::changed = true;
			ITypedParameter<T>::TypedUpdateSignal(stream);
		}
		virtual void UpdateData(T* data_, cv::cuda::Stream* stream = nullptr)
		{
			data = *data_;
			Parameter::changed = true;
			Parameter::UpdateSignal(stream);
		}
	};

	template<typename T> class TypedParameterPtr : public MetaTypedParameter < T >
	{
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


		virtual T* Data()
		{
			return ptr;
		}
		virtual void UpdateData(T& data, cv::cuda::Stream* stream = nullptr)
		{
			ptr = &data;
			Parameter::changed = true;
			Parameter::UpdateSignal(stream);
		}
		virtual void UpdateData(const T& data, cv::cuda::Stream* stream = nullptr)
		{
			if (ptr)
				*ptr = data;
			Parameter::changed = true;
			Parameter::UpdateSignal(stream);
		}
		virtual void UpdateData(T* data_, cv::cuda::Stream* stream = nullptr)
		{
			ptr = data_;
			Parameter::changed = true;
			Parameter::UpdateSignal(stream);
		}
	};
}
