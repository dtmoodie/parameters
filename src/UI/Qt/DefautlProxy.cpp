#include "parameters/UI/Qt/DefaultProxy.hpp"
#include "parameters/Parameter.hpp"
#include <qgridlayout.h>
#include <qlabel.h>
#include <qstring.h>
using namespace Parameters::UI::qt;

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