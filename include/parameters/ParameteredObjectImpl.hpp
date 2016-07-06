#pragma once
#include "ParameteredObject.h"
#include "parameters/Parameter.hpp"
#include "parameters/InputParameter.hpp"
#include "parameters/TypedParameter.hpp"
#include "parameters/TypedInputParameter.hpp"
#include "parameters/RangedParameter.hpp"
#include "parameters/type.h"
#include <boost/lexical_cast.hpp>

#define BEGIN_PARAMS_1(BASE, N_) \
void InitParentParams(){} \
void WrapParentParams() {} \
template<int N> void InitParams_(Signals::_counter_<N> dummy) \
{ \
    InitParams_(Signals::_counter_<N-1>()); \
} \
void InitParams_(Signals::_counter_<N_> dummy) \
{ \
} \
template<int N> void WrapParams_(Signals::_counter_<N> dummy) \
{ \
    WrapParams_(--dummy); \
} \
void WrapParams_(Signals::_counter_<N_> dummy) \
{ \
} \
template<int N> static void getParameterInfo_(std::vector<Parameters::ParameterInfo*>& info, Signals::_counter_<N> dummy) \
{ \
   return getParameterInfo_(info, --dummy);  \
} \
static void getParameterInfo_(std::vector<Parameters::ParameterInfo*>& info, Signals::_counter_<N_> dummy) \
{ \
}

#define BEGIN_PARAMS_2(DERIVED, BASE, N_) \
void InitParentParams(){ BASE::InitializeExplicitParamsToDefault();} \
void WrapParentParams() {BASE::WrapExplicitParams();} \
template<int N> void InitParams_(Signals::_counter_<N> dummy) \
{ \
    InitParams_(Signals::_counter_<N-1>()); \
} \
void InitParams_(Signals::_counter_<N_> dummy) \
{ \
} \
template<int N> void WrapParams_(Signals::_counter_<N> dummy) \
{ \
    WrapParams_(--dummy); \
} \
void WrapParams_(Signals::_counter_<N_> dummy) \
{ \
} \
template<int N> static void getParameterInfo_(std::vector<Parameters::ParameterInfo*>& info, Signals::_counter_<N> dummy) \
{ \
   return getParameterInfo_(info, --dummy);  \
} \
static void getParameterInfo_(std::vector<Parameters::ParameterInfo*>& info, Signals::_counter_<N_> dummy) \
{ \
}

#define DEFINE_PARAM_3(type, name, N) \
void InitParams_(Signals::_counter_<N> dummy) \
{ \
    InitParams_(--dummy); \
} \
void WrapParams_(Signals::_counter_<N> dummy) \
{ \
    name##_param.SetName(#name); \
    name##_param.UpdateData(&name); \
    this->addParameter(&name##_param); \
    WrapParams_(--dummy); \
} \
static void getParameterInfo_(std::vector<Parameters::ParameterInfo*>& info, Signals::_counter_<N> dummy) \
{ \
    static Parameters::ParameterInfo s_info{Loki::TypeInfo(typeid(type)), #name}; \
    info.push_back(&s_info); \
    getParameterInfo_(info, --dummy); \
}

#define DEFINE_PARAM_4(type, name, initial_value, N) \
void InitParams_(Signals::_counter_<N> dummy) \
{ \
    name = initial_value; \
    InitParams_(--dummy); \
} \
void WrapParams_(Signals::_counter_<N> dummy) \
{ \
    name##_param.SetName(#name); \
    name##_param.UpdateData(&name); \
    this->addParameter(&name##_param); \
    WrapParams_(--dummy); \
} \
static void getParameterInfo_(std::vector<Parameters::ParameterInfo*>& info, Signals::_counter_<N> dummy) \
{ \
    static Parameters::ParameterInfo s_info{Loki::TypeInfo(typeid(type)), #name}; \
    info.push_back(&s_info); \
    getParameterInfo_(info, --dummy); \
}

#define DEFINE_PARAM_5(type, name, min, max, N) \
void InitParams_(Signals::_counter_<N> dummy) \
{ \
    InitParams_(Signals::_counter_<N-1>()); \
} \
void WrapParams_(Signals::_counter_<N> dummy) \
{ \
    name##_param.SetName(#name); \
    name##_param.UpdateData(&name); \
    name##_param.SetRange(min, max); \
    this->addParameter(&name##_param); \
    WrapParams_(--dummy); \
} \
static void getParameterInfo_(std::vector<Parameters::ParameterInfo*>& info, Signals::_counter_<N> dummy) \
{ \
    static Parameters::ParameterInfo s_info{Loki::TypeInfo(typeid(type)), #name}; \
    info.push_back(&s_info); \
    getParameterInfo_(info, --dummy); \
}

#define DEFINE_PARAM_6(type, name, min, max, initial_value, N) \
void InitParams_(Signals::_counter_<N> dummy) \
{ \
    name = initial_value; \
    InitParams_(Signals::_counter_<N-1>()); \
} \
void WrapParams_(Signals::_counter_<N> dummy) \
{ \
    name##_param.SetName(#name); \
    name##_param.UpdateData(&name); \
    name##_param.SetRange(min, max); \
    this->addParameter(&name##_param); \
    WrapParams_(--dummy); \
} \
static void getParameterInfo_(std::vector<Parameters::ParameterInfo*>& info, Signals::_counter_<N> dummy) \
{ \
    static Parameters::ParameterInfo s_info{Loki::TypeInfo(typeid(type)), #name}; \
    info.push_back(&s_info); \
    getParameterInfo_(info, --dummy); \
}

#define PARAM_2(type, name) DEFINE_PARAM_3(type, name, __COUNTER__); type name; Parameters::TypedParameterPtr<type> name##_param;
#define PARAM_3(type, name, initial_value) DEFINE_PARAM_4(type, name, initial_value, __COUNTER__); type name; Parameters::TypedParameterPtr<type> name##_param;
#define PARAM_4(type, name, min, max) DEFINE_PARAM_5(type, name, min, max, __COUNTER__); type name; Parameters::RangedParameterPtr<type> name##_param;
#define PARAM_5(type, name, min, max, initial_value) DEFINE_PARAM_6(type, name, min, max, initial_value, __COUNTER__); type name; Parameters::RangedParameterPtr<type> name##_param;


#define PARAM_TOOLTIP_(NAME, TOOLTIP, N) \
static void getParameterInfo_(std::vector<Parameters::ParameterInfo*>& info, Signals::_counter_<N> dummy) \
{ \
    getParameterInfo_(info, --dummy); \
    for(auto& ptr : info) \
    { \
        if(ptr->name == #NAME && ptr->tooltip.size() == 0) \
        { \
            ptr->tooltip = TOOLTIP; \
        } \
    } \
}

#define PARAM_DESCRIPTION_(NAME, DESCRIPTION, N) \
static void getParameterInfo_(std::vector<Parameters::ParameterInfo*>& info, Signals::_counter_<N> dummy) \
{ \
    getParameterInfo_(info, --dummy); \
    for(auto& ptr : info) \
    { \
        if(ptr->name == #NAME && ptr->description.size() == 0) \
        { \
            ptr->description = DESCRIPTION; \
        } \
    } \
}

#define END_PARAMS_(N) \
virtual void InitializeExplicitParamsToDefault() \
{  \
    InitParentParams(); \
    InitParams_<N-1>(Signals::_counter_<N-1>()); \
} \
virtual void WrapExplicitParams() \
{ \
    WrapParentParams(); \
    WrapParams_(Signals::_counter_<N-1>()); \
} \
static std::vector<Parameters::ParameterInfo*> getParameterInfoStatic() \
{ \
    std::vector<Parameters::ParameterInfo*> output; \
    getParameterInfo_(output, Signals::_counter_<N-1>()); \
    return output; \
} \
virtual std::vector<Parameters::ParameterInfo*> getParameterInfo() const \
{ \
    return getParameterInfoStatic(); \
}


#define PARAM_TOOLTIP(NAME, TOOLTIP) PARAM_TOOLTIP_(NAME, TOOLTIP, __COUNTER__)
#define PARAM_DESCRIPTION(NAME, DESCRIPTION) PARAM_DESCRIPTION_(NAME, DESCRIPTION, __COUNTER__)

#ifdef _MSC_VER
#define PARAM(...) BOOST_PP_CAT( BOOST_PP_OVERLOAD(PARAM_, __VA_ARGS__ )(__VA_ARGS__), BOOST_PP_EMPTY() )
#define BEGIN_PARAMS(...) BOOST_PP_CAT( BOOST_PP_OVERLOAD(BEGIN_PARAMS_, __VA_ARGS__ )(__VA_ARGS__, __COUNTER__), BOOST_PP_EMPTY() )
#else
#define PARAM(...) BOOST_PP_OVERLOAD(PARAM_, __VA_ARGS__ )(__VA_ARGS__)
#define BEGIN_PARAMS(...) BOOST_PP_OVERLOAD(BEGIN_PARAMS_, __VA_ARGS__ )(__VA_ARGS__, __COUNTER__)
#endif

#define END_PARAMS END_PARAMS_(__COUNTER__);
//#define BEGIN_PARAMS BEGIN_PARAMS_(__COUNTER__);
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
        if(!exists(name))
        {
            auto input_param = new Parameters::TypedInputParameter<T>(name);
            addParameter(typename Parameters::TypedInputParameter<T>::Ptr(input_param));
            return input_param;
        }
        return dynamic_cast<TypedInputParameter<T>*>(getParameterOptional<T>(name));
    }

    template<typename T>
    bool ParameteredObject::updateInputQualifier(const std::string& name, const std::function<bool(Parameters::Parameter*)>& qualifier_)
    {
        auto param = getParameter(name);
        if (param && param->type & Parameters::Parameter::Input)
        {
            Parameters::InputParameter* inputParam = dynamic_cast<Parameters::InputParameter*>(param);
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
        Parameters::ITypedParameter<T>* param;
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
        ASSERT_NE(typedParam, nullptr) << "Failed to cast parameter to the appropriate type, requested type: " << TypeInfo::demangle(typeid(T).name()) << " actual type: " << param->GetTypeInfo().name();

        return typedParam;
    }

    template<typename T>
    typename Parameters::ITypedParameter<T>* ParameteredObject::getParameter(int idx)
    {
        auto param = getParameter(idx);
        if (param == nullptr)
            throw cv::Exception(0, "Failed to get parameter by index " + boost::lexical_cast<std::string>(idx), __FUNCTION__, __FILE__, __LINE__);

        auto typedParam = dynamic_cast<typename Parameters::ITypedParameter<T>*>(param);
        ASSERT_NE(typedParam, nullptr) << "Failed to cast parameter to the appropriate type, requested type: " << TypeInfo::demangle(typeid(T).name()) << " actual type: " << param->GetTypeInfo().name();
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
        ASSERT_NE(typedParam, nullptr) << "Failed to cast parameter to the appropriate type, requested type: " << TypeInfo::demangle(typeid(T).name()) << " actual type: " << param->GetTypeInfo().name();

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
