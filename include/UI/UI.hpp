#pragma once

//#ifdef PARAMTERS_USE_UI && !def(PARAMETERS_NO_UI)

#include "Qt.hpp"
#include <boost/asio/io_service.hpp>
namespace Parameters
{
	namespace UI
	{
		class Parameter_EXPORTS UiCallbackService
		{
			static boost::asio::io_service service;
		public:
			static boost::asio::io_service& Instance();
		};
		class Parameter_EXPORTS ProcessingThreadCallbackService
		{
			static boost::asio::io_service service;
		public:
			static boost::asio::io_service& Instance();
		};



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