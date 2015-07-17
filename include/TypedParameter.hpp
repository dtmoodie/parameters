#pragma once
#include "MetaParameter.hpp"
namespace Parameters
{
	template<typename T> class TypedParameter : public MetaTypedParameter < T >
	{
		T data;
	public:
		typedef std::shared_ptr<TypedParameter<T>> Ptr;
		static Ptr create(const T& init, const std::string& name, const ParameterType& type = ParameterType::Control, const std::string& tooltip = "")
		{
			return Ptr(new TypedParameter<T>(name, init, type, tooltip));
		}
		TypedParameter(const std::string& name, const T& init = T(), const ParameterType& type = ParameterType::Control, const std::string& tooltip = "") :
			MetaTypedParameter<T>(name, type, tooltip), data(init) {}

		virtual T* Data()
		{
			return &data;
		}
		virtual void UpdateData(T& data_)
		{
			data = data_;
			changed = true;
			UpdateSignal();
		}
		virtual void UpdateData(const T& data_)
		{
			data = data_;
			changed = true;
			UpdateSignal();
		}
		virtual void UpdateData(T* data_)
		{
			data = *data_;
			changed = true;
			UpdateSignal();
		}
	};

	template<typename T> class TypedParameterRef : public MetaTypedParameter < T >
	{
		T& data;
	public:
		typedef std::shared_ptr<TypedParameterRef<T>> Ptr;
		TypedParameterRef(const std::string& name, const ParameterType& type = ParameterType::Control, const std::string& tooltip = "") :
			MetaTypedParameter<T>(name, type, tooltip){}
		virtual T* Data()
		{
			return &data;
		}
		virtual void UpdateData(const T& data_)
		{
			data = data_;
			changed = true;
			UpdateSignal();
		}
		virtual void UpdateData(const T* data_)
		{
			data = *data_;
			changed = true;
			UpdateSignal();
		}

	};

	template<typename T> class TypedParameterPtr : public MetaTypedParameter < T >
	{
		T* ptr;
		bool ownsData;
	public:
		typedef std::shared_ptr<TypedParameterPtr<T>> Ptr;
		static Ptr create(const std::string& name, T* ptr_ = nullptr,
			const ParameterType& type = ParameterType::Control, const std::string& tooltip = "")
		{
			return Ptr(new TypedInputParameterPtr(name, ptr_, type, tooltip));
		}
		TypedParameterPtr(const std::string& name, T* ptr_ = nullptr,
			const ParameterType& type = ParameterType::Control, const std::string& tooltip = "", bool ownsData_ = false) :
			ptr(ptr_),
			MetaTypedParameter<T>(name, type, tooltip), ownsData(ownsData_){}

		virtual T* Data()
		{
			return ptr;
		}
		virtual void UpdateData(T& data)
		{
			ptr = &data;
			changed = true;
			UpdateSignal();
		}
		virtual void UpdateData(const T& data)
		{
			if (ptr)
				*ptr = data;
			changed = true;
			UpdateSignal();
		}
		virtual void UpdateData(T* data_)
		{
			ptr = data_;
			changed = true;
			UpdateSignal();
		}
	};
}