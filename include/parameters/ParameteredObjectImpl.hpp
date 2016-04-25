#pragma once
#include "ParameteredObject.h"
#include "parameters/Parameter.hpp"
#include "parameters/InputParameter.hpp"
#include "parameters/TypedParameter.hpp"
#include "parameters/TypedInputParameter.hpp"
#include "parameters/RangedParameter.hpp"
#include "parameters/type.h"
#include <boost/lexical_cast.hpp>

#define BEGIN_PARAMS \
template<int N> struct ParamRegisterer \
{ \
	template<class C> static void Register(C* obj){} \
}; \
template<> struct ParamRegisterer<__COUNTER__> \
{ \
	template<class C> static void Register(C* obj){} \
};\

#define DEFINE_PARAM_3(type, name, N) \
template<> struct ParamRegisterer<N> \
{ \
	template<class C> static void Register(C* obj) { \
		obj->##name##_param.SetName(#name); \
		obj->##name##_param.UpdateData(&obj->name); \
		obj->addParameter(&obj->##name##_param); \
		ParamRegisterer<N-1>::Register(obj); \
	} \
};

#define DEFINE_PARAM_4(type, name, initial_value, N) \
template<> struct ParamRegisterer<N> \
{ \
	template<class C> static void Register(C* obj) { \
		obj->name = initial_value; \
		obj->##name##_param.SetName(#name); \
		obj->##name##_param.UpdateData(&obj->name); \
		obj->addParameter(&obj->##name##_param); \
		ParamRegisterer<N-1>::Register(obj); \
	} \
};

#define DEFINE_PARAM_5(type, name, min, max, N) \
template<> struct ParamRegisterer<N> \
{ \
	template<class C> static void Register(C* obj) { \
		obj->##name##_param.SetName(#name); \
		obj->##name##_param.SetRange(min, max); \
		obj->##name##_param.UpdateData(&obj->name); \
		obj->addParameter(&obj->##name##_param); \
		ParamRegisterer<N-1>::Register(obj); \
	} \
};

#define DEFINE_PARAM_6(type, name, min, max, initial_value, N) \
template<> struct ParamRegisterer<N> \
{ \
	template<class C> static void Register(C* obj) { \
		obj->name = initial_value; \
		obj->##name##_param.SetName(#name); \
		obj->##name##_param.SetRange(min, max); \
		obj->##name##_param.UpdateData(&obj->name); \
		obj->addParameter(&obj->##name##_param); \
		ParamRegisterer<N-1>::Register(obj); \
	} \
};

#define PARAM_2(type, name) DEFINE_PARAM_3(type, name, __COUNTER__); type name; TypedParameterPtr<type> ##name##_param;
#define PARAM_3(type, name, initial_value) DEFINE_PARAM_4(type, name, initial_value, __COUNTER__); type name; TypedParameterPtr<type> ##name##_param;
#define PARAM_4(type, name, min, max) DEFINE_PARAM_5(type, name, min, max, __COUNTER__); type name; RangedParameterPtr<type> ##name##_param;
#define PARAM_5(type, name, min, max, initial_value) DEFINE_PARAM_6(type, name, min, max, initial_value, __COUNTER__); type name; RangedParameterPtr<type> ##name##_param;

#ifdef _MSC_VER
#define PARAM(...) BOOST_PP_CAT( BOOST_PP_OVERLOAD(PARAM_, __VA_ARGS__ )(__VA_ARGS__), BOOST_PP_EMPTY() )
#else
#define PARAM(...) BOOST_PP_CAT( BOOST_PP_OVERLOAD(PARAM_, __VA_ARGS__ )(__VA_ARGS__), BOOST_PP_EMPTY() )
#endif

#define END_PARAMS_(N) virtual void RegisterAllParams(){ ParamRegisterer<N- 1>::Register(this);}

#define END_PARAMS END_PARAMS_(__COUNTER__);

namespace Parameters
{
	template<typename T>
	Parameters::ITypedParameter<T>* ParameteredObject::registerParameter(const std::string& name, T* data)
	{
		typename Parameters::TypedParameterPtr<T>::Ptr param(new Parameters::TypedParameterPtr<T>(name, data));
		addParameter(param);
		return param.get();
	}

	template<typename T>
	Parameters::ITypedParameter<T>* ParameteredObject::addParameter(const std::string& name, const T& data)
	{
		typename Parameters::TypedParameter<T>::Ptr param(new Parameters::TypedParameter<T>(name, data));
		addParameter(param);
		return param.get();
	}

	template<typename T> 
	Parameters::ITypedParameter<T>* ParameteredObject::addParameter(const std::string& name, const T& data, const T& min_value_, const T& max_value_)
	{
		typename Parameters::RangedParameter<T>::Ptr param(new Parameters::RangedParameter<T>(min_value_, max_value_, name, data));
		addParameter(param);
		return param.get();
	}

	template<typename T>
	Parameters::ITypedParameter<T>* ParameteredObject::addIfNotExist(const std::string& name, const T& data)
	{
		if (!exists(name))
		{
			typename Parameters::TypedParameter<T>::Ptr param(new Parameters::TypedParameter<T>(name, data));
			addParameter(param);
			return param.get();
		}
		return nullptr;
	}

	template<typename T>
	Parameters::TypedInputParameter<T>* ParameteredObject::addInputParameter(const std::string& name)
	{
		auto input_param = new Parameters::TypedInputParameter<T>(name);
		addParameter(typename Parameters::TypedInputParameter<T>::Ptr(input_param));
		return input_param;
	}

	template<typename T>
	bool ParameteredObject::updateInputQualifier(const std::string& name, const std::function<bool(Parameters::Parameter*)>& qualifier_)
	{
		auto param = getParameter(name);
		if (param && param->type & Parameters::Parameter::Input)
		{
			Parameters::InputParameter* inputParam = dynamic_cast<Parameters::InputParameter*>(param.get());
			if (inputParam)
			{
				inputParam->SetQualifier(qualifier_);
				return true;
			}
		}
		return false;
	}

	template<typename T>
	bool ParameteredObject::updateInputQualifier(int idx, const std::function<bool(Parameters::Parameter*)>& qualifier)
	{
		auto param = getParameter<T>(idx);
		if (param && param->type & Parameters::Parameter::Input)
		{
			Parameters::InputParameter* inputParam = dynamic_cast<Parameters::InputParameter*>(param);
			if (inputParam)
			{
				inputParam->SetQualifier(qualifier);
				return true;
			}
		}
		return false;
	}

	template<typename T>
	Parameters::ITypedParameter<T>* ParameteredObject::updateParameterPtr(const std::string& name, T* data, long long timestamp, cv::cuda::Stream* stream)
	{
		typename Parameters::ITypedParameter<T>* param = nullptr;
		param = getParameterOptional<T>(name);
		if (param == nullptr)
		{
			LOG(debug) << name << " doesn't exist, adding";
			return registerParameter<T>(name, data);
		}
		if (auto non_ref_param = dynamic_cast<TypedParameter<T>*>(param))
		{
			// Parameter exists but is of the wrong type, due to being loaded from a file as a generic typed parameter
			*data = *non_ref_param->Data();
			// Find the incorrectly typed parameter
			for (int i = 0; i < _parameters.size(); ++i)
			{
				if (_parameters[i]->GetName() == name)
				{
					// check if it's an implicit or explicit parameter
					if (std::count_if(_implicit_parameters.begin(), _implicit_parameters.end(), [this, i](const Parameters::Parameter::Ptr& param) {return param.get() == _parameters[i]; }))
					{
						auto itr = std::find_if(_implicit_parameters.begin(), _implicit_parameters.end(), [this, i](const Parameters::Parameter::Ptr& param) {return param.get() == _parameters[i]; });
						itr->reset(new Parameters::TypedParameterPtr<T>(name, data));
						_parameters[i] = itr->get();
					}
					else
					{
						if (std::count(_explicit_parameters.begin(), _explicit_parameters.end(), _parameters[i]))
						{
							return nullptr;
						}
						else
						{
							return nullptr;
						}
					}
				}
			}
		}
		param->UpdateData(data, timestamp, stream);
        if(param->type != Parameters::Parameter::Output)
		    onUpdate(param,stream);
		return param;
	}

	template<typename T>
	Parameters::ITypedParameter<T>* ParameteredObject::updateParameter(const std::string& name, const T& data, long long timestamp, cv::cuda::Stream* stream)
	{
		typename Parameters::ITypedParameter<T>* param;
		param = getParameterOptional<T>(name);
		if (param == nullptr)
		{
			LOG(debug) << "Parameter named \"" << name << "\" with type " << Loki::TypeInfo(typeid(T)).name() << " doesn't exist, adding";
			return addParameter<T>(name, data);
		}
		param->UpdateData(data, timestamp, stream);
        if(param->type != Parameters::Parameter::Output)
		    onUpdate(param, stream);
		return param;
	}

	template<typename T>
	Parameters::ITypedParameter<T>* ParameteredObject::updateParameter(size_t idx, const T data, long long timestamp, cv::cuda::Stream* stream)
	{
		DOIF_LOG_PASS(idx > _parameters.size() || idx < 0, return nullptr, debug);
		
		auto param = dynamic_cast<Parameters::ITypedParameter<T>*>(_parameters[idx]);

		DOIF_LOG_PASS(param == nullptr, return nullptr, debug);
		param->UpdateData(data, timestamp, stream);
        if(param->type != Parameters::Parameter::Output)
		    onUpdate(param, stream);
		return param;
	}

	template<typename T> 
	Parameters::ITypedParameter<T>* ParameteredObject::updateParameter(const std::string& name, const T& data, const T& min_value_, const T& max_value_, long long timestamp, cv::cuda::Stream* stream)
	{
		typename Parameters::ITypedParameter<T>* param;
		param = getParameterOptional<T>(name);
		if (param == nullptr)
		{
			LOG(debug) << "Parameter named \"" << name << "\" with type " << Loki::TypeInfo(typeid(T)).name() << " doesn't exist, adding";
			auto output = addParameter<T>(name, data, min_value_, max_value_);
			output->SetTimeIndex(timestamp);
			return output;
		}

		auto ranged_param = dynamic_cast<Parameters::ITypedRangedParameter<T>*>(param);
		DOIF_LOG_FAIL(ranged_param, ranged_param->SetRange(min_value_, max_value_) , debug);

		param->UpdateData(data, timestamp, stream);
		if (param->type != Parameters::Parameter::Output)
			onUpdate(param, stream);
		return param;
	}

	template<typename T> 
	Parameters::ITypedParameter<T>* ParameteredObject::updateParameter(size_t idx, const T& data, const T& min_value_, const T& max_value_, long long timestamp, cv::cuda::Stream* stream)
	{
		DOIF_LOG_PASS(idx > _parameters.size() || idx < 0, return nullptr, debug);

		auto ranged_param = dynamic_cast<Parameters::ITypedRangedParameter<T>*>(_parameters[idx]);
		DOIF_LOG_FAIL(ranged_param, ranged_param->SetRange(min_value_, max_value_), debug);

		auto param = dynamic_cast<Parameters::ITypedParameter<T>*>(_parameters[idx]);

		if (param == NULL)
			return nullptr;

		param->UpdateData(data, timestamp, stream);
		if (param->type != Parameters::Parameter::Output)
			onUpdate(param, stream);
		return param;
	}
	template<typename T> std::vector<ITypedParameter<T>*> ParameteredObject::getParameters()
	{
		std::vector<ITypedParameter<T>*> output;
		auto all_params = getParameters();
		for (auto itr : all_params)
		{
			ITypedParameter<T>* typed = nullptr;
			if (itr->GetTypeInfo() == Loki::TypeInfo(typeid(T)))
			{
				if(typed = dynamic_cast<ITypedParameter<T>*>(itr))
					output.push_back(typed);
			}
				
				
		}
		return output;
	}
	template<typename T>
	typename Parameters::ITypedParameter<T>* ParameteredObject::getParameter(std::string name)
	{
		auto param = getParameter(name);
		if (param == nullptr)
		{
			throw cv::Exception(0, "Failed to get parameter by name " + name, __FUNCTION__, __FILE__, __LINE__);
			return nullptr;
		}
		auto typedParam = dynamic_cast<typename Parameters::ITypedParameter<T>*>(param);
		if (typedParam == nullptr)
			throw cv::Exception(0, "Failed to cast parameter to the appropriate type, requested type: " +
			TypeInfo::demangle(typeid(T).name()) + " parameter actual type: " + param->GetTypeInfo().name(), __FUNCTION__, __FILE__, __LINE__);

		return typedParam;
	}

	template<typename T>
	typename Parameters::ITypedParameter<T>* ParameteredObject::getParameter(int idx)
	{
		auto param = getParameter(idx);
		if (param == nullptr)
			throw cv::Exception(0, "Failed to get parameter by index " + boost::lexical_cast<std::string>(idx), __FUNCTION__, __FILE__, __LINE__);

		auto typedParam = dynamic_cast<typename Parameters::ITypedParameter<T>*>(param);
		if (typedParam == nullptr)
			throw cv::Exception(0, "Failed to cast parameter to the appropriate type, requested type: " +
			TypeInfo::demangle(typeid(T).name()) + " parameter actual type: " + param->GetTypeInfo().name(), __FUNCTION__, __FILE__, __LINE__);
		return typedParam;
	}


	template<typename T>
	typename Parameters::ITypedParameter<T>* ParameteredObject::getParameterOptional(std::string name)
	{
		auto param = getParameterOptional(name);
		if (param == nullptr)
		{
			return nullptr;
		}
		auto typedParam = dynamic_cast<typename Parameters::ITypedParameter<T>*>(param);
		if (typedParam == nullptr)
			LOG(debug) << "Failed to cast parameter to the appropriate type, requested type: " <<
			TypeInfo::demangle(typeid(T).name()) << " parameter actual type: " << param->GetTypeInfo().name();

		return typedParam;
	}

	template<typename T>
	typename Parameters::ITypedParameter<T>* ParameteredObject::getParameterOptional(int idx)
	{
		auto param = getParameterOptional(idx);
		if (param == nullptr)
			return typename Parameters::ITypedParameter<T>::Ptr(); // Return a nullptr

		return dynamic_cast<typename Parameters::ITypedParameter<T>*>(param);
	}
}