#include "parameters/VariableManager.h"
#include "parameters/Parameter.hpp"
#include "parameters/InputParameter.hpp"

#include <signals/logging.hpp>
using namespace Parameters;

void VariableManager::AddParameter(Parameters::Parameter* param)
{
    _parameters[param->GetTreeName()] = param;
	_delete_connections[param->GetTreeName()] = param->RegisterDeleteNotifier(std::bind(&VariableManager::RemoveParameter, this, std::placeholders::_1));
}
void VariableManager::RemoveParameter(Parameters::Parameter* param)
{
	_parameters.erase(param->GetTreeName());
	_delete_connections.erase(param->GetTreeName());
}
std::vector<Parameters::Parameter*> VariableManager::GetOutputParameters(Loki::TypeInfo type)
{
    std::vector<Parameters::Parameter*> valid_outputs;
    for(auto itr = _parameters.begin(); itr != _parameters.end(); ++itr)
    {
        if(itr->second->GetTypeInfo() == type && itr->second->type & Parameters::Parameter::Output)
        {
            valid_outputs.push_back(itr->second);
        }
    }
    return valid_outputs;
}
Parameters::Parameter* VariableManager::GetOutputParameter(std::string name)
{
    auto itr = _parameters.find(name);
    if(itr != _parameters.end())
    {
        return itr->second;
    }
    LOG(debug) << "Unable to find parameter named " << name;
	return nullptr;
}
void VariableManager::LinkParameters(Parameters::Parameter* output, Parameters::Parameter* input)
{
	if(auto input_param = dynamic_cast<Parameters::InputParameter*>(input))
		input_param->SetInput(output);
}