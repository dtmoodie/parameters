#pragma once
#include "IParameterProxy.hpp"
#include <signals/connection.h>
namespace Parameters
{
    namespace UI
    {
        namespace qt
        {
            // *****************************************************************************
            //                                DefaultProxy
            // *****************************************************************************
            class PARAMETER_EXPORTS DefaultProxy: public IParameterProxy
            {
                Parameters::Parameter* parameter;
                std::shared_ptr<Signals::connection> delete_connection;
                void onUiUpdate()
                {            }
                void onParamUpdate()
                {                }
                void onParamDelete()
                {
                    parameter = nullptr;
                }
            public:
                DefaultProxy(Parameters::Parameter* param);
                virtual bool CheckParameter(Parameter* param);
                bool SetParameter(Parameter* param);
                QWidget* GetParameterWidget(QWidget* parent);
            };
        }
    }
}