#pragma once
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
#include <LokiTypeInfo.h>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include "../Types.hpp"
#include "InterThread.hpp"
#if defined(OPENCV_FOUND) || defined(CV_EXPORTS) || defined(CVAPI_EXPORTS)
#include <opencv2/core/types.hpp>
#endif
#include <memory>
#include <boost/log/trivial.hpp>
#include <boost/log/attributes/named_scope.hpp>
namespace Parameters{
	class Parameter;
	template<typename T> class ITypedParameter;

	namespace UI{
		namespace qt{
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
				static std::map<Loki::TypeInfo, HandlerCreator> registry;
			};			
			// *****************************************************************************
			//								IParameterProxy
			// *****************************************************************************
			class Parameter_EXPORTS IParameterProxy
			{
			protected:
			public:
				typedef std::shared_ptr<IParameterProxy> Ptr;
				IParameterProxy()
				{
					
				}
				
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
				
			protected:
				SignalProxy* proxy;
				std::function<void(void)> onUpdate;
			public:
				
				IHandler() : write(true), paramMtx(nullptr), proxy(new SignalProxy(this)) {}
                virtual void OnUiUpdate(QObject* sender) {}
				virtual void OnUiUpdate(QObject* sender, double val) {}
				virtual void OnUiUpdate(QObject* sender, int val) {}
				virtual void OnUiUpdate(QObject* sender, bool val) {}
				virtual void OnUiUpdate(QObject* sender, QString val) {}
				virtual void OnUiUpdate(QObject* sender, int row, int col) {}
				std::function<void(void)>& GetUpdateSignal()
				{
					LOG_TRACE;
					return onUpdate;
				}
				virtual std::vector<QWidget*> GetUiWidgets(QWidget* parent)
				{
					LOG_TRACE;
					return std::vector<QWidget*>();
				}
				bool write;
				boost::recursive_mutex* paramMtx;
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
					LOG_TRACE;
					std::cout << "Creating handler for default unspecialized parameter" << std::endl;
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
					LOG_TRACE;
					std::cout << "Creating widget for default unspecialized parameter" << std::endl;
					return std::vector<QWidget*>();
				}
			};

			// **********************************************************************************
			// *************************** Bool ************************************************
			// **********************************************************************************
			template<> class Handler<bool, void>: public IHandler
			{
				QCheckBox* chkBox;
				bool* boolData;
			public:
				Handler() : chkBox(nullptr), boolData(nullptr) {}
				virtual void UpdateUi(const bool& data)
				{
					LOG_TRACE;
					chkBox->setChecked(data);
				}
				virtual void OnUiUpdate(QObject* sender)
				{
					LOG_TRACE;
					if (sender == chkBox)
					{
						boost::recursive_mutex::scoped_lock lock(*IHandler::paramMtx);
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
					LOG_TRACE;
					boost::recursive_mutex::scoped_lock lock(*IHandler::paramMtx);
					boolData = data_;
					if (chkBox)
						UpdateUi(*data_);
				}
				virtual std::vector < QWidget*> GetUiWidgets(QWidget* parent_)
				{
					LOG_TRACE;
					std::vector<QWidget*> output;
					if (chkBox == nullptr)
						chkBox = new QCheckBox(parent_);
					chkBox->connect(chkBox, SIGNAL(stateChanged(int)), IHandler::proxy, SLOT(on_update(int)));
					output.push_back(chkBox);
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
				std::is_same<T, Parameters::ReadFile>::value, void>::type> : public IHandler
			{
				QPushButton* btn;
				QWidget* parent;
				T* fileData;
			public:
				Handler(): btn(nullptr), parent(nullptr){}
				virtual void UpdateUi(const T& data)
				{
					LOG_TRACE;
					boost::recursive_mutex::scoped_lock lock(*IHandler::paramMtx);
					btn->setText(QString::fromStdString(data.string()));
				}
				virtual void OnUiUpdate(QObject* sender)
				{
					LOG_TRACE;
					if (sender == btn)
					{
						boost::recursive_mutex::scoped_lock lock(*IHandler::paramMtx);
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
					LOG_TRACE;
					fileData = data_;
					if (btn)
						UpdateUi(*data_);
				}
				virtual std::vector<QWidget*> GetUiWidgets(QWidget* parent_)
				{
					LOG_TRACE;
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
			template<typename T, int ROW, int COL> class Handler<typename ::cv::Matx<T, ROW, COL>, void> : public IHandler
			{
				QTableWidget* table;
				std::vector<QTableWidgetItem*> items;
				::cv::Matx<T, ROW, COL>* matData;
			public:
				Handler() : table(nullptr), matData(nullptr), IHandler()
				{
					LOG_TRACE;
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
					LOG_TRACE;
					matData = data;
					UpdateUi(*data);
				}
				virtual void UpdateUi(const ::cv::Matx<T, ROW, COL>& data)
				{
					LOG_TRACE;
					boost::recursive_mutex::scoped_lock lock(*IHandler::paramMtx);
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
					LOG_TRACE;
					if (sender == table)
					{
						boost::recursive_mutex::scoped_lock lock(*IHandler::paramMtx);
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
					LOG_TRACE;
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
			template<typename T> class Handler<typename ::cv::Point_<T>, void>: public IHandler
			{
				QTableWidget* table;
				QTableWidgetItem* first;
				QTableWidgetItem* second;
				::cv::Point_<T>* ptData;
			public:
				Handler():ptData(nullptr)
				{
					LOG_TRACE;
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
					LOG_TRACE;
					boost::recursive_mutex::scoped_lock lock(*IHandler::paramMtx);
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
					LOG_TRACE;
					boost::recursive_mutex::scoped_lock lock(*IHandler::paramMtx);
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
					LOG_TRACE;
					ptData = data_;
					UpdateUi(*ptData);
				}
				virtual std::vector<QWidget*> GetUiWidgets(QWidget* parent)
				{
					LOG_TRACE;
					std::vector<QWidget*> output;
					output.push_back(table);
					return output;
				}
			};
			// **********************************************************************************
			// *************************** cv::Point3_ *********************************************
			// **********************************************************************************
            template<typename T> class Handler<typename ::cv::Point3_<T>, void>: public IHandler
            {
                QTableWidget* table;
                QTableWidgetItem* first;
                QTableWidgetItem* second;
                QTableWidgetItem* third;
                ::cv::Point3_<T>* ptData;
            public:
                Handler():ptData(nullptr)
				{
					LOG_TRACE;
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
					LOG_TRACE;
                    boost::recursive_mutex::scoped_lock lock(*IHandler::paramMtx);
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
					LOG_TRACE;
                    boost::recursive_mutex::scoped_lock lock(*IHandler::paramMtx);
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
					LOG_TRACE;
                    ptData = data_;
                    UpdateUi(*ptData);
                }
                virtual std::vector<QWidget*> GetUiWidgets(QWidget* parent)
				{
					LOG_TRACE;
                    std::vector<QWidget*> output;
                    output.push_back(table);
                    return output;
                }
            };
#endif
			
			// **********************************************************************************
			// *************************** Enums ************************************************
			// **********************************************************************************
			template<> class Handler<Parameters::EnumParameter, void> : public IHandler
			{
				QComboBox* enumCombo;
				Parameters::EnumParameter* enumData;
			public:
				Handler() : enumCombo(nullptr){}
				virtual void UpdateUi(const Parameters::EnumParameter& data)
				{
					LOG_TRACE;
					boost::recursive_mutex::scoped_lock lock(*IHandler::paramMtx);
					enumCombo->clear();
					for (int i = 0; i < data.enumerations.size(); ++i)
					{
						enumCombo->addItem(QString::fromStdString(data.enumerations[i]));
					}
				}
				virtual void OnUiUpdate(QObject* sender, int idx)
				{
					LOG_TRACE;
					boost::recursive_mutex::scoped_lock lock(*IHandler::paramMtx);
					if (idx != -1 && sender == enumCombo && enumData)
					{
						enumData->currentSelection = enumData->values[idx];
						if (onUpdate)
							onUpdate();
					}
				}
				virtual void SetData(Parameters::EnumParameter* data_)
				{
					LOG_TRACE;
					enumData = data_;
					if (enumCombo)
						UpdateUi(*enumData);
				}
				virtual std::vector<QWidget*> GetUiWidgets(QWidget* parent)
				{
					LOG_TRACE;
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

			template<> class Handler<std::string, void> : public IHandler
			{
				std::string* strData;
				QLineEdit* lineEdit;
			public:
				Handler() : strData(nullptr), lineEdit(nullptr) {}
				virtual void UpdateUi(const std::string& data)
				{
					LOG_TRACE;
					boost::recursive_mutex::scoped_lock lock(*IHandler::paramMtx);
					lineEdit->setText(QString::fromStdString(data));
				}
				virtual void OnUiUpdate(QObject* sender)
				{
					LOG_TRACE;
					if (sender == lineEdit && strData)
					{	
						boost::recursive_mutex::scoped_lock lock(*IHandler::paramMtx);
						*strData = lineEdit->text().toStdString();
						if (onUpdate)
							onUpdate();
					}
				}
				virtual void SetData(std::string* data_)
				{
					LOG_TRACE;
					strData = data_;
					if (lineEdit)
						lineEdit->setText(QString::fromStdString(*strData));
				}
				virtual std::vector<QWidget*> GetUiWidgets(QWidget* parent)
				{
					LOG_TRACE;
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
			// *************************** boost::function<void(void)> **************************
			// **********************************************************************************

			template<> class Handler<boost::function<void(void)>, void> : public IHandler
			{
				boost::function<void(void)>* funcData;
				QPushButton* btn;
			public:
				Handler() : funcData(nullptr), btn(nullptr) {}
				virtual void UpdateUi(const boost::function<void(void)>& data)
				{}
				virtual void OnUiUpdate(QObject* sender)
				{
					LOG_TRACE;
					if (sender == btn)
					{
						boost::recursive_mutex::scoped_lock lock(*IHandler::paramMtx);
						// TODO processing thread callback
						if (funcData)
						{
							(*funcData)();
							if (onUpdate)
								onUpdate();
						}
					}
				}
				virtual void SetData(boost::function<void(void)>* data_)
				{
					LOG_TRACE;
					funcData = data_;
				}
				virtual std::vector<QWidget*> GetUiWidgets(QWidget* parent)
				{
					LOG_TRACE;
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
			class Handler<T, typename std::enable_if<std::is_floating_point<T>::value, void>::type> : public IHandler
			{
				T* floatData;
				QDoubleSpinBox* box;
			public:
				Handler() : box(nullptr), floatData(nullptr) {}
				virtual void UpdateUi(const T& data)
				{
					LOG_TRACE;
					box->setValue(data);
				}
				virtual void OnUiUpdate(QObject* sender, double val = 0)
				{
					LOG_TRACE;
					boost::recursive_mutex::scoped_lock lock(*IHandler::paramMtx);
					if (sender == box && floatData)
						*floatData = box->value();
					if (onUpdate)
						onUpdate();
				}
				virtual void SetData(T* data_)
				{
					LOG_TRACE;
					boost::recursive_mutex::scoped_lock lock(*IHandler::paramMtx);
					floatData = data_;
					if (box)
						box->setValue(*floatData);
				}
				virtual std::vector<QWidget*> GetUiWidgets(QWidget* parent)
				{
					LOG_TRACE;
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
			class Handler<T, typename std::enable_if<std::is_integral<T>::value, void>::type> : public IHandler
			{
				T* intData;
				QSpinBox* box;
			public:
				Handler() : box(nullptr), intData(nullptr) {}
				virtual void UpdateUi(const T& data)
				{
					LOG_TRACE;
					boost::recursive_mutex::scoped_lock lock(*IHandler::paramMtx);
					box->setValue(data);
				}
				virtual void OnUiUpdate(QObject* sender, int val = -1)
				{
					LOG_TRACE;
					boost::recursive_mutex::scoped_lock lock(*IHandler::paramMtx);
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
					LOG_TRACE;
					intData = data_;
					if (box)
						box->setValue(*intData);
				}
				virtual std::vector<QWidget*> GetUiWidgets(QWidget* parent)
				{
					LOG_TRACE;
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
					LOG_TRACE;

				}
				virtual void OnUiUpdate(QObject* sender)
				{
					LOG_TRACE;
					boost::recursive_mutex::scoped_lock lock(*IHandler::paramMtx);
					Handler<T1>::OnUiUpdate(sender);
					Handler<T1>::OnUiUpdate(sender);
				}
				virtual void SetData(std::pair<T1, T1>* data_)
				{
					LOG_TRACE;
					pairData = data_;
					Handler<T1>::SetData(&data_->first);
					Handler<T1>::SetData(&data_->second);
				}
				virtual std::vector<QWidget*> GetUiWidgets(QWidget* parent)
				{
					LOG_TRACE;
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
					LOG_TRACE;
				}
				virtual void OnUiUpdate(QObject* sender)
				{
					LOG_TRACE;
					boost::recursive_mutex::scoped_lock lock(*IHandler::paramMtx);
					Handler<T1>::OnUiUpdate(sender);
					Handler<T2>::OnUiUpdate(sender);
				}
				virtual void SetData(std::pair<T1, T2>* data_)
				{
					LOG_TRACE;
					pairData = data_;
					Handler<T1>::SetData(&data_->first);
					Handler<T2>::SetData(&data_->second);
				}
				virtual std::vector<QWidget*> GetUiWidgets(QWidget* parent)
				{
					LOG_TRACE;
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
					LOG_TRACE;
					if (data.size())
					{
						boost::recursive_mutex::scoped_lock lock(*IHandler::paramMtx);
						index->setMaximum(data.size() - 1);
						Handler<T>::UpdateUi(data[index->value()]);
					}
				}
				virtual void OnUiUpdate(QObject* sender, int idx = 0)
				{
					LOG_TRACE;
					if (sender == index && vectorData && vectorData->size() && idx < vectorData->size())
					{
						boost::recursive_mutex::scoped_lock lock(*IHandler::paramMtx);
						Handler<T>::SetData(&(*vectorData)[idx]);
						Handler<T>::OnUiUpdate(sender);
					}						
				}
				virtual void SetData(std::vector<T>* data_)
				{
					LOG_TRACE;
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
					LOG_TRACE;
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
			public:
				void onUiUpdate()
				{
					LOG_TRACE;
					//TODO Notify parameter of update on the processing thread.
					parameter->changed = true;
					parameter->UpdateSignal();
				}
				void onParamUpdate()
				{
					LOG_TRACE;
					auto dataPtr = parameter->Data();	
					if (dataPtr)
					{
                        UiCallbackService::Instance()->post(boost::bind(&Handler<T>::UpdateUi, &paramHandler, boost::ref(*dataPtr)));
					}
				}
			public:
				ParameterProxy(std::shared_ptr<Parameters::Parameter> param)
				{
					LOG_TRACE;
					auto typedParam = std::dynamic_pointer_cast<Parameters::ITypedParameter<T>>(param);
					if (typedParam)
					{
						parameter = typedParam;
						paramHandler.IHandler::paramMtx = &parameter->mtx;
						paramHandler.SetData(parameter->Data());
						paramHandler.IHandler::GetUpdateSignal() = std::bind(&ParameterProxy<T>::onUiUpdate, this);
						parameter->RegisterNotifier(std::bind(&ParameterProxy<T>::onParamUpdate, this));
					}
				}
				virtual bool CheckParameter(Parameter* param)
				{
					LOG_TRACE;
					return param == parameter.get();
				}
				QWidget* GetParameterWidget(QWidget* parent)
				{
					LOG_TRACE;
					QWidget* output = new QWidget(parent);
					auto widgets = paramHandler.GetUiWidgets(output);
					QGridLayout* layout = new QGridLayout(output);
					if (parameter->GetTypeInfo() == Loki::TypeInfo(typeid(boost::function<void(void)>)))
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
					WidgetFactory::RegisterCreator(Loki::TypeInfo(typeid(T)), std::bind(&Factory<T>::Create, std::placeholders::_1));
				}
			};

		} /* namespace qt */
	} /* namespace UI */
} /* namespace Perameters */

