#pragma once
#include "IHandler.hpp"
#include "UIUpdateHandler.hpp"

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
                    
                    BOOST_LOG_TRIVIAL(debug) << "Creating handler for default unspecialized parameter " << typeid(T).name();
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
                    
                    BOOST_LOG_TRIVIAL(debug) << "Creating widget for default unspecialized parameter " << typeid(T).name();
                    return std::vector<QWidget*>();
                }
                static bool UiUpdateRequired()
                {
                    return false;
                }
                //static const bool UiUpdateRequired = false;
            };

            // **********************************************************************************
            // *************************** Bool ************************************************
            // **********************************************************************************
            template<> class Handler<bool, void> : public UiUpdateHandler
            {
                QCheckBox* chkBox;
                bool* boolData;
                bool _currently_updating;
            public:
                Handler() : chkBox(nullptr), boolData(nullptr), _currently_updating(false) {}
                virtual void UpdateUi( bool* data)
                {
                    if(data)
                    {
                        _currently_updating = true;
                        chkBox->setChecked(*data);
                        _currently_updating = false;
                    }
                    
                }
                virtual void OnUiUpdate(QObject* sender, int val)
                {
                    if(_currently_updating)
                        return;
                    if (sender == chkBox && IHandler::GetParamMtx())
                    {
                        std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
                        if (boolData)
                        {
                            *boolData = chkBox->isChecked();
                            if (onUpdate)
                                onUpdate();
                            if(_listener)
                                _listener->OnUpdate(this);
                        }                            
                    }
                }
                virtual void SetData(bool* data_)
                {    
                    if(IHandler::GetParamMtx())
                    {
                        std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
                        boolData = data_;
                        if (chkBox)
                            UpdateUi(data_);
                    }
                }
                bool* GetData()
                {
                    return boolData;
                }
                virtual std::vector < QWidget*> GetUiWidgets(QWidget* parent_)
                {
                    
                    std::vector<QWidget*> output;
                    if (chkBox == nullptr)
                        chkBox = new QCheckBox(parent_);
                    chkBox->connect(chkBox, SIGNAL(stateChanged(int)), IHandler::proxy, SLOT(on_update(int)));
                    output.push_back(chkBox);
                    return output;
                }
                static bool UiUpdateRequired()
                {
                    return false;
                }
                //static const bool UiUpdateRequired = true;
            };

            // **********************************************************************************
            // *************************** std::string ******************************************
            // **********************************************************************************

            template<> class Handler<std::string, void> : public UiUpdateHandler
            {
                std::string* strData;
                QLineEdit* lineEdit;
                bool _currently_updating = false;
            public:
                Handler() : strData(nullptr), lineEdit(nullptr) {}
                virtual void UpdateUi( std::string* data)
                {
                    if(data)
                    {
                        std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
                        _currently_updating = true;
                        lineEdit->setText(QString::fromStdString(*data));
                        _currently_updating = false;
                    }                    
                }
                virtual void OnUiUpdate(QObject* sender)
                {
                    if(_currently_updating || !IHandler::GetParamMtx())
                        return;
                    if (sender == lineEdit && strData)
                    {    
                        std::lock_guard<std::recursive_mutex>lock(*IHandler::GetParamMtx());
                        *strData = lineEdit->text().toStdString();
                        if (onUpdate)
                            onUpdate();
                        if(_listener)
                            _listener->OnUpdate(this);
                    }
                }
                virtual void SetData(std::string* data_)
                {
                    strData = data_;
                    if (lineEdit)
                    {
                        _currently_updating = true;
                        lineEdit->setText(QString::fromStdString(*strData));
                        _currently_updating = false;
                    }
                    
                }
                std::string* GetData()
                {
                    return strData;
                }
                virtual std::vector<QWidget*> GetUiWidgets(QWidget* parent)
                {
                    
                    std::vector<QWidget*> output;
                    if (lineEdit == nullptr)
                        lineEdit = new QLineEdit(parent);
                    //lineEdit->connect(lineEdit, SIGNAL(editingFinished()), proxy, SLOT(on_update()));
                    lineEdit->connect(lineEdit, SIGNAL(returnPressed()), proxy, SLOT(on_update()));
                    output.push_back(lineEdit);
                    return output;
                }
            };

            // **********************************************************************************
            // *************************** std::function<void(void)> **************************
            // **********************************************************************************

            template<> class Handler<std::function<void(void)>, void> : public UiUpdateHandler
            {
                std::function<void(void)>* funcData;
                QPushButton* btn;
            public:
                Handler() : funcData(nullptr), btn(nullptr) {}
                void UpdateUi(std::function<void(void)>* data)
                {
                    funcData = data;
                }
                virtual void OnUiUpdate(QObject* sender)
                {
                    if (sender == btn && IHandler::GetParamMtx())
                    {
                        std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
                        if (funcData)
                        {
                            (*funcData)();
                            if (onUpdate)
                            {
                                onUpdate();
                                if(_listener)
                                _listener->OnUpdate(this);
                            }
                        }
                    }
                }
                virtual void SetData(std::function<void(void)>* data_)
                {
                    funcData = data_;
                }
                std::function<void(void)>* GetData()
                {
                    return funcData;
                }
                virtual std::vector<QWidget*> GetUiWidgets(QWidget* parent)
                {
                    
                    std::vector<QWidget*> output;
                    if (btn == nullptr)
                    {
                        btn = new QPushButton(parent);
                    }

                    btn->connect(btn, SIGNAL(clicked()), proxy, SLOT(on_update()));
                    output.push_back(btn);
                    return output;
                }
            };

            // **********************************************************************************
            // *************************** floating point data **********************************
            // **********************************************************************************

            template<typename T>
            class Handler<T, typename std::enable_if<std::is_floating_point<T>::value, void>::type> : public UiUpdateHandler
            {
                T* floatData;
                QDoubleSpinBox* box;
                bool _currently_updating;
            public:
                typedef T min_max_type;
                Handler() : box(nullptr), floatData(nullptr), _currently_updating(false) {}
                virtual void UpdateUi( T* data)
                {
                    if(data)
                    {
                        _currently_updating = true;
                        box->setValue(*data);
                        _currently_updating = false;
                    }                    
                }
                virtual void OnUiUpdate(QObject* sender, double val = 0)
                {
                    if(_currently_updating || !IHandler::GetParamMtx())
                        return;
                    std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
                    if (sender == box && floatData)
                        *floatData = box->value();
                    if (onUpdate)
                        onUpdate();
                    if(_listener)
                        _listener->OnUpdate(this);
                }
                virtual void SetData(T* data_)
                {
                    std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
                    floatData = data_;
                    if (box)
                    {
                        _currently_updating = true;
                        box->setValue(*floatData);
                        _currently_updating = false;
                    }
                        
                }
                T* GetData()
                {
                    return floatData;
                }
                void SetMinMax(T min, T max)
                {
                    box->setMinimum(min);
                    box->setMaximum(max);
                }
                virtual std::vector<QWidget*> GetUiWidgets(QWidget* parent)
                {    
                    std::vector<QWidget*> output;
                    if (box == nullptr)
                    {
                        box = new QDoubleSpinBox(parent);
                        box->setMaximumWidth(100);
                        box->setMinimum(std::numeric_limits<T>::min());
                        box->setMaximum(std::numeric_limits<T>::max());
                    }

                    box->connect(box, SIGNAL(valueChanged(double)), proxy, SLOT(on_update(double)));
                    output.push_back(box);
                    return output;
                }
            };

             // **********************************************************************************
            // *************************** integers *********************************************
            // **********************************************************************************

            template<typename T>
            class Handler<T, typename std::enable_if<std::is_integral<T>::value, void>::type> : public UiUpdateHandler
            {
                T* intData;
                QSpinBox* box;
                bool _currently_updating;
            public:
                typedef T min_max_type;
                Handler() : box(nullptr), intData(nullptr), _currently_updating(false){}
                virtual void UpdateUi( T* data)
                {
                    if(data)
                    {
                        _currently_updating = true;
                        std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
                        box->setValue(*data);
                        _currently_updating = false;
                    }                    
                }
                virtual void OnUiUpdate(QObject* sender, int val = -1)
                {
                    if(_currently_updating || !IHandler::GetParamMtx())
                        return;
                    std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
                    if (sender == box && intData)
                    {
                        if(val == -1)
                        {
                            *intData = box->value();
                        }else
                        {
                            *intData = val;
                        }
                    }
                    if (onUpdate)
                        onUpdate();
                    if(_listener)
                        _listener->OnUpdate(this);
                }
                virtual void SetData(T* data_)
                {
                    intData = data_;
                    if (box)
                    {
                        _currently_updating = true;
                        box->setValue(*intData);
                        _currently_updating = false;
                    }                        
                }
                T* GetData()
                {
                    return intData;
                }
                void SetMinMax(T min_, T max_)
                {
                    box->setMinimum(min_);
                    box->setMaximum(max_);
                }
                virtual std::vector<QWidget*> GetUiWidgets(QWidget* parent)
                {
                    std::vector<QWidget*> output;
                    if (box == nullptr)
                    {
                        box = new QSpinBox(parent);
                        box->setMaximumWidth(100);
                        if (std::numeric_limits<T>::max() > std::numeric_limits<int>::max())
                            box->setMinimum(std::numeric_limits<int>::max());
                        else
                            box->setMinimum(std::numeric_limits<T>::max());

                        box->setMinimum(std::numeric_limits<T>::min());

                        if (intData)
                            box->setValue(*intData);
                        else
                            box->setValue(0);
                    }

                    box->connect(box, SIGNAL(valueChanged(int)), proxy, SLOT(on_update(int)));
                    box->connect(box, SIGNAL(editingFinished()), proxy, SLOT(on_update()));
                    output.push_back(box);
                    return output;
                }
            };
            // **********************************************************************************
            // *************************** Enums ************************************************
            // **********************************************************************************
            template<> class Handler<Parameters::EnumParameter, void> : public UiUpdateHandler
            {
                QComboBox* enumCombo;
                Parameters::EnumParameter* enumData;
                bool _updating;
            public:
                Handler() : enumCombo(nullptr), _updating(false){}
                ~Handler()
                {
                
                }
                virtual void UpdateUi( Parameters::EnumParameter* data)
                {
                    if(_updating)
                        return;
                    if(data)
                    {
                        std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
                        _updating = true;
                        enumCombo->clear();
                        for (int i = 0; i < data->enumerations.size(); ++i)
                        {
                            enumCombo->addItem(QString::fromStdString(data->enumerations[i]));
                        }
                        enumCombo->setCurrentIndex(data->currentSelection);
                        _updating = false;
                    }                    
                }
                virtual void OnUiUpdate(QObject* sender, int idx)
                {
                    if(_updating || !IHandler::GetParamMtx())
                        return;
                    std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
                    if (idx != -1 && sender == enumCombo && enumData)
                    {
                        if(enumData->currentSelection == idx)
                            return;
                        _updating = true;
                        enumData->currentSelection = idx;
                        if (onUpdate)
                            onUpdate();
                        if(_listener)
                            _listener->OnUpdate(this);
                        _updating = false;
                    }
                }
                virtual void SetData(Parameters::EnumParameter* data_)
                {
                    enumData = data_;
                    if (enumCombo)
                        UpdateUi(enumData);
                }
                Parameters::EnumParameter*  GetData()
                {
                    return enumData;
                }
                virtual std::vector<QWidget*> GetUiWidgets(QWidget* parent)
                {
                    
                    std::vector<QWidget*> output;
                    if (enumCombo == nullptr)
                        enumCombo = new QComboBox(parent);
                    enumCombo->connect(enumCombo, SIGNAL(currentIndexChanged(int)), proxy, SLOT(on_update(int)));
                    output.push_back(enumCombo);
                    return output;
                }
            };

            // **********************************************************************************
            // *************************** Files ************************************************
            // **********************************************************************************

            template<typename T> class Handler<T, typename std::enable_if<
                std::is_same<T, Parameters::WriteDirectory>::value || 
                std::is_same<T, Parameters::ReadDirectory>::value || 
                std::is_same<T, Parameters::WriteFile>::value || 
                std::is_same<T, Parameters::ReadFile>::value, void>::type> : public UiUpdateHandler
            {
                QPushButton* btn;
                QWidget* parent;
                T* fileData;
                bool _currently_updating;
            public:
                Handler(): btn(nullptr), parent(nullptr), _currently_updating(false){}
                virtual void UpdateUi( T* data)
                {
                    if(data && IHandler::GetParamMtx())
                    {
                        std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
                        _currently_updating = true;
                        btn->setText(QString::fromStdString(data->string()));
                        _currently_updating = false;
                    }                    
                }
                virtual void OnUiUpdate(QObject* sender)
                {
                    if(_currently_updating)
                        return;
                    if (sender == btn && IHandler::GetParamMtx())
                    {
                        std::lock_guard<std::recursive_mutex>lock(*IHandler::GetParamMtx());
                        QString filename;
                        if (std::is_same<T, Parameters::WriteDirectory>::value || std::is_same<T, Parameters::ReadDirectory>::value)
                        {
                            filename = QFileDialog::getExistingDirectory(parent, "Select save directory");
                        }
                        if (std::is_same<T, Parameters::WriteFile>::value)
                        {
                            filename = QFileDialog::getSaveFileName(parent, "Select file to save");
                        }
                        if (std::is_same<T, Parameters::ReadFile>::value)
                        {
                            filename = QFileDialog::getOpenFileName(parent, "Select file to open");
                        }
                        btn->setText(filename);
                        *fileData = T(filename.toStdString());
                        if(onUpdate)
                            onUpdate();
                        if(_listener)
                                _listener->OnUpdate(this);
                    }                    
                }
                virtual void SetData(T* data_)
                {
                    fileData = data_;
                    if (btn)
                        UpdateUi(data_);
                }
                T* GetData()
                {
                    return fileData;
                }
                virtual std::vector<QWidget*> GetUiWidgets(QWidget* parent_)
                {
                    
                    std::vector< QWidget* > output;
                    parent = parent_;
                    if (btn == nullptr)
                        btn = new QPushButton(parent);
                    btn->connect(btn, SIGNAL(clicked()), proxy, SLOT(on_update()));
                    output.push_back(btn);
                    return output;
                }
            };

        }
    }
}