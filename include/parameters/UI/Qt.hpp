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
#pragma once
#ifdef Qt5_FOUND
#include "../Parameter_def.hpp"
#include <QtWidgets/QWidget>
#include <type_traits>

#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/qwidget.h>
#include "QtWidgets/qlayout.h"
#include "QtWidgets/qlabel.h"
#include "QtWidgets/qcombobox.h"
#include "QtWidgets/qcheckbox.h"
#include "QtCore/qdatetime.h"
#include "QtWidgets/qpushbutton.h"
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qfiledialog.h>
#include "QtWidgets/qtablewidget.h"
#include "QtWidgets/qheaderview.h"
#include <functional>
#include <parameters/LokiTypeInfo.h>
#include <mutex>
#include "../Types.hpp"
#include "InterThread.hpp"
#if defined(HAVE_OPENCV) || defined(CV_EXPORTS) || defined(CVAPI_EXPORTS)
#include <opencv2/core/types.hpp>
#endif
#include <memory>
#include <signals/connection.h>
#include <signals/thread_registry.h>
#include <boost/log/trivial.hpp>
#include <boost/log/attributes/named_scope.hpp>
namespace cv
{
    namespace cuda
    {
        class Stream;
    }
}
namespace Parameters
{
    class Parameter;
    namespace UI
    {
        namespace qt
        {
            class IHandler;
            class IParameterProxy;
            // *****************************************************************************
            //                                WidgetFactory
            // *****************************************************************************
            class PARAMETER_EXPORTS WidgetFactory
            {
            public:
                typedef std::function<std::shared_ptr<IParameterProxy>(Parameter*)> HandlerCreator;
                void RegisterCreator(Loki::TypeInfo type, HandlerCreator f);
                std::shared_ptr<IParameterProxy> Createhandler(Parameters::Parameter* param);
                
                static WidgetFactory* Instance();
            private:
                WidgetFactory();
                std::map<Loki::TypeInfo, HandlerCreator> registry;
            };
        } /* namespace qt */
    } /* namespace UI */
} /* namespace Perameters */

#endif // Qt5_FOUND
