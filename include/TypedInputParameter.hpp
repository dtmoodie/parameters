#pragma once
#include <ITypedParameter.hpp>
#include "InputParameter.hpp"

namespace Parameters
{
	template<typename T> class TypedInputParameter : public ITypedParameter<T>, public InputParameter
	{
		typename ITypedParameter<T>::Ptr input;
		boost::signals2::connection inputConnection;
		virtual void onInputUpdate()
		{
			Parameter::UpdateSignal(nullptr);
		}
	public:
		typedef std::shared_ptr<TypedInputParameter<T>> Ptr;

		TypedInputParameter(const std::string& name, 
			const std::string& tooltip = "", 
			const boost::function<bool(Parameter*)>& qualifier_ = boost::function<bool(Parameter*)>()) :
			ITypedParameter<T>(name, Parameter::ParameterType::Input, tooltip)
		{ 
				qualifier = qualifier_; 
		}
        ~TypedInputParameter()
        {
            inputConnection.disconnect();
        }

		virtual bool SetInput(const std::string& name_)
		{
			return false;
		}

		virtual bool SetInput(const Parameter::Ptr param)
		{
			if (param == nullptr)
			{
				input.reset();
				inputConnection.disconnect();
				Parameter::UpdateSignal(nullptr);
				return true;
			}				
			typename ITypedParameter<T>::Ptr castedParam = std::dynamic_pointer_cast<ITypedParameter<T>>(param);
			if (castedParam)
			{
				input = castedParam;
				inputConnection.disconnect();
				inputConnection = castedParam->RegisterNotifier(boost::bind(&TypedInputParameter<T>::onInputUpdate, this));
				Parameter::UpdateSignal(nullptr);
				return true;
			}
			return false;
		}
		std::shared_ptr<Parameter> GetInput()
		{
			return input;
		}

		virtual bool AcceptsInput(const Parameter::Ptr param)
		{
			if (qualifier)
				return qualifier(param.get());
			return Loki::TypeInfo(typeid(T)) == param->GetTypeInfo();
		}

		virtual bool AcceptsType(const Loki::TypeInfo& type)
		{
			return Loki::TypeInfo(typeid(T)) == type;
		}
		virtual T* Data()
		{
			if (input)
				return input->Data();
			return nullptr;
		}
		virtual void UpdateData(T& data_, cv::cuda::Stream* stream)
		{

		}
		virtual void UpdateData(const T& data_, cv::cuda::Stream* stream)
		{

		}
		virtual void UpdateData(T* data_, cv::cuda::Stream* stream)
		{

		}
		virtual Loki::TypeInfo GetTypeInfo()
		{
			return Loki::TypeInfo(typeid(T));
		}

	};

	template<typename T> class TypedInputParameterCopy : public MetaTypedParameter<T>, public InputParameter
	{
		T* userVar; // Pointer to the user space variable of type T
		typename ITypedParameter<T>::Ptr input;
		boost::signals2::connection inputConnection;

		virtual void onInputUpdate()
		{
			// The input variable has been updated, update user var
			*userVar = *input->Data();
			Parameter::changed = true;
			Parameter::UpdateSignal(nullptr);
		}
	public:
		typedef std::shared_ptr<TypedInputParameterCopy<T>> Ptr;
		static Ptr create(T* userVar_)
		{
			return Ptr(new TypedInputParameterCopy(userVar_));
		}
		TypedInputParameterCopy(const std::string& name, T* userVar_,
			const Parameter::ParameterType& type = Parameter::ParameterType::Control, const std::string& tooltip = "") :
			MetaTypedParameter<T>(name, type, tooltip), userVar(userVar_) {}
        ~TypedInputParameterCopy()
        {
            inputConnection.disconnect();
        }
		virtual bool SetInput(const std::string& name_)
		{
			return false;
		}

		virtual bool SetInput(const Parameter::Ptr param)
		{
			typename ITypedParameter<T>::Ptr castedParam = std::dynamic_pointer_cast<ITypedParameter<T>>(param);
			if (castedParam)
			{
				input = castedParam;
				inputConnection.disconnect();
				inputConnection = castedParam->RegisterNotifier(boost::bind(&TypedInputParameterCopy<T>::onInputUpdate, this));
				*userVar = *input->Data();
				return true;
			}
			return false;
		}
		std::shared_ptr<Parameter> GetInput()
		{
			return input;
		}

		virtual bool AcceptsInput(const Parameter::Ptr param)
		{
			if (qualifier)
				return qualifier(param.get());
			return Loki::TypeInfo(typeid(T)) == param->GetTypeInfo();
		}

		virtual bool AcceptsType(const Loki::TypeInfo& type)
		{
			return Loki::TypeInfo(typeid(T)) == type;
		} 
		virtual T* Data()
		{
			return userVar;
		}
		virtual void UpdateData(T& data_, cv::cuda::Stream* stream)
		{
			*userVar = data_;
		}
		virtual void UpdateData(const T& data_, cv::cuda::Stream* stream)
		{
			*userVar = data_;
		}
		virtual void UpdateData(T* data_, cv::cuda::Stream* stream)
		{
			*userVar = *data_;
		}
		virtual Loki::TypeInfo GetTypeInfo()
		{
			return Loki::TypeInfo(typeid(T));
		}

	};

	// Meant to reference a pointer variable in user space, and to update that variable whenever 
	// IE int* myVar; 
	// auto typedParam = TypedInputParameterPtr(&myVar); // TypedInputParameter now updates myvar to point to whatever the
	// input variable is for typedParam.
	template<typename T> class TypedInputParameterPtr : public MetaTypedParameter<T>, public InputParameter
	{
		T** userVar; // Pointer to the user space pointer variable of type T
		typename ITypedParameter<T>::Ptr input;
		boost::signals2::connection inputConnection;
		virtual void onInputUpdate()
		{
			// The input variable has been updated, update user var
			*userVar = input->Data();
		}
	public:
		typedef std::shared_ptr<TypedInputParameterPtr<T>> Ptr;
		static Ptr create(T** userVar_)
		{
			return Ptr(new TypedInputParameterPtr(userVar_));
		}
		TypedInputParameterPtr(const std::string& name, T** userVar_,
			const Parameter::ParameterType& type = Parameter::ParameterType::Control, const std::string& tooltip = "") :
			MetaTypedParameter<T>(name, type, tooltip), userVar(userVar_){}
        ~TypedInputParameterPtr()
        {
            inputConnection.disconnect();
        }
		virtual bool SetInput(const std::string& name_)
		{
			return false;
		}

		virtual bool SetInput(const Parameter::Ptr param)
		{
			typename ITypedParameter<T>::Ptr castedParam = std::dynamic_pointer_cast<ITypedParameter<T>>(param);
			if (castedParam)
			{
				input = castedParam;
				inputConnection.disconnect();
				inputConnection = castedParam->RegisterNotifier(boost::bind(&TypedInputParameterPtr<T>::onInputUpdate, this));
				*userVar = input->Data();
				return true;
			}
			return false;
		}
		std::shared_ptr<Parameter> GetInput()
		{
			return input;
		}

		virtual bool AcceptsInput(const Parameter::Ptr param)
		{
			if (qualifier)
				return qualifier(param.get());
			return Loki::TypeInfo(typeid(T)) == param->GetTypeInfo();
		}

		virtual bool AcceptsType(const Loki::TypeInfo& type)
		{
			return Loki::TypeInfo(typeid(T)) == type;
		}
		virtual T* Data()
		{
			if (input)
				return input->Data();
			return nullptr;
		}
		virtual void UpdateData(T& data_, cv::cuda::Stream* stream)
		{

		}
		virtual void UpdateData(const T& data_, cv::cuda::Stream* stream)
		{

		}
		virtual void UpdateData(T* data_, cv::cuda::Stream* stream)
		{

		}
		virtual Loki::TypeInfo GetTypeInfo()
		{
			return Loki::TypeInfo(typeid(T));
		}
	};
}
