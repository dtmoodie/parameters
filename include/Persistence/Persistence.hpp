#pragma once
#ifdef PARAMTERS_GENERATE_PERSISTENCE

#include "OpenCV.hpp"
#include "TextSerializer.hpp"

namespace Parameters
{
	namespace Persistence
	{
		template<typename T> class PersistencePolicy : 
			public Parameters::Persistence::cv::PersistencePolicy<T>, 
			public Parameters::Persistence::Text::PersistencePolicy<T>
		{	
		};
	}
}
#else
namespace Parameters
{
	namespace Persistence
	{
		template<typename T> class PersistencePolicy
		{
		};
	}
}
#endif