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
	template<typename T> class ITypedParameter;
	template<typename T> class ITypedRangedParameter;
	namespace UI
	{
		namespace qt
		{
			class IHandler;
			class IParameterProxy;
			// *****************************************************************************
			//								WidgetFactory
			// *****************************************************************************
			class PARAMETER_EXPORTS WidgetFactory
			{
			public:
				typedef std::function<std::shared_ptr<IParameterProxy>(Parameter*)> HandlerCreator;
                static void RegisterCreator(Loki::TypeInfo type, HandlerCreator f);
				static std::shared_ptr<IParameterProxy> Createhandler(Parameters::Parameter* param);

			private:
				static std::map<Loki::TypeInfo, HandlerCreator>& registry();
			};			
			// *****************************************************************************
			//								IParameterProxy
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
			// *****************************************************************************
			//								SignalProxy
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
			// *****************************************************************************
			//								DefaultProxy
			// *****************************************************************************
			class PARAMETER_EXPORTS DefaultProxy: public IParameterProxy
			{
				Parameters::Parameter* parameter;
				std::shared_ptr<Signals::connection> delete_connection;
				void onUiUpdate()
				{			}
				void onParamUpdate()
				{				}
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
            struct PARAMETER_EXPORTS UiUpdateListener
            {
                virtual void OnUpdate(IHandler* handler) = 0;
            };

			// *****************************************************************************
			//								IHandler
			// *****************************************************************************
			class PARAMETER_EXPORTS IHandler
			{
            private:
                std::recursive_mutex* paramMtx;
			protected:
                UiUpdateListener* _listener;
				SignalProxy* proxy;
				std::function<void(void)> onUpdate;
			public:
                IHandler();
                virtual void OnUiUpdate(QObject* sender);
                virtual void OnUiUpdate(QObject* sender, double val);
                virtual void OnUiUpdate(QObject* sender, int val);
                virtual void OnUiUpdate(QObject* sender, bool val);
                virtual void OnUiUpdate(QObject* sender, QString val);
                virtual void OnUiUpdate(QObject* sender, int row, int col);
                virtual void SetUpdateListener(UiUpdateListener* listener);
				
                virtual std::function<void(void)>& GetOnUpdate();
                virtual std::vector<QWidget*> GetUiWidgets(QWidget* parent);
                virtual void SetParamMtx(std::recursive_mutex* mtx);
                virtual std::recursive_mutex* GetParamMtx();
                static bool UiUpdateRequired();
			};

			class UiUpdateHandler: public IHandler
			{
			public:
				//static const bool UiUpdateRequired = true;
                static bool UiUpdateRequired() 
                {
                    return true;
                }
			};
			// *****************************************************************************
			//								Default Handler
			// *****************************************************************************
			template<typename T, typename Enable = void> class Handler : public IHandler
			{
				T* currentData;
                
			public:
				Handler() :currentData(nullptr)
				{
					
					BOOST_LOG_TRIVIAL(debug) << "Creating handler for default unspecialized parameter";
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
					
                    BOOST_LOG_TRIVIAL(debug) << "Creating widget for default unspecialized parameter";
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
					if (sender == chkBox)
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
					std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
					boolData = data_;
					if (chkBox)
						UpdateUi(data_);
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
                    if(data)
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
					if (sender == btn)
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

#if defined(OPENCV_FOUND) || defined(CV_EXPORTS) || defined(CVAPI_EXPORTS)
			// **********************************************************************************
			// *************************** cv::Matx *********************************************
			// **********************************************************************************
			template<typename T, int ROW, int COL> class Handler<typename ::cv::Matx<T, ROW, COL>, void> : public UiUpdateHandler
			{
				QTableWidget* table;
				std::vector<QTableWidgetItem*> items;
				::cv::Matx<T, ROW, COL>* matData;
				bool _currently_updating;
			public:
				Handler() : table(nullptr), matData(nullptr), _currently_updating(false), UiUpdateHandler()
				{
					
					table = new QTableWidget();
					table->horizontalHeader()->hide();
					table->verticalHeader()->hide();
					items.reserve(ROW*COL);
                    if(COL == 1)
                    {
                        table->setColumnCount(ROW);
					    table->setRowCount(1);
                    }else
                    {
                        table->setColumnCount(COL);
					    table->setRowCount(ROW);
                    }
					for (int i = 0; i < ROW; ++i)
					{
						for (int j = 0; j < COL; ++j)
						{
                            if(COL != 1)
                                table->setColumnWidth(j, 40);
                            else
                                table->setColumnWidth(i, 40);
							QTableWidgetItem* item = new QTableWidgetItem();
							items.push_back(item);
                            if(COL == 1)
							    table->setItem(0, i, item);
                            else
                                table->setItem(i, j, item);
						}
					}
                    
					proxy->connect(table, SIGNAL(cellChanged(int, int)), proxy, SLOT(on_update(int, int)));
				}
				virtual void SetData(::cv::Matx<T, ROW, COL>* data)
				{
					matData = data;
					UpdateUi(data);
				}
                ::cv::Matx<T, ROW, COL>* GetData()
                {
                    return matData;
                }
				virtual void UpdateUi( ::cv::Matx<T, ROW, COL>* data)
				{
					if(data)
                    {
						_currently_updating = true;
                        std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
                        for (int i = 0; i < ROW; ++i)
					    {
						    for (int j = 0; j < COL; ++j)
						    {
                                items[i*COL + j]->setToolTip(QString::number((*data)(i,j)));
							    items[i*COL + j]->setData(Qt::EditRole, (*data)(i, j));
						    }
					    }
						_currently_updating = false;
                    }					
				}
				virtual void OnUiUpdate(QObject* sender, int row = -1, int col = -1)
				{
					if(_currently_updating)
						return;
					if (sender == table)
					{
						std::lock_guard<std::recursive_mutex>lock(*IHandler::GetParamMtx());
						if (matData)
						{
							if (typeid(T) == typeid(float))
								(*matData)(row, col) = (T)items[row* COL + col]->data(Qt::EditRole).toFloat();
							if(typeid(T) == typeid(double))
								(*matData)(row, col) = (T)items[row* COL + col]->data(Qt::EditRole).toDouble();
							if(typeid(T) == typeid(int))
								(*matData)(row, col) = (T)items[row* COL + col]->data(Qt::EditRole).toInt();
							if(typeid(T) == typeid(unsigned int))
								(*matData)(row, col) = (T)items[row* COL + col]->data(Qt::EditRole).toUInt();
                            if(_listener)
                                _listener->OnUpdate(this);
						}
					}
					else if (row == -1 && col == -1)
					{
						if (matData)
							UpdateUi(matData);
                        if(_listener)
                                _listener->OnUpdate(this);
					}

				}
				virtual std::vector<QWidget*> GetUiWidgets(QWidget* parent)
				{
					std::vector<QWidget*> output;
					output.push_back(table);
					return output;
				}
			};
			template<typename T, int ROW> class Handler<typename ::cv::Vec<T, ROW>, void> : public Handler<::cv::Matx<T,ROW,1>>
			{
			};

			template<typename T> class Handler<typename ::cv::Scalar_<T>, void> : public Handler<::cv::Vec<T, 4>>
			{
			};
			// **********************************************************************************
			// *************************** cv::Point_ *********************************************
			// **********************************************************************************
			template<typename T> class Handler<typename ::cv::Point_<T>, void> : public UiUpdateHandler
			{
				QTableWidget* table;
				QTableWidgetItem* first;
				QTableWidgetItem* second;
				::cv::Point_<T>* ptData;
				bool _currently_updating;
			public:
				Handler():ptData(nullptr), _currently_updating(false)
				{
					
					table = new QTableWidget();
					first = new QTableWidgetItem();
					second = new QTableWidgetItem();
					table->horizontalHeader()->hide();
					table->verticalHeader()->hide();
					table->setItem(0, 0, first);
					table->setItem(0, 1, second);
					table->setRowCount(1);
					table->setColumnCount(2);
                    table->setColumnWidth(0, 40);
                    table->setColumnWidth(1, 40);
					proxy->connect(table, SIGNAL(cellChanged(int, int)), proxy, SLOT(on_update(int, int)));
				}
				virtual void UpdateUi(::cv::Point_<T>* data)
				{
					if(data)
                    {
                        std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
					    if (table)
					    {
							_currently_updating = true;
						    first = new QTableWidgetItem();
						    second = new QTableWidgetItem();
						    first->setData(Qt::EditRole, ptData->x);
						    second->setData(Qt::EditRole, ptData->y);
                            first->setToolTip(QString::number(ptData->x));
                            second->setToolTip(QString::number(ptData->y));
						    table->setItem(0, 0, first);
						    table->setItem(0, 1, second);
						    table->update();
							_currently_updating = false;
					    }
                    }					
				}
				virtual void OnUiUpdate(QObject* sender, int row = -1, int col = -1)
				{
					if(_currently_updating)
						return;
					std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
					if (ptData == nullptr)
						return;
					if (typeid(T) == typeid(double))
					{
						ptData->x = (T)first->data(Qt::EditRole).toDouble();
						ptData->y = (T)second->data(Qt::EditRole).toDouble();
					}
					if (typeid(T) == typeid(float))
					{
						ptData->x = (T)first->data(Qt::EditRole).toFloat();
						ptData->y = (T)second->data(Qt::EditRole).toFloat();
					}
					if (typeid(T) == typeid(int))
					{
						ptData->x = (T)first->data(Qt::EditRole).toInt();
						ptData->y = (T)second->data(Qt::EditRole).toInt();
					}
					if (typeid(T) == typeid(unsigned int))
					{
						ptData->x = (T)first->data(Qt::EditRole).toUInt();
						ptData->y = (T)second->data(Qt::EditRole).toUInt();
					}
                    if(_listener)
                        _listener->OnUpdate(this);
				}
				virtual void SetData(::cv::Point_<T>* data_)
				{
					ptData = data_;
					UpdateUi(ptData);
				}
                ::cv::Point_<T>* GetData()
                {
                    return ptData;
                }
				virtual std::vector<QWidget*> GetUiWidgets(QWidget* parent)
				{
					
					std::vector<QWidget*> output;
					output.push_back(table);
					return output;
				}
			};
			// **********************************************************************************
			// *************************** cv::Point3_ *********************************************
			// **********************************************************************************
			template<typename T> class Handler<typename ::cv::Point3_<T>, void> : public UiUpdateHandler
            {
                QTableWidget* table;
                QTableWidgetItem* first;
                QTableWidgetItem* second;
                QTableWidgetItem* third;
                ::cv::Point3_<T>* ptData;
                bool _updating;
            public:
                Handler():ptData(nullptr)
				{
					
                    table = new QTableWidget();
                    first = new QTableWidgetItem();
                    second = new QTableWidgetItem();
                    third = new QTableWidgetItem();
                    table->horizontalHeader()->hide();
                    table->verticalHeader()->hide();
                    table->setItem(0, 0, first);
                    table->setItem(0, 1, second);
                    table->setItem(0, 2, third);
                    table->setRowCount(1);
                    table->setColumnCount(3);
                    table->setColumnWidth(0, 40);
                    table->setColumnWidth(1, 40);
                    table->setColumnWidth(2, 40);
                    proxy->connect(table, SIGNAL(cellChanged(int, int)), proxy, SLOT(on_update(int, int)));
                }
                virtual void UpdateUi( ::cv::Point3_<T>* data)
				{
                    if(data)
                    {
						_updating = true;
                        std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
                        first = new QTableWidgetItem();
                        second = new QTableWidgetItem();
                        third = new QTableWidgetItem();
                        first->setData(Qt::EditRole, ptData->x);
                        second->setData(Qt::EditRole, ptData->y);
                        third->setData(Qt::EditRole, ptData->z);
                        first->setToolTip(QString::number(ptData->x));
                        second->setToolTip(QString::number(ptData->y));
                        third->setToolTip(QString::number(ptData->z));
                        table->setItem(0, 0, first);
                        table->setItem(0, 1, second);
                        table->setItem(0, 2, third);
						_updating = false;
                    }					
                }
                virtual void OnUiUpdate(QObject* sender, int row = -1, int col = -1)
				{
					if(_updating)
						return;
					std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
                    if (ptData == nullptr)
                        return;
                    if (typeid(T) == typeid(double))
                    {
                        ptData->x = (T)first->data(Qt::EditRole).toDouble();
                        ptData->y = (T)second->data(Qt::EditRole).toDouble();
                        ptData->z = (T)third->data(Qt::EditRole).toDouble();
                    }
                    if (typeid(T) == typeid(float))
                    {
                        ptData->x = (T)first->data(Qt::EditRole).toFloat();
                        ptData->y = (T)second->data(Qt::EditRole).toFloat();
                        ptData->z = (T)third->data(Qt::EditRole).toFloat();
                    }
                    if (typeid(T) == typeid(int))
                    {
                        ptData->x = (T)first->data(Qt::EditRole).toInt();
                        ptData->y = (T)second->data(Qt::EditRole).toInt();
                        ptData->z = (T)third->data(Qt::EditRole).toInt();
                    }
                    if (typeid(T) == typeid(unsigned int))
                    {
                        ptData->x = (T)first->data(Qt::EditRole).toUInt();
                        ptData->y = (T)second->data(Qt::EditRole).toUInt();
                        ptData->z = (T)third->data(Qt::EditRole).toUInt();
                    }
                    if(_listener)
                                _listener->OnUpdate(this);
                }
                virtual void SetData(::cv::Point3_<T>* data_)
				{
					
                    ptData = data_;
                    UpdateUi(ptData);
                }
                ::cv::Point3_<T>* GetData()
                {
                    return ptData;
                }
                virtual std::vector<QWidget*> GetUiWidgets(QWidget* parent)
				{
					
                    std::vector<QWidget*> output;
                    output.push_back(table);
                    return output;
                }
            };
#endif
			
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
					if(_updating)
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
					if(_currently_updating)
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
					if (sender == btn)
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
					if(_currently_updating)
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
					if(_currently_updating)
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
			// *************************** std::pair ********************************************
			// **********************************************************************************

			template<typename T1> class Handler<std::pair<T1, T1>>  : public IHandler
			{
				std::pair<T1,T1>* pairData;
                Handler<T1> _handler1;
                Handler<T1> _handler2;
				bool _currently_updating;
			public:
				Handler() : pairData(nullptr), _currently_updating(false) {}

				virtual void UpdateUi( std::pair<T1, T1>* data)
				{
					_currently_updating = true;
                    if(data)
                    {
                        _handler1.UpdateUi(&data->first);
                        _handler2.UpdateUi(&data->second);
                    }else
                    {
                        _handler1.UpdateUi(nullptr);
                        _handler2.UpdateUi(nullptr);
                    }
					_currently_updating = false;
				}
				virtual void OnUiUpdate(QObject* sender)
				{
					if(_currently_updating)
						return;
					std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
					_handler1.OnUiUpdate(sender);
					_handler2.OnUiUpdate(sender);
                    if(_listener)
                        _listener->OnUpdate(this);
				}
				virtual void SetData(std::pair<T1, T1>* data_)
				{
					pairData = data_;
                    if(data_)
                    {
                        _handler1.SetData(&data_->first);
					    _handler2.SetData(&data_->second);
                    }
				}
                std::pair<T1, T1>* GetData()
                {
                    return pairData;
                }
				virtual std::vector<QWidget*> GetUiWidgets(QWidget* parent)
				{
					auto out1 = _handler1.GetUiWidgets(parent);
					auto out2 = _handler2.GetUiWidgets(parent);
					out2.insert(out2.end(), out1.begin(), out1.end());
					return out2;
				}
                virtual void SetParamMtx(std::recursive_mutex* mtx)
                {
                    IHandler::SetParamMtx(mtx);
                    _handler1.SetParamMtx(mtx);
                    _handler2.SetParamMtx(mtx);
                }
                virtual void SetUpdateListener(UiUpdateListener* listener)
                {
                    _handler1.SetUpdateListener(listener);
                    _handler2.SetUpdateListener(listener);
                }
			};

			template<typename T1, typename T2> class Handler<std::pair<T1, T2>>: public Handler<T1>, public Handler<T2>
			{
				std::pair<T1,T2>* pairData;
			public:
				Handler() : pairData(nullptr) {}

				virtual void UpdateUi( std::pair<T1, T2>* data)
				{
					
				}
				virtual void OnUiUpdate(QObject* sender)
				{
					std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
					Handler<T1>::OnUiUpdate(sender);
					Handler<T2>::OnUiUpdate(sender);
				}
				virtual void SetData(std::pair<T1, T2>* data_)
				{
					pairData = data_;
					Handler<T1>::SetData(&data_->first);
					Handler<T2>::SetData(&data_->second);
				}
                std::pair<T1, T2>* GetData()
                {
                    return pairData;
                }
				virtual std::vector<QWidget*> GetUiWidgets(QWidget* parent)
				{
					
					auto output = Handler<T1>::GetUiWidgets(parent);
					auto out2 = Handler<T2>::GetUiWidgets(parent);
					output.insert(output.end(), out2.begin(), out2.end());
					return output;
				}
			};

			// **********************************************************************************
			// *************************** std::vector ******************************************
			// **********************************************************************************
			template<typename T> class Handler<std::vector<T>> : public Handler < T >, public UiUpdateListener
			{
				std::vector<T>* vectorData;
                T _appendData;
				QSpinBox* index;
				bool _currently_updating;
			public:
				Handler(): index(new QSpinBox()), vectorData(nullptr), _currently_updating(false) 
                {
                    Handler<T>::SetUpdateListener(this);
                }
				virtual void UpdateUi( std::vector<T>* data)
				{
					if (data && data->size())
					{
                        std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
						_currently_updating = true;
						index->setMaximum(data->size());
                        if(index->value() < data->size())
						    Handler<T>::UpdateUi(&(*data)[index->value()]);
                        else
                            Handler<T>::UpdateUi(&_appendData);
						_currently_updating = false;
					}
				}
				virtual void OnUiUpdate(QObject* sender, int idx = 0)
				{
					if(_currently_updating)
						return;
					if (sender == index && vectorData )
					{
                        if(vectorData->size() && idx < vectorData->size())
                        {
                            std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
						    Handler<T>::SetData(&(*vectorData)[idx]);
    						Handler<T>::OnUiUpdate(sender);
                        }else
                        {
                            Handler<T>::SetData(&_appendData);
    						Handler<T>::OnUiUpdate(sender);
                        }   
					}
				}
				virtual void SetData(std::vector<T>* data_)
				{
					vectorData = data_;
					if (vectorData)
					{
						if (data_->size())
						{
							if (index && index->value() < vectorData->size())
								Handler<T>::SetData(&(*vectorData)[index->value()]);
						}
					}
				}
                std::vector<T>* GetData()
                {
                    return vectorData;
                }
				virtual std::vector<QWidget*> GetUiWidgets(QWidget* parent)
				{
					auto output = Handler<T>::GetUiWidgets(parent);
					index->setParent(parent);
					index->setMinimum(0);
					IHandler::proxy->connect(index, SIGNAL(valueChanged(int)), IHandler::proxy, SLOT(on_update(int)));
					output.push_back(index);
					return output;
				}
                virtual void OnUpdate(IHandler* handler)
                {
                    if(Handler<T>::GetData() == &_appendData && vectorData)
                    {
                        vectorData->push_back(_appendData);
                        Handler<T>::SetData(&vectorData->back());
                        index->setMaximum(vectorData->size());
                    }
                }
			};

			// **********************************************************************************
			// *************************** ParameterProxy ***************************************
			// **********************************************************************************

			template <typename T>
			class has_minmax
			{
				typedef char one;
				typedef long two;

				template <typename C> static one test(decltype(&C::SetMinMax));
				template <typename C> static two test(...);

			public:
				enum { value = sizeof(test<T>(0)) == sizeof(char) };
			};



            template<typename T> void SetMinMax(typename std::enable_if<has_minmax<Handler<T>>::value, Handler<T>>::type& handler, Parameters::ITypedParameter<T>* param)
            {
                auto rangedParam = dynamic_cast<ITypedRangedParameter<T>*>(param);
                if (rangedParam)
                {
                    typename Handler<T>::min_max_type min, max;
                    rangedParam->GetRange(min, max);
                    handler.SetMinMax(min, max);
                }
            }
            
            template<typename T> void SetMinMax(typename std::enable_if<!has_minmax<Handler<T>>::value, Handler<T>>::type& handler, Parameters::ITypedParameter<T>* param)
            {
                
            }
			
			template<typename T> class ParameterProxy : public IParameterProxy
			{
				Handler<T> paramHandler;
				Parameters::ITypedParameter<T>* parameter;
				std::shared_ptr<Signals::connection> connection;
				std::shared_ptr<Signals::connection> delete_connection;
			public:
                ~ParameterProxy()
                {
                    InvalidCallbacks::invalidate((void*)&paramHandler);
                }
				void onUiUpdate()
				{
					//TODO Notify parameter of update on the processing thread.
					parameter->changed = true;
					parameter->OnUpdate(nullptr);
				}
                // Guaranteed to be called on the GUI thread thanks to the signal connection configuration
				void onParamUpdate(cv::cuda::Stream* stream)
				{
					auto dataPtr = parameter->Data();	
					if (dataPtr)
					{
						if (Handler<T>::UiUpdateRequired())
						{
                            paramHandler.UpdateUi(dataPtr);
						}
					}
				}
				void onParamDelete()
				{
					parameter = nullptr;
					connection.reset();
					delete_connection.reset();
					paramHandler.SetParamMtx(nullptr);
				}
			public:
				ParameterProxy(Parameters::Parameter* param)
				{
					SetParameter(param);
				}
				virtual bool CheckParameter(Parameter* param)
				{
					return param == parameter;
				}
				QWidget* GetParameterWidget(QWidget* parent)
				{
					QWidget* output = new QWidget(parent);
					auto widgets = paramHandler.GetUiWidgets(output);
                    SetMinMax<T>(paramHandler, parameter);
					QGridLayout* layout = new QGridLayout(output);
					if (parameter->GetTypeInfo() == Loki::TypeInfo(typeid(std::function<void(void)>)))
					{
						dynamic_cast<QPushButton*>(widgets[0])->setText(QString::fromStdString(parameter->GetName()));
						layout->addWidget(widgets[0], 0, 0);
					}
					else
					{
						QLabel* nameLbl = new QLabel(QString::fromStdString(parameter->GetName()), output);
                        nameLbl->setToolTip(QString::fromStdString(parameter->GetTypeInfo().name()));
						layout->addWidget(nameLbl, 0, 0);
						int count = 1;
						output->setLayout(layout);
						for (auto itr = widgets.rbegin(); itr != widgets.rend(); ++itr, ++count)
						{
							layout->addWidget(*itr, 0, count);
						}
                        // Correct the tab order of the widgets
                        for(int i = widgets.size() - 1; i > 0; --i)
                        {
                            QWidget::setTabOrder(widgets[i], widgets[i - 1]);
                        }
						paramHandler.UpdateUi(parameter->Data());
					}
					return output;
				}
				virtual bool SetParameter(Parameter* param)
				{
					auto typedParam = dynamic_cast<Parameters::ITypedParameter<T>*>(param);
					if (typedParam)
					{
						parameter = typedParam;
						paramHandler.SetParamMtx(&parameter->mtx());
						paramHandler.SetData(parameter->Data());
						paramHandler.IHandler::GetOnUpdate() = std::bind(&ParameterProxy<T>::onUiUpdate, this);
						connection = parameter->update_signal.connect(std::bind(&ParameterProxy<T>::onParamUpdate, this, std::placeholders::_1), Signals::GUI, true, this);
						delete_connection = parameter->delete_signal.connect(std::bind(&ParameterProxy<T>::onParamDelete, this));
						return true;
					}
					return false;
				}
			};

			// **********************************************************************************
			// *************************** Factory **********************************************
			// **********************************************************************************

			template<typename T> class Factory
			{
			public:
				Factory()
				{
					WidgetFactory::RegisterCreator(Loki::TypeInfo(typeid(T)), std::bind(&Factory<T>::Create, std::placeholders::_1));
				}
				static std::shared_ptr<IParameterProxy> Create(Parameters::Parameter* param)
				{
					return std::shared_ptr<IParameterProxy>(new ParameterProxy<T>(param));
				}
			};
			template<typename T> class QtUiPolicy
			{
				static Factory<T> factory;
			public:
				QtUiPolicy()
				{
					(void)&factory;
					//WidgetFactory::RegisterCreator(Loki::TypeInfo(typeid(T)), std::bind(&Factory<T>::Create, std::placeholders::_1));
				}
			};
			template<typename T> Factory<T> QtUiPolicy<T>::factory = Factory<T>();

		} /* namespace qt */
	} /* namespace UI */
} /* namespace Perameters */

#endif // Qt5_FOUND
