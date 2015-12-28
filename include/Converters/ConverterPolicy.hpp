#pragma once

#ifdef PARAMTERS_GENERATE_CONVERTERS
#include "DoubleConverter.hpp"
namespace Parameters
{
	namespace Converters
	{
		template<typename T> class ConverterPolicy: public Double::ConverterPolicy<T>
		{

		};
	}
}
#else
namespace Parameters
{
	namespace Converters
	{
		template<typename T> class ConverterPolicy
		{

		};
	}
}
#endif