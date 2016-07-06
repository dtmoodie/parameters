/*
Copyright (c) 2015 Daniel Moodie.
All rights reserved.

Redistribution and use in source and binary forms are permitted
provided that the above copyright notice and this paragraph are
duplicated in all such forms and that any documentation,
advertising materials, and other materials related to such
distribution and use acknowledge that the software was developed
by the Daniel Moodie. The name of
Daniel Moodie may not be used to endorse or promote products derived
from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

https://github.com/dtmoodie/parameters
*/
#ifdef Qt5_FOUND
#include "parameters/UI/Qt.hpp"
#include "parameters/Parameter.hpp"
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
std::map<Loki::TypeInfo, WidgetFactory::HandlerCreator>& WidgetFactory::registry()
{
    static auto instance = std::map<Loki::TypeInfo, WidgetFactory::HandlerCreator>();
    return instance;
}

void WidgetFactory::RegisterCreator(Loki::TypeInfo type, HandlerCreator f)
{
    LOG_TRIVIAL(trace) << "Registering type " << type.name();
    WidgetFactory::registry()[type] = f;
}

std::shared_ptr<IParameterProxy> WidgetFactory::Createhandler(Parameters::Parameter* param)
{
    std::string typeName = param->GetTypeInfo().name();
    std::string treeName = param->GetTreeName();
    auto itr = registry().find(param->GetTypeInfo());
    if (itr == registry().end())
    {
        
        LOG_TRIVIAL(debug) << "No Widget Factory registered for type " << typeName << " unable to make widget for parameter: " << treeName;
        return std::shared_ptr<IParameterProxy>(new DefaultProxy(param));
    }
    LOG_TRIVIAL(trace) << "Creating handler for " << typeName << " " << treeName;
    return itr->second(param);
}

DefaultProxy::DefaultProxy(Parameters::Parameter* param)
{
    parameter = param;
    delete_connection = param->RegisterDeleteNotifier(std::bind(&DefaultProxy::onParamDelete, this));
}
bool DefaultProxy::SetParameter(Parameters::Parameter* param)
{
    parameter = param;
    return true;
}
bool DefaultProxy::CheckParameter(Parameters::Parameter* param)
{    
    return param == parameter;
}
QWidget* DefaultProxy::GetParameterWidget(QWidget* parent)
{
    
    QWidget* output = new QWidget(parent);

    QGridLayout* layout = new QGridLayout(output);
    QLabel* nameLbl = new QLabel(QString::fromStdString(parameter->GetName()), output);
    nameLbl->setToolTip(QString::fromStdString(parameter->GetTypeInfo().name()));
    layout->addWidget(nameLbl, 0, 0);
    output->setLayout(layout);
    return output;
}
IHandler::IHandler() : paramMtx(nullptr), proxy(new SignalProxy(this)), _listener(nullptr) {}
IHandler::~IHandler()
{
    if(proxy)
        delete proxy;
}
void IHandler::OnUiUpdate(QObject* sender) {}
void IHandler::OnUiUpdate(QObject* sender, double val) {}
void IHandler::OnUiUpdate(QObject* sender, int val) {}
void IHandler::OnUiUpdate(QObject* sender, bool val) {}
void IHandler::OnUiUpdate(QObject* sender, QString val) {}
void IHandler::OnUiUpdate(QObject* sender, int row, int col) {}
void IHandler::SetUpdateListener(UiUpdateListener* listener)
{
    _listener = listener;
}

std::function<void(void)>& IHandler::GetOnUpdate()
{

    return onUpdate;
}
std::vector<QWidget*> IHandler::GetUiWidgets(QWidget* parent)
{

    return std::vector<QWidget*>();
}
void IHandler::SetParamMtx(std::recursive_mutex* mtx)
{
    paramMtx = mtx;
}
std::recursive_mutex* IHandler::GetParamMtx()
{
    return paramMtx;
}

bool IHandler::UiUpdateRequired()
{
    return false;
}
#endif // Qt5_FOUND