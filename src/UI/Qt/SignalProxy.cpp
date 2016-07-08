#include "parameters/UI/Qt/SignalProxy.hpp"
#include "parameters/UI/Qt/IHandler.hpp"

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