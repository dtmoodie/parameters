#pragma once

#include "Parameter.hpp"
namespace Parameters
{


	template<typename T> class ITypedParameter : public Parameter
	{
	public:
		typedef std::shared_ptr<ITypedParameter<T>> Ptr;
		virtual T* Data() = 0;
		virtual void UpdateData(T& data_) = 0;
		virtual void UpdateData(const T& data_) = 0;
		virtual void UpdateData(T* data_) = 0;
		ITypedParameter(const std::string& name, const ParameterType& type = Parameter::Control, const std::string& tooltip = "") :
			Parameter(name, type, tooltip){}
		virtual Loki::TypeInfo GetTypeInfo()
		{
			return Loki::TypeInfo(typeid(T));
		}
	};
}