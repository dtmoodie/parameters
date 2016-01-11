#include "UI/UI.hpp"
#include "UI/InterThread.hpp"
using namespace Parameters::UI;

qt::IParameterProxy::IParameterProxy()
{

}
qt::IParameterProxy::~IParameterProxy()
{

}

void InvalidCallbacks::invalidate(void* sender)
{
    boost::mutex::scoped_lock lock(mtx);
    invalid_senders.push_back(sender);
}
bool InvalidCallbacks::check_valid(void* sender)
{
    boost::mutex::scoped_lock lock(mtx);
    if (std::find(invalid_senders.begin(), invalid_senders.end(), sender) == invalid_senders.end())
        return true;
    return false;
}
void InvalidCallbacks::clear()
{
    boost::mutex::scoped_lock lock(mtx);
    invalid_senders.clear();
}
        
std::list<void*> InvalidCallbacks::invalid_senders;
boost::mutex InvalidCallbacks::mtx;

UiCallbackService * UiCallbackService::Instance()
{
	static UiCallbackService instance;
	return &instance;   
}
void UiCallbackService::post(boost::function<void ()> f, std::pair<void*, Loki::TypeInfo> source)
{
    if(user_thread_callback_service)
    {
        user_thread_callback_service(f, source);
        return;
    }
	if (user_thread_callback_notifier)
		user_thread_callback_notifier();
    io_queue.push(std::make_pair(source, f));
}

void UiCallbackService::setCallback(boost::function<void (boost::function<void ()>, std::pair<void*, Loki::TypeInfo>)> f)
{
    Instance()->user_thread_callback_service = f;
}
void UiCallbackService::setCallback(boost::function<void(void)>& f)
{
	Instance()->user_thread_callback_notifier = f;
}

void UiCallbackService::run()
{
    std::pair<std::pair<void*, Loki::TypeInfo>, boost::function<void(void)>> data;
    auto inst = Instance();
    while (inst->io_queue.try_pop(data))
    {
        if (InvalidCallbacks::check_valid(data.first.first))
        {
            data.second();
        }
    }
}


ProcessingThreadCallbackService* ProcessingThreadCallbackService::Instance()
{
	static ProcessingThreadCallbackService instance;
	return &instance;
}

void ProcessingThreadCallbackService::setCallback(boost::function<void(boost::function<void(void)>, std::pair<void*, Loki::TypeInfo>)> f)
{
	Instance()->user_processing_thread_callback_function = f;
}


void ProcessingThreadCallbackService::run()
{
    std::pair<std::pair<void*, Loki::TypeInfo>, boost::function<void(void)>> data;
    auto inst = Instance();
    while (inst->io_queue.try_pop(data))
    {
        if (InvalidCallbacks::check_valid(data.first.first))
        {
            data.second();
        }
    }
}

void ProcessingThreadCallbackService::post(boost::function<void(void)> f, std::pair<void*, Loki::TypeInfo> source)
{
	auto instance = Instance();
	if (instance->user_processing_thread_callback_function)
	{
		instance->user_processing_thread_callback_function(f, source);
		return;
	}
    instance->io_queue.push(std::make_pair(source, f));
}