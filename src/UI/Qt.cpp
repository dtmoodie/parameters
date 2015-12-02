#include "UI/Qt.hpp"
#include "Parameters.hpp"
#include <boost/log/trivial.hpp>
#include <boost/log/attributes/named_scope.hpp>

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
		LOG_TRACE;
		lastCallTime.start();
		handler->OnUiUpdate(sender());
	}
	
}
void SignalProxy::on_update(int val)
{
	if (lastCallTime.elapsed() > 15)
	{
		LOG_TRACE;
		lastCallTime.start();
		handler->OnUiUpdate(sender(), val);
	}
}
void SignalProxy::on_update(double val)
{
	if (lastCallTime.elapsed() > 15)
	{
		LOG_TRACE;
		lastCallTime.start();
		handler->OnUiUpdate(sender(), val);
	}
}
void SignalProxy::on_update(bool val)
{
	if (lastCallTime.elapsed() > 15)
	{
		LOG_TRACE;
		lastCallTime.start();
		handler->OnUiUpdate(sender(), val);
	}
}
void SignalProxy::on_update(QString val)
{
	if (lastCallTime.elapsed() > 15)
	{
		LOG_TRACE;
		lastCallTime.start();
		handler->OnUiUpdate(sender(), val);
	}
}
void SignalProxy::on_update(int row, int col)
{
	if (lastCallTime.elapsed() > 15)
	{
		LOG_TRACE;
		lastCallTime.start();
		handler->OnUiUpdate(sender(), row, col);
	}
}
std::map<Loki::TypeInfo, WidgetFactory::HandlerCreator>& WidgetFactory::registry()
{
	static auto instance = std::map<Loki::TypeInfo, WidgetFactory::HandlerCreator>();
	return instance;
}

void WidgetFactory::RegisterCreator(Loki::TypeInfo type, HandlerCreator f)
{
	LOG_TRIVIAL(info) << "Registering type " << type.name();
	WidgetFactory::registry()[type] = f;
}

std::shared_ptr<IParameterProxy> WidgetFactory::Createhandler(std::shared_ptr<Parameters::Parameter> param)
{
	std::string typeName = param->GetTypeInfo().name();
	std::string treeName = param->GetTreeName();
	auto itr = registry().find(param->GetTypeInfo());
    if (itr == registry().end())
	{
        
		LOG_TRIVIAL(warning) << "No Widget Factory registered for type " << typeName << " unable to make widget for parameter: " << treeName;
		return std::shared_ptr<IParameterProxy>(new DefaultProxy(param));
	}
	LOG_TRIVIAL(info) << "Creating handler for " << typeName << " " << treeName;
	return itr->second(param);
}

DefaultProxy::DefaultProxy(std::shared_ptr<Parameters::Parameter> param)
{
	LOG_TRACE;
	parameter = param;
}

bool DefaultProxy::CheckParameter(Parameters::Parameter* param)
{
	LOG_TRACE;
	return param == parameter.get();
}
QWidget* DefaultProxy::GetParameterWidget(QWidget* parent)
{
	LOG_TRACE;
	QWidget* output = new QWidget(parent);

	QGridLayout* layout = new QGridLayout(output);
	QLabel* nameLbl = new QLabel(QString::fromStdString(parameter->GetName()), output);
	nameLbl->setToolTip(QString::fromStdString(parameter->GetTypeInfo().name()));
    layout->addWidget(nameLbl, 0, 0);
	output->setLayout(layout);
	return output;
}
IHandler::IHandler() : paramMtx(nullptr), proxy(new SignalProxy(this)) {}
void IHandler::OnUiUpdate(QObject* sender) {}
void IHandler::OnUiUpdate(QObject* sender, double val) {}
void IHandler::OnUiUpdate(QObject* sender, int val) {}
void IHandler::OnUiUpdate(QObject* sender, bool val) {}
void IHandler::OnUiUpdate(QObject* sender, QString val) {}
void IHandler::OnUiUpdate(QObject* sender, int row, int col) {}

std::function<void(void)>& IHandler::GetUpdateSignal()
{

    return onUpdate;
}
std::vector<QWidget*> IHandler::GetUiWidgets(QWidget* parent)
{

    return std::vector<QWidget*>();
}
void IHandler::SetParamMtx(boost::recursive_mutex* mtx)
{
    paramMtx = mtx;
}
boost::recursive_mutex* IHandler::GetParamMtx()
{
    return paramMtx;
}

bool IHandler::UiUpdateRequired()
{
    return false;
}