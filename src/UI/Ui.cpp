#include "UI/UI.hpp"
#include "UI/InterThread.hpp"
using namespace Parameters::UI;

boost::asio::io_service UiCallbackService::service;
boost::asio::io_service ProcessingThreadCallbackService::service;

boost::asio::io_service& ProcessingThreadCallbackService::Instance()
{
	return service;
}
boost::asio::io_service& UiCallbackService::Instance()
{
	return service;
}
