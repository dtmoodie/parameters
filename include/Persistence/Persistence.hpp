#pragma once


#include "OpenCV.hpp"
#include "TextSerializer.hpp"

namespace Parameters
{
	namespace Persistence
	{
		template<typename T> class PersistencePolicy : public cv::PersistencePolicy<T>, public Text::PersistencePolicy<T>
		{
		public:
			
		};
	}
}

