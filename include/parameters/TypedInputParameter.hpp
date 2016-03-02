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
#include "InputParameter.hpp"

namespace Parameters
{
	template<typename T> class TypedInputParameter : public ITypedParameter<T>, public InputParameter
	{
		typename ITypedParameter<T>::Ptr input;
		std::shared_ptr<Signals::connection> inputConnection;
		virtual void onInputUpdate()
		{
			Parameter::UpdateSignal(nullptr);
            Parameter::changed = true;
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
            //inputConnection.disconnect();
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
				inputConnection.reset();
				Parameter::UpdateSignal(nullptr);
				return true;
			}				
			typename ITypedParameter<T>::Ptr castedParam = std::dynamic_pointer_cast<ITypedParameter<T>>(param);
            if(input)
            {
                input->subscribers--;
            }
			if (castedParam)
			{
				input = castedParam;
				input->subscribers++;
				inputConnection = castedParam->RegisterNotifier(boost::bind(&TypedInputParameter<T>::onInputUpdate, this));
				Parameter::UpdateSignal(nullptr);
                Parameter::changed = true;
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
        virtual Parameter::Ptr DeepCopy() const
        {
            return Parameter::Ptr();
        }
	};

	template<typename T> class TypedInputParameterCopy : public MetaTypedParameter<T>, public InputParameter
	{
		T* userVar; // Pointer to the user space variable of type T
		typename ITypedParameter<T>::Ptr input;
		std::shared_ptr<Signals::connection> inputConnection;

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
            //inputConnection.disconnect();
        }
		virtual bool SetInput(const std::string& name_)
		{
			return false;
		}

		virtual bool SetInput(const Parameter::Ptr param)
		{
            if(input)
                input->subscribers--;
			typename ITypedParameter<T>::Ptr castedParam = std::dynamic_pointer_cast<ITypedParameter<T>>(param);
			if (castedParam)
			{
				input = castedParam;
				param->subscribers++;
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
        virtual Parameter::Ptr DeepCopy() const
        {
            return Parameter::Ptr();
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
		std::shared_ptr<Signals::connection> inputConnection;
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
            inputConnection.reset();
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
                inputConnection.reset();
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
        virtual Parameter::Ptr DeepCopy() const
        {
            return Parameter::Ptr();
        }
	};
}
