#pragma once
#include "parameters/Parameter_def.hpp"

#include <parameters/LokiTypeInfo.h>

#include <vector>
#include <memory>

namespace Parameters
{
	class Parameter;
	template<typename T> class ITypedParameter;
	template<typename T> class TypedInputParameter;
	class PARAMETER_EXPORTS IVariableManager
	{
	public:
		virtual void AddParameter(Parameters::Parameter* param) = 0;

		virtual void RemoveParameter(Parameters::Parameter* param) = 0;

		virtual std::vector<Parameters::Parameter*> GetOutputParameters(Loki::TypeInfo type) = 0;
        virtual std::vector<Parameters::Parameter*> GetAllParmaeters() = 0;
        virtual std::vector<Parameters::Parameter*> GetAllOutputParmaeters() = 0;

		template<typename T> std::vector<Parameters::Parameter*> GetOutputParameters();

        virtual Parameters::Parameter* GetOutputParameter(std::string name) = 0;
        virtual Parameters::Parameter* GetParameter(std::string name) = 0;

		// Links an output parameter to an input parameter with either a direct link or a buffered link.
		// Buffered links allow parameters to desync frame number between the producer and the consumer
		virtual void LinkParameters(Parameters::Parameter* output, Parameters::Parameter* input) = 0;
	};

	template<typename T> std::vector<Parameters::Parameter*> IVariableManager::GetOutputParameters()
	{
		return GetOutputParameters(Loki::TypeInfo(typeid(T)));
	}
}
