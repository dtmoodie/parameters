#include "parameters/ParameteredObject.h"
#include "parameters/ParameteredObjectImpl.hpp"
#include "parameters/IVariableManager.h"

#include "Signals/signal_manager.h"

#include <signals/logging.hpp>

using namespace Parameters;

ParameteredObject::ParameteredObject()
{
	_sig_parameter_updated = nullptr;
	_sig_parameter_added = nullptr;
    _variable_manager = nullptr;
}

ParameteredObject::~ParameteredObject()
{
    _callback_connections.clear();
    if(_variable_manager)
    {
        for(int i = 0; i < _parameters.size(); ++i)
        {
            _variable_manager->RemoveParameter(_parameters[i]);
        }
    }
    _parameters.clear();
}

void ParameteredObject::setup_signals(Signals::signal_manager* manager)
{
	_sig_parameter_updated = manager->get_signal<void(ParameteredObject*)>("parameter_updated", this, "Emitted when a parameter is updated from ui");
	_sig_parameter_added = manager->get_signal<void(ParameteredObject*)>("parameter_added", this, "Emitted when a new parameter is added");
}

void ParameteredObject::SetupVariableManager(IVariableManager* manager)
{
    _variable_manager = manager;
	if (_variable_manager)
	{
		for (auto param : _parameters)
		{
			_variable_manager->AddParameter(param);
		}
	}
}

Parameters::IVariableManager* ParameteredObject::GetVariableManager()
{
    return _variable_manager;
}

Parameter* ParameteredObject::addParameter(Parameter::Ptr param)
{
	std::lock_guard<std::recursive_mutex> lock(mtx);
	DOIF_LOG_FAIL(_sig_parameter_added, (*_sig_parameter_updated)(this), warning);
    DOIF_LOG_FAIL(_variable_manager, _variable_manager->AddParameter(param.get()), debug);
	_callback_connections.push_back(param->RegisterNotifier(std::bind(&ParameteredObject::onUpdate, this, param.get(), std::placeholders::_1)));
    _parameters.push_back(param.get());
	_implicit_parameters.push_back(param);
    return param.get();
}

Parameter* ParameteredObject::addParameter(Parameter* param)
{
	std::lock_guard<std::recursive_mutex> lock(mtx);
	// Check if it already exists
	auto itr = std::find(_explicit_parameters.begin(), _explicit_parameters.end(), param);
	if (itr != _explicit_parameters.end())
	{
		// an implicit parameter already exists with this exact pointer, do nothing
		return param;
	}
	auto existing_param = getParameterOptional(param->GetName());
	if (existing_param != nullptr)
	{
		// Parameter has already been added, either implicitly or explicitly, do nothing
		LOG(debug) << "Parameter with name " << param->GetName() << " already exists but not as an explicitly defined parameter";
		return param;
	}
	DOIF_LOG_FAIL(_sig_parameter_added, (*_sig_parameter_updated)(this), warning);
	DOIF_LOG_FAIL(_variable_manager, _variable_manager->AddParameter(param), debug);
	
	_callback_connections.push_back(param->RegisterNotifier(std::bind(&ParameteredObject::onUpdate, this, param, std::placeholders::_1)));
	_parameters.push_back(param);
	_explicit_parameters.push_back(param);
	return param;
}


Parameter* updateParameter(std::shared_ptr<Parameter> parameter)
{

	return parameter.get();
}
void ParameteredObject::RemoveParameter(std::string name)
{
    for(auto itr = _parameters.begin(); itr != _parameters.end(); ++itr)
    {
        if((*itr)->GetName() == name)
        {
            DOIF_LOG_FAIL(_variable_manager, _variable_manager->RemoveParameter(*itr), debug);
            itr = _parameters.erase(itr);
        }
    }
}

void ParameteredObject::RemoveParameter(size_t index)
{
    if(index < _parameters.size())
    {
        DOIF_LOG_FAIL(_variable_manager, _variable_manager->RemoveParameter(_parameters[index]), debug);
        _parameters.erase(_parameters.begin() + index);
    }
}

Parameter* ParameteredObject::getParameter(int idx)
{
    CV_Assert(idx >= 0 && idx < _parameters.size());
    return _parameters[idx];
}

Parameter* ParameteredObject::getParameter(const std::string& name)
{
    for (auto& itr : _parameters)
    {
        if (itr->GetName() == name)
        {
            return itr;
        }
    }
    throw std::string("Unable to find parameter by name: " + name);
}

Parameter* ParameteredObject::getParameterOptional(int idx)
{
    if (idx < 0 || idx >= _parameters.size())
    {
		LOG(debug) << "Requested index " << idx << " out of bounds " << _parameters.size();
		return nullptr;
    }
    return _parameters[idx];
}

Parameter* ParameteredObject::getParameterOptional(const std::string& name)
{
    for (auto& itr : _parameters)
    {
        if (itr->GetName() == name)
        {
            return itr;
        }
    }
    LOG(debug) << "Unable to find parameter by name: " << name;
	return nullptr;
}

void ParameteredObject::RegisterAllParams()
{
}

std::vector<Parameter*> ParameteredObject::getParameters()
{
    return _parameters;
}

std::vector<Parameter*> ParameteredObject::getDisplayParameters()
{
	return getParameters();
}

void ParameteredObject::RegisterParameterCallback(int idx, const std::function<void(cv::cuda::Stream*)>& callback, bool lock_param, bool lock_object)
{
    RegisterParameterCallback(getParameter(idx), callback, lock_param, lock_object);
}

void ParameteredObject::RegisterParameterCallback(const std::string& name, const std::function<void(cv::cuda::Stream*)>& callback, bool lock_param, bool lock_object)
{
    RegisterParameterCallback(getParameter(name), callback, lock_param, lock_object);
}

void ParameteredObject::RegisterParameterCallback(Parameter* param, const std::function<void(cv::cuda::Stream*)>& callback, bool lock_param, bool lock_object)
{
    if (lock_param && !lock_object)
    {
        _callback_connections.push_back(param->RegisterNotifier(std::bind(&ParameteredObject::RunCallbackLockParameter, this, std::placeholders::_1, callback, &param->mtx())));
        return;
    }
    if (lock_object && !lock_param)
    {
		_callback_connections.push_back(param->RegisterNotifier(std::bind(&ParameteredObject::RunCallbackLockObject, this, std::placeholders::_1, callback)));
        return;
    }
    if (lock_object && lock_param)
    {

		_callback_connections.push_back(param->RegisterNotifier(std::bind(&ParameteredObject::RunCallbackLockBoth, this, std::placeholders::_1, callback, &param->mtx())));
        return;
    }
	_callback_connections.push_back(param->RegisterNotifier(callback));    
}

void ParameteredObject::onUpdate(Parameters::Parameter* param, cv::cuda::Stream* stream)
{
	DOIF_LOG_FAIL(_sig_parameter_updated != nullptr, (*_sig_parameter_updated)(this), debug);
}

void ParameteredObject::RunCallbackLockObject(cv::cuda::Stream* stream, const std::function<void(cv::cuda::Stream*)>& callback)
{
	std::lock_guard<std::recursive_mutex> lock(mtx);
    callback(stream);
}

void ParameteredObject::RunCallbackLockParameter(cv::cuda::Stream* stream, const std::function<void(cv::cuda::Stream*)>& callback, std::recursive_mutex* paramMtx)
{
	std::lock_guard<std::recursive_mutex> lock(*paramMtx);
    callback(stream);
}

void ParameteredObject::RunCallbackLockBoth(cv::cuda::Stream* stream, const std::function<void(cv::cuda::Stream*)>& callback, std::recursive_mutex* paramMtx)
{
	std::lock_guard<std::recursive_mutex> lock(mtx);
	std::lock_guard<std::recursive_mutex> lock_(*paramMtx);
    callback(stream);
}

bool ParameteredObject::exists(const std::string& name)
{
    return getParameterOptional(name) != nullptr;
}

bool ParameteredObject::exists(size_t index)
{
    return index < _parameters.size();
}
