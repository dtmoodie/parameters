#pragma once
#include "parameters/Parameter_def.hpp"
#include <qobject.h>
#include <qdatetime.h>

namespace Parameters
{
    namespace UI
    {
        namespace qt
        {
            class IHandler;
            // *****************************************************************************
            //                                SignalProxy
            // *****************************************************************************
            class PARAMETER_EXPORTS SignalProxy : public QObject
            {
                Q_OBJECT
                IHandler* handler;
                QTime lastCallTime;
            public:
                SignalProxy(IHandler* handler_);

            public slots:
                void on_update();
                void on_update(int);
                void on_update(double);
                void on_update(bool);
                void on_update(QString);
                void on_update(int row, int col);
            };
        }
    }
}