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
void SignalProxy::on_update(int val)
{
	if (lastCallTime.elapsed() > 15)
	{
		lastCallTime.start();
		handler->OnUiUpdate(sender(), val);
	}
}
void SignalProxy::on_update(double val)
{
	if (lastCallTime.elapsed() > 15)
	{
		lastCallTime.start();
		handler->OnUiUpdate(sender(), val);
	}
}
void SignalProxy::on_update(bool val)
{
	if (lastCallTime.elapsed() > 15)
	{
		lastCallTime.start();
		handler->OnUiUpdate(sender(), val);
	}
}
void SignalProxy::on_update(QString val)
{
	if (lastCallTime.elapsed() > 15)
	{
		lastCallTime.start();
		handler->OnUiUpdate(sender(), val);
	}
}
void SignalProxy::on_update(int row, int col)
{
	if (lastCallTime.elapsed() > 15)
	{
		lastCallTime.start();
		handler->OnUiUpdate(sender(), row, col);
	}
}
std::map<Loki::TypeInfo, WidgetFactory::HandlerCreator> WidgetFactory::registry = std::map<Loki::TypeInfo, WidgetFactory::HandlerCreator>();

void WidgetFactory::RegisterCreator(Loki::TypeInfo type, HandlerCreator f)
{
    std::cout << "[Widget Factory] Registering type " << type.name() << std::endl;
	WidgetFactory::registry[type] = f;
}

std::shared_ptr<IParameterProxy> WidgetFactory::Createhandler(std::shared_ptr<Parameters::Parameter> param)
{
	auto itr = registry.find(param->GetTypeInfo());
    if (itr == registry.end())
	{
        std::string typeName = param->GetTypeInfo().name();
        std::string treeName = param->GetTreeName();
        std::cout << "No Widget Factory registered for type " << typeName << " unable to make widget for parameter: " << treeName << std::endl;
		return std::shared_ptr<IParameterProxy>(new DefaultProxy(param));
	}
		
	return itr->second(param);
}

DefaultProxy::DefaultProxy(std::shared_ptr<Parameters::Parameter> param)
{
	parameter = param;
}

bool DefaultProxy::CheckParameter(Parameters::Parameter* param)
{
	return param == parameter.get();
}
QWidget* DefaultProxy::GetParameterWidget(QWidget* parent)
{
	QWidget* output = new QWidget(parent);

	QGridLayout* layout = new QGridLayout(output);
	QLabel* nameLbl = new QLabel(QString::fromStdString(parameter->GetName()), output);
	nameLbl->setToolTip(QString::fromStdString(parameter->GetTypeInfo().name()));
	layout->addWidget(nameLbl, 0, 0);
	int count = 1;
	output->setLayout(layout);
	return output;
}