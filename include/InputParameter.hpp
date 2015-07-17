#pragma once
#include "Parameter_def.hpp"
#include <string>
#include <boost/function.hpp>
#include "Parameter.hpp"
#include "LokiTypeInfo.h"

namespace Parameters
{
	class Parameter_EXPORTS InputParameter
	{
	protected:
		boost::function<bool(Parameter*)> qualifier;
	public:
		typedef std::shared_ptr<InputParameter> Ptr;
		virtual bool SetInput(const std::string& name_) = 0;
		virtual bool SetInput(const Parameter::Ptr param) = 0;
		virtual bool AcceptsInput(const Parameter::Ptr param) = 0;
		virtual bool AcceptsType(const Loki::TypeInfo& type) = 0;
		virtual void SetQualifier(const boost::function<bool(Parameter*)>& f)
		{
			qualifier = f;
		}
	};
}