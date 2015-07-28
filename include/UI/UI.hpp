#pragma once

#ifdef PARAMETERS_USE_UI

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

#else
namespace Parameters
{
	namespace UI
	{
		class NoUiPolicy
		{		};
		template<typename T> class UiPolicy: public NoUiPolicy
		{
		public:

		};
	}
}
#endif