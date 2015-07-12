#include "UI/Qt.hpp"
#include "Parameters.hpp"

using namespace Parameters::UI::qt;

SignalProxy::SignalProxy(IHandler* handler_)
{
	handler = handler_;
	lastCallTime.start();
}

void SignalProxy::on_update()
{
	if (lastCallTime.elapsed() > 15)
	{
		lastCallTime.start();
		handler->OnUiUpdate(sender());
	}
	
}
void SignalProxy::on_update(int)
{
	if (lastCallTime.elapsed() > 15)
	{
		lastCallTime.start();
		handler->OnUiUpdate(sender());
	}
}
void SignalProxy::on_update(double)
{
	if (lastCallTime.elapsed() > 15)
	{
		lastCallTime.start();
		handler->OnUiUpdate(sender());
	}
}
void SignalProxy::on_update(bool)
{
	if (lastCallTime.elapsed() > 15)
	{
		lastCallTime.start();
		handler->OnUiUpdate(sender());
	}
}
void SignalProxy::on_update(QString)
{
	if (lastCallTime.elapsed() > 15)
	{
		lastCallTime.start();
		handler->OnUiUpdate(sender());
	}
}
std::map<Loki::TypeInfo, WidgetFactory::HandlerCreator> WidgetFactory::registry = std::map<Loki::TypeInfo, WidgetFactory::HandlerCreator>();

void WidgetFactory::RegisterCreator(Loki::TypeInfo& type, HandlerCreator f)
{
	WidgetFactory::registry[type] = f;
}

std::shared_ptr<IParameterProxy> WidgetFactory::Createhandler(std::shared_ptr<Parameters::Parameter> param)
{
	auto itr = registry.find(param->GetTypeInfo());
	if (itr == registry.end())
	{
		std::cout << "No Widget Factory registered for type " << param->GetTypeInfo().name() << " unable to make widget for parameter: " << param->GetTreeName() << std::endl;
		return std::shared_ptr<IParameterProxy>();
	}
		
	return itr->second(param);
}

