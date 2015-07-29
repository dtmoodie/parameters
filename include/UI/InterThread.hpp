#pragma once

#include <boost/asio/io_service.hpp>
#include "Parameter_def.hpp"
#include <boost/function.hpp>
namespace Parameters
{
	namespace UI
	{
		class Parameter_EXPORTS UiCallbackService
		{
            boost::function<void(boost::function<void(void)>)> user_thread_callback_service;
            static UiCallbackService* g_instance;

			static boost::asio::io_service service;
		public:
            static UiCallbackService* Instance();

            void post(boost::function<void(void)> f);

            static void setCallback(boost::function<void(boost::function<void(void)>)> f);
            // To be called from the UI thread
            static void run();
		};
		class Parameter_EXPORTS ProcessingThreadCallbackService
		{
			static boost::asio::io_service service;
            static ProcessingThreadCallbackService* g_instance;
		public:
            static ProcessingThreadCallbackService* Instance();
		};
	}
}
