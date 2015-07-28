#pragma once

#include <boost/asio/io_service.hpp>
#include "Parameter_def.hpp"

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
	}
}
