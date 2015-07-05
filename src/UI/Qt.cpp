#include "UI/Qt.hpp"
#include "Parameters.hpp"

using namespace Parameters::UI::qt;

SignalProxy::SignalProxy(IHandler* handler_)
{
	handler = handler_;
}

void SignalProxy::on_update()
{
	handler->OnUiUpdate(sender());
}
void SignalProxy::on_update(int)
{
	handler->OnUiUpdate(sender());
}
void SignalProxy::on_update(double)
{
	handler->OnUiUpdate(sender());
}
void SignalProxy::on_update(bool)
{
	handler->OnUiUpdate(sender());
}
void SignalProxy::on_update(QString)
{
	handler->OnUiUpdate(sender());
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
		return std::shared_ptr<IParameterProxy>();
	return itr->second(param);
}