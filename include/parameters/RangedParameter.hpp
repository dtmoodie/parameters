#pragma once

#include "IRangedParameter.hpp"
#include "TypedParameter.hpp"
#include "ParameterFactory.hpp"
namespace Parameters
{
    template<typename T> class TypedParameterPtr;
    template<typename T> class TypedParameter;
    template<typename T> void rail(T& value, const T& min, const T& max)
    {
        value = std::min(max, value);
        value = std::max(min, value);
    }
    template<typename T> void rail(std::vector<T>& value, const T& min, const T& max)
    {
        for(auto & it : value)
        {
            rail(it, min, max);
        }
    }
    template<typename T> class ITypedRangedParameter: public IRangedParameter
    {
    protected:
        T min_value;
        T max_value;    
        bool range_initialized;
    public:
        ITypedRangedParameter(const T& min_value_, const T& max_value_):
            min_value(min_value_),
            max_value(max_value_),
            range_initialized(true)
        {            
        }
        ITypedRangedParameter():
            range_initialized(false)
        {

        }
        void SetRange(const T& min_value_, const T& max_value_)
        {
            min_value = min_value_;
            max_value = max_value_;
            range_initialized = true;
        }
        bool GetRange(T& min_value_, T& max_value_) const
        {
            if (range_initialized)
            {
                min_value_ = min_value;
                max_value_ = max_value;
                return true;
            }
            return false;
        }
        bool CheckInRange(const T& value) const
        {
            if (range_initialized)
            {
                return value > min_value && value < max_value;
            }
            return true;
        }
    };
    template<typename T> class ITypedRangedParameter<std::vector<T>>: public virtual IRangedParameter
    {
    protected:
        T min_value;
        T max_value;
        bool range_initialized;
    public:
        ITypedRangedParameter(const T& min_value_, const T& max_value_):
            min_value(min_value_),
            max_value(max_value_),
            range_initialized(true)
        {            
        }
        ITypedRangedParameter() :
            range_initialized(false)
        {

        }
        void SetRange(const T& min_value_, const T& max_value_)
        {
            min_value = min_value_;
            max_value = max_value_;
            range_initialized = true;
        }
        bool GetRange(T& min_value_, T& max_value_) const
        {
            if (range_initialized)
            {
                min_value_ = min_value;
                max_value_ = max_value;
                return true;
            }
            return false;
        }
        bool CheckInRange(const std::vector<T>& value) const
        {
            if (range_initialized)
            {
                for (auto& it : value)
                {
                    if (it < min_value || it> max_value)
                        return false;
                }
                return true;
            }
            return true;
        }
    };

    // -----------------------------------------------------------------------------------------------------
    template<typename T> class RangedParameter: public TypedParameter<T>, public ITypedRangedParameter<T>
    {
        static FactoryRegisterer<RangedParameter<T>, T, RangedParameter_c> _ranged_constructor;
    public:
        typedef std::shared_ptr<RangedParameter<T>> Ptr;
        RangedParameter(const T& min_value_ = T(),
                        const T& max_value_ = T(),
                        const std::string& name = "",
                        const T& init = T(),
                        const Parameter::ParameterType& type = Parameter::ParameterType::Control,
                        const std::string& tooltip = "") : 
            TypedParameter<T>(name, init, type, tooltip),
            ITypedRangedParameter<T>(min_value_, max_value_)
        {
            (void)&_ranged_constructor;
        }
        virtual T* Data(long long time_index = -1)
        {
            return TypedParameter<T>::Data(time_index);
        }
        virtual void UpdateData(T& data_, long long time_index = -1, cv::cuda::Stream* stream = nullptr)
        {
            if(data_ > ITypedRangedParameter<T>::min_value && data_ > ITypedRangedParameter<T>::max_value)
            {
                TypedParameter<T>::UpdateData(data_, time_index, stream);
            }            
        }
        virtual void UpdateData(const T& data_, long long time_index = -1, cv::cuda::Stream* stream = nullptr)
        {
            if(data_ > ITypedRangedParameter<T>::min_value && data_ < ITypedRangedParameter<T>::max_value)
            {
                TypedParameter<T>::UpdateData(data_, time_index, stream);
            }            
        }
        virtual void UpdateData(T* data_, long long time_index = -1, cv::cuda::Stream* stream = nullptr)
        {
            if(*data_ > ITypedRangedParameter<T>::min_value && *data_ < ITypedRangedParameter<T>::max_value)
            {
                TypedParameter<T>::UpdateData(data_, time_index, stream);
            }
        }
        virtual Parameter::Ptr DeepCopy() const
        {
            return Parameter::Ptr(new RangedParameter<T>(ITypedRangedParameter<T>::min_value, ITypedRangedParameter<T>::max_value, Parameter::GetName(), TypedParameter<T>::data));
        }
        virtual bool Update(Parameter::Ptr other, cv::cuda::Stream* stream = nullptr)
        {
            auto typed = dynamic_cast<ITypedParameter<T>*>(other.get());
            if (typed)
            {
                if (ITypedRangedParameter<T>::CheckInRange(*(typed->Data())))
                {
                    TypedParameter<T>::UpdateData(typed->Data(), other->GetTimeIndex(), stream);
                }
                return true;
            }
            return false;
        }
        virtual void RailValue()
        {
            rail(TypedParameter<T>::data, ITypedRangedParameter<T>::min_value, ITypedRangedParameter<T>::max_value);
        }
    }; // class RangedParameter
    template<typename T> FactoryRegisterer<RangedParameter<T>, T, RangedParameter_c> RangedParameter<T>::_ranged_constructor;

    template<typename T> class RangedParameter<std::vector<T>>: public TypedParameter<std::vector<T>>, public ITypedRangedParameter<std::vector<T>>
    {
        static FactoryRegisterer<RangedParameter<std::vector<T>>, std::vector<T>, RangedParameter_c> _vector_ranged_constructor;
    public:
        typedef std::shared_ptr<RangedParameter<std::vector<T>>> Ptr;
        RangedParameter(const T& min_value_ = T(),
            const T& max_value_ = T(),
            const std::string& name = "",
            const std::vector<T>& init = std::vector<T>(),
            const Parameter::ParameterType& type = Parameter::ParameterType::Control,
            const std::string& tooltip = "") : 
            TypedParameter<std::vector<T>>(name, init, type, tooltip),
            ITypedRangedParameter<std::vector<T>>(min_value_, max_value_)
        {
            (void)&_vector_ranged_constructor;
        }
        virtual std::vector<T>* Data(long long time_index = -1)
        {
            return TypedParameter<std::vector<T>>::Data(time_index);
        }
        virtual void UpdateData(std::vector<T>& data_, long long time_index = -1, cv::cuda::Stream* stream = nullptr)
        {
            TypedParameter<std::vector<T>>::UpdateData(data_, time_index, stream);
            RailValue();
        }
        virtual void UpdateData(const std::vector<T>& data_, long long time_index = -1, cv::cuda::Stream* stream = nullptr)
        {
            TypedParameter<std::vector<T>>::UpdateData(data_, time_index, stream);
            RailValue();
        }
        virtual void UpdateData(std::vector<T>* data_, long long time_index = -1, cv::cuda::Stream* stream = nullptr)
        {
            TypedParameter<std::vector<T>>::UpdateData(data_, time_index, stream);
            RailValue();
        }
        virtual Parameter::Ptr DeepCopy() const
        {
            return Parameter::Ptr(new RangedParameter<std::vector<T>>(ITypedRangedParameter<std::vector<T>>::min_value, ITypedRangedParameter<std::vector<T>>::max_value, Parameter::GetName(), TypedParameter<std::vector<T>>::data));
        }
        virtual bool Update(Parameter::Ptr other, cv::cuda::Stream* stream = nullptr)
        {
            auto typed = dynamic_cast<ITypedParameter<std::vector<T>>*>(other.get());
            if (typed)
            {
                if (ITypedRangedParameter<std::vector<T>>::CheckInRange(*(typed->Data())))
                {
                    TypedParameter<std::vector<T>>::UpdateData(typed->Data(), other->GetTimeIndex(), stream);
                }
                return true;
            }
            return false;
        }
        virtual void RailValue()
        {
            rail(TypedParameter<std::vector<T>>::data, ITypedRangedParameter<std::vector<T>>::min_value, ITypedRangedParameter<std::vector<T>>::max_value);
        }
    }; // class RangedParameter
    template<typename T> FactoryRegisterer<RangedParameter<std::vector<T>>, std::vector<T>, RangedParameter_c> RangedParameter<std::vector<T>>::_vector_ranged_constructor;
    // -----------------------------------------------------------------------------------------------------
    template<typename T> class RangedParameterPtr: public TypedParameterPtr<T>, public ITypedRangedParameter<T>
    {
    public:
        typedef std::shared_ptr<RangedParameterPtr<T>> Ptr;
        RangedParameterPtr(const T& min_value_,
            const T& max_value_,
            const std::string& name,
            T* init = nullptr,
            const Parameter::ParameterType& type = Parameter::ParameterType::Control,
            const std::string& tooltip = "") :
            TypedParameterPtr<T>(name, init, type, tooltip),
            ITypedRangedParameter<T>(min_value_, max_value_)
        {

        }
        RangedParameterPtr() :
            TypedParameterPtr<T>(""),
            ITypedRangedParameter<T>()
        {

        }
        virtual T* Data(long long time_index = -1)
        {
            return TypedParameterPtr<T>::Data(time_index);
        }
        virtual void UpdateData(T& data, long long time_index = -1, cv::cuda::Stream* stream = nullptr)
        {
            if (ITypedRangedParameter<T>::CheckInRange(data))
            {
                TypedParameterPtr<T>::UpdateData(data, time_index, stream);
            }
        }
        virtual void UpdateData(const T& data, long long time_index = -1, cv::cuda::Stream* stream = nullptr)
        {
            if (ITypedRangedParameter<T>::CheckInRange(data))
            {
                TypedParameterPtr<T>::UpdateData(data, time_index, stream);
            }
        }
        virtual void UpdateData(T* data_, long long time_index = -1, cv::cuda::Stream* stream = nullptr)
        {
            if (data_)
            {
                if (ITypedRangedParameter<T>::CheckInRange(*data_) || TypedParameterPtr<T>::ptr == nullptr)
                {
                    TypedParameterPtr<T>::ptr = data_;
                    Parameter::_current_time_index = time_index;
                    Parameter::changed = true;
                    Parameter::OnUpdate(stream);
                }
            }
            else
            {
                TypedParameterPtr<T>::ptr = data_;
                Parameter::_current_time_index = time_index;
                Parameter::changed = true;
                Parameter::OnUpdate(stream);
            }

            
        }
        virtual bool Update(Parameter::Ptr other, cv::cuda::Stream* stream = nullptr)
        {
            auto typed = dynamic_cast<ITypedParameter<T>*>(other.get());
            if (typed)
            {
                if (typed->Data())
                {
                    if (ITypedRangedParameter<T>::CheckInRange(*typed->Data()))
                    {
                        *TypedParameterPtr<T>::ptr = *(typed->Data());
                        TypedParameterPtr<T>::_current_time_index = other->GetTimeIndex();
                        Parameter::changed = true;
                        Parameter::OnUpdate(stream);
                        return true;
                    }                    
                }
                else
                {
                    TypedParameterPtr<T>::ptr = nullptr;
                }
                
            }
            return false;
        }
        virtual Parameter::Ptr DeepCopy() const
        {
            return Parameter::Ptr(new RangedParameterPtr<T>(ITypedRangedParameter<T>::min_value, ITypedRangedParameter<T>::max_value, Parameter::GetName(), TypedParameterPtr<T>::ptr, Parameter::type, Parameter::tooltip));
        }
        virtual void RailValue()
        {
            if (TypedParameterPtr<T>::ptr)
                rail(*TypedParameterPtr<T>::ptr, ITypedRangedParameter<T>::min_value, ITypedRangedParameter<T>::max_value);
        }
    }; // class RangedParameterPtr
    template<typename T> class RangedParameterPtr<std::vector<T>>: public TypedParameterPtr<std::vector<T>>, public ITypedRangedParameter<std::vector<T>>
    {
    public:
        typedef std::shared_ptr<RangedParameterPtr<std::vector<T>>> Ptr;
        RangedParameterPtr(const T& min_value_,
            const T& max_value_,
            const std::string& name,
            std::vector<T>* init = nullptr,
            const Parameter::ParameterType& type = Parameter::ParameterType::Control,
            const std::string& tooltip = "") :
            TypedParameterPtr<std::vector<T>>(name, init, type, tooltip),
            ITypedRangedParameter<std::vector<T>>(min_value_, max_value_)
        {

        }
        RangedParameterPtr() :
            TypedParameterPtr<std::vector<T>>(""),
            ITypedRangedParameter<std::vector<T>>()
        {

        }
        virtual std::vector<T>* Data(long long time_index = -1)
        {
            return TypedParameterPtr<std::vector<T>>::Data(time_index);
        }
        virtual void UpdateData(std::vector<T>& data, long long time_index = -1, cv::cuda::Stream* stream = nullptr)
        {
            if (ITypedRangedParameter<std::vector<T>>::CheckInRange(data))
            {
                TypedParameterPtr<std::vector<T>>::UpdateData(data, time_index, stream);
            }
        }
        virtual void UpdateData(const std::vector<T>& data, long long time_index = -1, cv::cuda::Stream* stream = nullptr)
        {
            if (ITypedRangedParameter<std::vector<T>>::CheckInRange(data))
            {
                TypedParameterPtr<std::vector<T>>::UpdateData(data, time_index, stream);
            }
        }
        virtual void UpdateData(std::vector<T>* data_, long long time_index = -1, cv::cuda::Stream* stream = nullptr)
        {
            if (data_)
            {
                if (ITypedRangedParameter<std::vector<T>>::CheckInRange(*data_))
                {
                    TypedParameterPtr<std::vector<T>>::ptr = data_;
                    Parameter::_current_time_index = time_index;
                    Parameter::changed = true;
                    Parameter::OnUpdate(stream);
                }
            }
            else
            {
                TypedParameterPtr<std::vector<T>>::ptr = data_;
                Parameter::_current_time_index = time_index;
                Parameter::changed = true;
                Parameter::OnUpdate(stream);
            }


        }
        virtual bool Update(Parameter::Ptr other, cv::cuda::Stream* stream = nullptr)
        {
            auto typed = dynamic_cast<ITypedParameter<std::vector<T>>*>(other.get());
            if (typed)
            {
                if (typed->Data())
                {
                    if (ITypedRangedParameter<std::vector<T>>::CheckInRange(*typed->Data()))
                    {
                        *TypedParameterPtr<std::vector<T>>::ptr = *(typed->Data());
                        Parameter::_current_time_index = other->GetTimeIndex();
                        Parameter::changed = true;
                        Parameter::OnUpdate(stream);
                        return true;
                    }                    
                }
                else
                {
                    TypedParameterPtr<std::vector<T>>::ptr = nullptr;
                }

            }
            return false;
        }
        virtual Parameter::Ptr DeepCopy() const
        {
            return Parameter::Ptr(new RangedParameterPtr<std::vector<T>>(
                                      ITypedRangedParameter<std::vector<T>>::min_value,
                                      ITypedRangedParameter<std::vector<T>>::max_value,
                                      Parameter::GetName(), TypedParameterPtr<std::vector<T>>::ptr, TypedParameterPtr<std::vector<T>>::Parameter::type, Parameter::tooltip));
        }
        virtual void RailValue()
        {
            if (TypedParameterPtr<std::vector<T>>::ptr)
                rail(*TypedParameterPtr<std::vector<T>>::ptr, ITypedRangedParameter<std::vector<T>>::min_value, ITypedRangedParameter<std::vector<T>>::max_value);
        }
    }; // class RangedParameterPtr
}// namespace Parameters
