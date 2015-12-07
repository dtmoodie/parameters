#pragma once
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