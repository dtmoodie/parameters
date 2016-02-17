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
