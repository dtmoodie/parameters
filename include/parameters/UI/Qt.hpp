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
#if defined(OPENCV_FOUND) || defined(CV_EXPORTS) || defined(CVAPI_EXPORTS)
#include <opencv2/core/types.hpp>
#endif
#include <memory>
#include <signals/connection.h>
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

	namespace UI
	{
		namespace qt
		{
			class IHandler;
			class IParameterProxy;
			// *****************************************************************************
			//								WidgetFactory
			// *****************************************************************************
			class Parameter_EXPORTS WidgetFactory
			{
			public:
				typedef std::function<std::shared_ptr<IParameterProxy>(std::shared_ptr<Parameters::Parameter>)> HandlerCreator;
                static void RegisterCreator(Loki::TypeInfo type, HandlerCreator f);
				static std::shared_ptr<IParameterProxy> Createhandler(std::shared_ptr<Parameters::Parameter> param);

			private:
				static std::map<Loki::TypeInfo, HandlerCreator>& registry();
			};			
			// *****************************************************************************
			//								IParameterProxy
			// *****************************************************************************
			class Parameter_EXPORTS IParameterProxy
			{
			protected:
			public:
				typedef std::shared_ptr<IParameterProxy> Ptr;
                IParameterProxy();
                virtual ~IParameterProxy();
				
				virtual QWidget* GetParameterWidget(QWidget* parent) = 0;
				virtual bool CheckParameter(Parameter* param) = 0;
			};
			// *****************************************************************************
			//								SignalProxy
			// *****************************************************************************
			class Parameter_EXPORTS SignalProxy : public QObject
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
			class Parameter_EXPORTS DefaultProxy: public IParameterProxy
			{
				std::shared_ptr<Parameters::Parameter> parameter;
				void onUiUpdate()
				{			}
				void onParamUpdate()
				{				}
			public:
				DefaultProxy(std::shared_ptr<Parameters::Parameter> param);
				virtual bool CheckParameter(Parameter* param);
				QWidget* GetParameterWidget(QWidget* parent);
			};
			// *****************************************************************************
			//								IHandler
			// *****************************************************************************
			class Parameter_EXPORTS IHandler
			{
            private:
                std::recursive_mutex* paramMtx;
			protected:
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
				
                virtual std::function<void(void)>& GetUpdateSignal();
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
				virtual void UpdateUi(const T& data)
				{}
				virtual void OnUiUpdate(QObject* sender)
				{}
				virtual void SetData(T*)
				{

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
			public:
				Handler() : chkBox(nullptr), boolData(nullptr) {}
				virtual void UpdateUi(const bool& data)
				{
					
					chkBox->setChecked(data);
				}
				virtual void OnUiUpdate(QObject* sender, int val)
				{
					
					if (sender == chkBox)
					{
                        std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
						if (boolData)
						{
							*boolData = chkBox->isChecked();
							if (onUpdate)
								onUpdate();
						}							
					}
				}
				virtual void SetData(bool* data_)
				{
					
					std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
					boolData = data_;
					if (chkBox)
						UpdateUi(*data_);
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
			public:
				Handler(): btn(nullptr), parent(nullptr){}
				virtual void UpdateUi(const T& data)
				{
					
					std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
					btn->setText(QString::fromStdString(data.string()));
				}
				virtual void OnUiUpdate(QObject* sender)
				{
					
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
					}					
				}
				virtual void SetData(T* data_)
				{
					
					fileData = data_;
					if (btn)
						UpdateUi(*data_);
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
			public:
				Handler() : table(nullptr), matData(nullptr), UiUpdateHandler()
				{
					
					table = new QTableWidget();
					table->horizontalHeader()->hide();
					table->verticalHeader()->hide();
					items.reserve(ROW*COL);
					table->setColumnCount(COL);
					table->setRowCount(ROW);
					for (int i = 0; i < ROW; ++i)
					{
						for (int j = 0; j < COL; ++j)
						{
							QTableWidgetItem* item = new QTableWidgetItem();
							items.push_back(item);
							table->setItem(i, j, item);
						}
					}
					proxy->connect(table, SIGNAL(cellChanged(int, int)), proxy, SLOT(on_update(int, int)));
				}
				virtual void SetData(::cv::Matx<T, ROW, COL>* data)
				{
					
					matData = data;
					UpdateUi(*data);
				}
				virtual void UpdateUi(const ::cv::Matx<T, ROW, COL>& data)
				{
					
					std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
					for (int i = 0; i < ROW; ++i)
					{
						for (int j = 0; j < COL; ++j)
						{
							items[i*COL + j]->setData(Qt::EditRole, data(i, j));
						}
					}
				}
				virtual void OnUiUpdate(QObject* sender, int row = -1, int col = -1)
				{
					
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
						}
					}
					else if (row == -1 && col == -1)
					{
						if (matData)
							UpdateUi(*matData);
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
			public:
				Handler():ptData(nullptr)
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
					proxy->connect(table, SIGNAL(cellChanged(int, int)), proxy, SLOT(on_update(int, int)));
				}
				virtual void UpdateUi(const ::cv::Point_<T>& data)
				{
					
					std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
					if (table)
					{
						first = new QTableWidgetItem();
						second = new QTableWidgetItem();
						first->setData(Qt::EditRole, ptData->x);
						second->setData(Qt::EditRole, ptData->y);
						table->setItem(0, 0, first);
						table->setItem(0, 1, second);
						table->update();
					}
				}
				virtual void OnUiUpdate(QObject* sender, int row = -1, int col = -1)
				{
					
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
				}
				virtual void SetData(::cv::Point_<T>* data_)
				{
					
					ptData = data_;
					UpdateUi(*ptData);
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
                    proxy->connect(table, SIGNAL(cellChanged(int, int)), proxy, SLOT(on_update(int, int)));
                }
                virtual void UpdateUi(const ::cv::Point3_<T>& data)
				{
					std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
                    first = new QTableWidgetItem();
                    second = new QTableWidgetItem();
                    third = new QTableWidgetItem();
                    first->setData(Qt::EditRole, ptData->x);
                    second->setData(Qt::EditRole, ptData->y);
                    third->setData(Qt::EditRole, ptData->z);
                    table->setItem(0, 0, first);
                    table->setItem(0, 1, second);
                    table->setItem(0, 2, third);
                }
                virtual void OnUiUpdate(QObject* sender, int row = -1, int col = -1)
				{
					
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
                }
                virtual void SetData(::cv::Point3_<T>* data_)
				{
					
                    ptData = data_;
                    UpdateUi(*ptData);
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
				virtual void UpdateUi(const Parameters::EnumParameter& data)
				{
					std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
                    _updating = true;
					enumCombo->clear();
					for (int i = 0; i < data.enumerations.size(); ++i)
					{
						enumCombo->addItem(QString::fromStdString(data.enumerations[i]));
					}
                    enumCombo->setCurrentIndex(data.currentSelection);
                    _updating = false;
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
						enumData->currentSelection = idx;
						if (onUpdate)
							onUpdate();
					}
				}
				virtual void SetData(Parameters::EnumParameter* data_)
				{
					enumData = data_;
					if (enumCombo)
						UpdateUi(*enumData);
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
			public:
				Handler() : strData(nullptr), lineEdit(nullptr) {}
				virtual void UpdateUi(const std::string& data)
				{
					
					std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
					lineEdit->setText(QString::fromStdString(data));
				}
				virtual void OnUiUpdate(QObject* sender)
				{
					
					if (sender == lineEdit && strData)
					{	
						std::lock_guard<std::recursive_mutex>lock(*IHandler::GetParamMtx());
						*strData = lineEdit->text().toStdString();
						if (onUpdate)
							onUpdate();
					}
				}
				virtual void SetData(std::string* data_)
				{
					
					strData = data_;
					if (lineEdit)
						lineEdit->setText(QString::fromStdString(*strData));
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
				void UpdateUi(std::function<void(void)>& data)
				{
                    funcData = &data;
                }
				virtual void OnUiUpdate(QObject* sender)
				{
					
					if (sender == btn)
					{
						std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
						if (funcData)
						{
							//ProcessingThreadCallbackService::post(*funcData, 
                                //std::make_pair((void*)this, Loki::TypeInfo(typeid(Handler<std::function<void(void)>, void>))));
                            (*funcData)();
							if (onUpdate)
							{
								//ProcessingThreadCallbackService::post(onUpdate, 
                                  //  std::make_pair((void*)this, Loki::TypeInfo(typeid(Handler<std::function<void(void)>, void>))));
                                onUpdate();
							}
						}
					}
				}
				virtual void SetData(std::function<void(void)>* data_)
				{
					funcData = data_;
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
			public:
				Handler() : box(nullptr), floatData(nullptr) {}
				virtual void UpdateUi(const T& data)
				{
					
					box->setValue(data);
				}
				virtual void OnUiUpdate(QObject* sender, double val = 0)
				{
					
					std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
					if (sender == box && floatData)
						*floatData = box->value();
					if (onUpdate)
						onUpdate();
				}
				virtual void SetData(T* data_)
				{
					
					std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
					floatData = data_;
					if (box)
						box->setValue(*floatData);
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
			public:
				Handler() : box(nullptr), intData(nullptr) {}
				virtual void UpdateUi(const T& data)
				{
					
					std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
					box->setValue(data);
				}
				virtual void OnUiUpdate(QObject* sender, int val = -1)
				{
					
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
				}
				virtual void SetData(T* data_)
				{
					
					intData = data_;
					if (box)
						box->setValue(*intData);
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

			template<typename T1> class Handler<std::pair<T1, T1>> : public Handler<T1>
			{
				std::pair<T1,T1>* pairData;
			public:
				Handler() : pairData(nullptr) {}

				virtual void UpdateUi(const std::pair<T1, T1>& data)
				{
					

				}
				virtual void OnUiUpdate(QObject* sender)
				{
					
					std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
					Handler<T1>::OnUiUpdate(sender);
					Handler<T1>::OnUiUpdate(sender);
				}
				virtual void SetData(std::pair<T1, T1>* data_)
				{
					
					pairData = data_;
					Handler<T1>::SetData(&data_->first);
					Handler<T1>::SetData(&data_->second);
				}
				virtual std::vector<QWidget*> GetUiWidgets(QWidget* parent)
				{
					
					auto output = Handler<T1>::GetUiWidgets(parent);
					auto out2 = Handler<T1>::GetUiWidgets(parent);
					output.insert(output.end(), out2.begin(), out2.end());
					return output;
				}
			};

			template<typename T1, typename T2> class Handler<std::pair<T1, T2>>: public Handler<T1>, public Handler<T2>
			{
				std::pair<T1,T2>* pairData;
			public:
				Handler() : pairData(nullptr) {}

				virtual void UpdateUi(const std::pair<T1, T2>& data)
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
			template<typename T> class Handler<std::vector<T>> : public Handler < T >
			{
				std::vector<T>* vectorData;
				QSpinBox* index;
			public:
				Handler() : index(new QSpinBox()), vectorData(nullptr) {}
				virtual void UpdateUi(const std::vector<T>& data)
				{
					
					if (data.size())
					{
                        std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
						index->setMaximum(data.size() - 1);
						Handler<T>::UpdateUi(data[index->value()]);
					}
				}
				virtual void OnUiUpdate(QObject* sender, int idx = 0)
				{
					
					if (sender == index && vectorData && vectorData->size() && idx < vectorData->size())
					{
                        std::lock_guard<std::recursive_mutex> lock(*IHandler::GetParamMtx());
						Handler<T>::SetData(&(*vectorData)[idx]);
						Handler<T>::OnUiUpdate(sender);
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
				virtual std::vector<QWidget*> GetUiWidgets(QWidget* parent)
				{
					
					auto output = Handler<T>::GetUiWidgets(parent);
					index->setParent(parent);
					index->setMinimum(0);
					IHandler::proxy->connect(index, SIGNAL(valueChanged(int)), IHandler::proxy, SLOT(on_update(int)));
					output.push_back(index);
					return output;
				}
			};

			// **********************************************************************************
			// *************************** ParameterProxy ***************************************
			// **********************************************************************************
			template<typename T> class ParameterProxy : public IParameterProxy
			{
				Handler<T> paramHandler;
				std::shared_ptr<Parameters::ITypedParameter<T>> parameter;
				std::shared_ptr<Signals::connection> connection;
			public:
                ~ParameterProxy()
                {
                    InvalidCallbacks::invalidate((void*)&paramHandler);
                }
				void onUiUpdate()
				{
					
					//TODO Notify parameter of update on the processing thread.
					parameter->changed = true;
					parameter->UpdateSignal(nullptr);
				}
				void onParamUpdate(cv::cuda::Stream* stream)
				{
					
					auto dataPtr = parameter->Data();	
					if (dataPtr)
					{
						if (Handler<T>::UiUpdateRequired())
						{
							UiCallbackService::Instance()->post(
                                std::bind(&Handler<T>::UpdateUi, &paramHandler, std::ref(*dataPtr)), 
                                std::make_pair((void*)&paramHandler, Loki::TypeInfo(typeid(Handler<T>))));
						}
					}
				}
			public:
				ParameterProxy(std::shared_ptr<Parameters::Parameter> param)
				{
					
					auto typedParam = std::dynamic_pointer_cast<Parameters::ITypedParameter<T>>(param);
					if (typedParam)
					{
						parameter = typedParam;
						paramHandler.SetParamMtx(&parameter->mtx);
						paramHandler.SetData(parameter->Data());
						paramHandler.IHandler::GetUpdateSignal() = std::bind(&ParameterProxy<T>::onUiUpdate, this);
                        connection = parameter->UpdateSignal.connect(std::bind(&ParameterProxy<T>::onParamUpdate, this, std::placeholders::_1),      Signals::GUI, true);
						/*connection = parameter->RegisterNotifier(std::bind(&ParameterProxy<T>::onParamUpdate, this, std::placeholders::_1));*/
					}
				}
				virtual bool CheckParameter(Parameter* param)
				{
					
					return param == parameter.get();
				}
				QWidget* GetParameterWidget(QWidget* parent)
				{
					
					QWidget* output = new QWidget(parent);
					auto widgets = paramHandler.GetUiWidgets(output);
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
						paramHandler.UpdateUi(*parameter->Data());
					}
					return output;
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
				static std::shared_ptr<IParameterProxy> Create(std::shared_ptr<Parameters::Parameter> param)
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