#pragma once

#include "Parameter_def.hpp"

class QWidget;


namespace Parameters
{
    class Parameter;
    namespace UI
    {
        namespace qt
        {
            // *****************************************************************************
            //                                IParameterProxy
            // *****************************************************************************
            class PARAMETER_EXPORTS IParameterProxy
            {
            protected:
            public:
                typedef std::shared_ptr<IParameterProxy> Ptr;
                IParameterProxy();
                virtual ~IParameterProxy();
                
                virtual QWidget* GetParameterWidget(QWidget* parent) = 0;
                virtual bool CheckParameter(Parameter* param) = 0;
                virtual bool SetParameter(Parameter* param) = 0;
            };
        }
    }
}