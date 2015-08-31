#include "UI/UI.hpp"
#include "UI/InterThread.hpp"
using namespace Parameters::UI;

//boost::asio::io_service UiCallbackService::service;
//UiCallbackService* UiCallbackService::g_instance = nullptr;

UiCallbackService * UiCallbackService::Instance()
{
    //if(g_instance == nullptr)
    //    g_instance = new UiCallbackService();
	static UiCallbackService instance;
	return &instance;
    //return g_instance;
}
void UiCallbackService::post(boost::function<void ()> f)
{
    if(user_thread_callback_service)
    {
        user_thread_callback_service(f);
        return;
    }
    service.post(f);
}

void UiCallbackService::setCallback(boost::function<void (boost::function<void ()>)> f)
{
    Instance()->user_thread_callback_service = f;
}

void UiCallbackService::run()
{
    Instance()->service.run();
}

//boost::asio::io_service ProcessingThreadCallbackService::service;
//ProcessingThreadCallbackService* ProcessingThreadCallbackService::g_instance = nullptr;

ProcessingThreadCallbackService* ProcessingThreadCallbackService::Instance()
{
	static ProcessingThreadCallbackService instance;
	return &instance;
}

void ProcessingThreadCallbackService::setCallback(boost::function<void(boost::function<void(void)>)> f)
{
	Instance()->user_processing_thread_callback_function = f;
}

void ProcessingThreadCallbackService::run()
{
	Instance()->service.run();
}

void ProcessingThreadCallbackService::post(boost::function<void(void)> f)
{
	auto instance = Instance();
	if (instance->user_processing_thread_callback_function)
	{
		instance->user_processing_thread_callback_function(f);
		return;
	}
	instance->service.post(f);
}