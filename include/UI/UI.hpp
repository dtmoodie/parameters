#pragma once

//#ifdef PARAMTERS_USE_UI && !def(PARAMETERS_NO_UI)

#include "Qt.hpp"
namespace Parameters
{
	namespace UI
	{
		template<typename T> class UiPolicy: public qt::QtUiPolicy<T>
		{
		public:

		};
	}
}

/*#else
namespace Parameters
{
	namespace UI
	{
		template<typename T> class UiPolicy
		{
		public:

		};
	}
}
#endif*/