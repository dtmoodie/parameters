#pragma once
#include "Parameter_def.hpp"
#include "LokiTypeInfo.h"
#include <functional>
#include <map>
namespace Parameters
{
    class Parameter;
    // Only include types that it makes sense to dynamically construct.
    // No reason to create a TypedParameterPtr most of the time because it is used to wrap
    // user owned data
    enum ParameterTypeFlags
    {
        TypedParameter_c = 0,
        CircularBuffer_c = 1,
        Map_c = 2,
        ConstMap_c = 3,
        RangedParameter_c = 4
    };
    class PARAMETER_EXPORTS ParameterFactory
    {
    public:
        typedef std::function<Parameter*(void)> create_f;
        static ParameterFactory* instance();
        
        // Each specialization of a parameter must have a unique type
        void RegisterConstructor(Loki::TypeInfo data_type, create_f function, int parameter_type);
        void RegisterConstructor(Loki::TypeInfo parameter_type, create_f function);

        Parameter* create(Loki::TypeInfo data_type, int parameter_type); // Give datatype and parameter type enum
        Parameter* create(Loki::TypeInfo parameter_type); // Must give exact parameter type, such as TypedParameter<int>
    private:
        std::map<Loki::TypeInfo, std::map<int, create_f>> _registered_constructors;
        std::map<Loki::TypeInfo, create_f> _registered_constructors_exact;
        
    };
    template<class T1, class T2, int Type> class FactoryRegisterer
    {
    public:
        FactoryRegisterer()
        {
            ParameterFactory::instance()->RegisterConstructor(Loki::TypeInfo(typeid(T2)), 
                std::bind(&FactoryRegisterer<T1, T2, Type>::create), Type);

            ParameterFactory::instance()->RegisterConstructor(Loki::TypeInfo(typeid(T1)),
                std::bind(&FactoryRegisterer<T1, T2, Type>::create));
        }
        static Parameter* create()
        {
            return new T1();
        }
    };
}