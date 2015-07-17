#pragma once
#include "MetaParameter.hpp"
#include "InputParameter.hpp"
namespace Parameters
{
	template<typename T> class TypedInputParameter : public MetaTypedParameter<T>, public InputParameter
	{
		ITypedParameter<T>::Ptr input;
		boost::signals2::connection inputConnection;
		virtual void onInputUpdate()
		{

		}
	public:
		typedef std::shared_ptr<TypedInputParameter<T>> Ptr;
		TypedInputParameter(const std::string& name, const std::string& tooltip = "", const boost::function<bool(Parameter*)>& qualifier_ = boost::function<bool(Parameter*)>()) :
			MetaTypedParameter<T>(name, ParameterType::Input, tooltip){ qualifier = qualifier_; }
		virtual bool SetInput(const std::string& name_)
		{
			return false;
		}

		virtual bool SetInput(const Parameter::Ptr param)
		{
			ITypedParameter<T>::Ptr castedParam = std::dynamic_pointer_cast<ITypedParameter<T>>(param);
			if (castedParam)
			{
				input = castedParam;
				inputConnection.disconnect();
				inputConnection = castedParam->RegisterNotifier(boost::bind(&TypedInputParameter<T>::onInputUpdate, this));
				return true;
			}
			return false;
		}

		virtual bool AcceptsInput(const Parameter::Ptr param)
		{
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
		virtual void UpdateData(T& data_)
		{

		}
		virtual void UpdateData(const T& data_)
		{

		}
		virtual void UpdateData(T* data_)
		{

		}

	};

	// Meant to reference a pointer variable in user space, and to update that variable whenever 
	// IE int* myVar; 
	// auto typedParam = TypedInputParameterPtr(&myVar); // TypedInputParameter now updates myvar to point to whatever the
	// input variable is for typedParam.
	template<typename T> class TypedInputParameterPtr : public MetaTypedParameter<T>, public InputParameter
	{
		T** userVar; // Pointer to the user space pointer variable of type T
		ITypedParameter<T>::Ptr input;
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
			const ParameterType& type = ParameterType::Control, const std::string& tooltip = "") :
			MetaTypedParameter<T>(name, type, tooltip), userVar(userVar_){}

		virtual bool SetInput(const std::string& name_)
		{
			return false;
		}

		virtual bool SetInput(const Parameter::Ptr param)
		{
			ITypedParameter<T>::Ptr castedParam = std::dynamic_pointer_cast<ITypedParameter<T>>(param);
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

		virtual bool AcceptsInput(const Parameter::Ptr param)
		{
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
		virtual void UpdateData(T& data_)
		{

		}
		virtual void UpdateData(const T& data_)
		{

		}
		virtual void UpdateData(T* data_)
		{

		}
	};
}