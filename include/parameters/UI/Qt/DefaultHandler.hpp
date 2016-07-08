#pragma once
#include "IHandler.hpp"
#include <signals/logging.hpp>
class QWidget;

namespace Parameters
{
    namespace UI
    {
        namespace qt
        {
            // *****************************************************************************
            //                                Default Handler
            // *****************************************************************************
            template<typename T, typename Enable = void> class Handler : public IHandler
            {
                T* currentData;
                
            public:
                Handler() :currentData(nullptr)
                {
                    LOG(debug) << "Creating handler for default unspecialized parameter " << typeid(T).name();
                }
                virtual void UpdateUi( T* data)
                {}
                virtual void OnUiUpdate(QObject* sender)
                {}
                virtual void SetData(T* data)
                {
                    currentData = data;
                }
                T* GetData()
                {
                    return currentData;
                }
                virtual std::vector<QWidget*> GetUiWidgets(QWidget* parent)
                {
                    
                    LOG(debug) << "Creating widget for default unspecialized parameter " << typeid(T).name();
                    return std::vector<QWidget*>();
                }
                static bool UiUpdateRequired()
                {
                    return false;
                }
                static const bool IS_DEFAULT = true;
            };
        }
    }
}