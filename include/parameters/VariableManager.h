#pragma once

#include "parameters/IVariableManager.h"

#include <signals/connection.h>
#include <map>

namespace Parameters
{
    class PARAMETER_EXPORTS VariableManager: public IVariableManager
    {
        std::map<std::string, Parameters::Parameter*> _parameters;
		std::map<std::string, std::shared_ptr<Signals::connection>> _delete_connections;
    public:
        virtual void AddParameter(Parameters::Parameter* param);
		virtual void RemoveParameter(Parameters::Parameter* param);
		virtual std::vector<Parameters::Parameter*> GetOutputParameters(Loki::TypeInfo type);
        virtual Parameters::Parameter* GetOutputParameter(std::string name);
		virtual void LinkParameters(Parameters::Parameter* output, Parameters::Parameter* input);
    };

	// By default uses a buffered connection when linking two parameters together
	class PARAMETER_EXPORTS BufferedVariableManager : public VariableManager
	{
	};
}
