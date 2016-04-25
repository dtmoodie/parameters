#ifndef TYPE_HPP
#define TYPE_HPP

#include "parameters/Parameter_def.hpp"

#include <string>
#include <typeinfo>
namespace TypeInfo
{
	std::string PARAMETER_EXPORTS demangle(const char* name);

template <class T>
std::string type(const T& t) {

    return demangle(typeid(t).name());
}
}
//#endif
#endif
