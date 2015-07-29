#include "UI/UI.hpp"
#include "UI/InterThread.hpp"
using namespace Parameters::UI;

boost::asio::io_service UiCallbackService::service;
boost::asio::io_service ProcessingThreadCallbackService::service;

UiCallbackService* UiCallbackService::g_instance = nullptr;
ProcessingThreadCallbackService* ProcessingThreadCallbackService::g_instance = nullptr;

ProcessingThreadCallbackService *ProcessingThreadCallbackService::Instance()
{
    if(g_instance == nullptr)
        g_instance = new ProcessingThreadCallbackService();
    return g_instance;
}
UiCallbackService * UiCallbackService::Instance()
{
    if(g_instance == nullptr)
        g_instance = new UiCallbackService();
    return g_instance;
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
    service.run();
}
