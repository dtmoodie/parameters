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
#include <functional>
#include <LokiTypeInfo.h>
#include <boost/function.hpp>
#include "../Types.hpp"
#ifdef OPENCV_FOUND || def(CV_EXPORTS) || def(CVAPI_EXPORTS)
#include <opencv2/core/types.hpp>
#endif
#include <memory>



namespace Parameters{
	class Parameter;
	template<typename T> class ITypedParameter;

	namespace UI{
		namespace qt{
			class IHandler;
			class IParameterProxy;
			
			class Parameter_EXPORTS WidgetFactory
			{
			public:
				typedef std::function<std::shared_ptr<IParameterProxy>(std::shared_ptr<Parameters::Parameter>)> HandlerCreator;
				static void RegisterCreator(Loki::TypeInfo& type, HandlerCreator f);
				static std::shared_ptr<IParameterProxy> Createhandler(std::shared_ptr<Parameters::Parameter> param);

			private:
				static std::map<Loki::TypeInfo, HandlerCreator> registry;
			};			

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
			class Parameter_EXPORTS IHandler
			{
				
			protected:
				SignalProxy* proxy;
				std::function<void(void)> onUpdate;
			public:
				IHandler() : write(true), proxy(new SignalProxy(this)){}
				virtual void OnUiUpdate(){}
				virtual void OnUiUpdate(QObject* sender) {};
				virtual void OnUiUpdate(QObject* sender, double val) {}
				virtual void OnUiUpdate(QObject* sender, int val) {}
				virtual void OnUiUpdate(QObject* sender, bool val) {}
				virtual void OnUiUpdate(QObject* sender, QString val) {}
				virtual void OnUiUpdate(QObject* sender, int row, int col) {}
				std::function<void(void)>& GetUpdateSignal()
				{
					return onUpdate;
				}
				virtual std::vector<QWidget*> GetUiWidgets(QWidget* parent)
				{
					return std::vector<QWidget*>();
				}
				bool write;

			};
			template<typename T, typename Enable = void> class Handler : public IHandler
			{
				T* currentData;
			public:
				Handler() :currentData(nullptr) {}
				virtual void UpdateUi(const T& data)
				{}
				virtual void OnUiUpdate(QObject* sender)
				{}
				virtual void SetData(T*)
				{

				}
				virtual std::vector<QWidget*> GetUiWidgets(QWidget* parent)
				{
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
					chkBox->setChecked(data);
				}
				virtual void OnUiUpdate(QObject* sender)
				{
					if (sender == chkBox)
					{
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
					btn->setText(QString::fromStdString(data.string()));
				}
				virtual void OnUiUpdate(QObject* sender)
				{
					if (sender == btn)
					{
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

#ifdef OPENCV_FOUND || def(CV_EXPORTS) || def(CVAPI_EXPORTS)
			// **********************************************************************************
			// *************************** cv::Matx ************************************************
			// **********************************************************************************
			template<typename T, int ROW, int COL> class Handler<typename ::cv::Matx<T, ROW, COL>, void> : public IHandler
			{
				QTableWidget* table;
				std::vector<typename QTableWidgetItem*> items;
				::cv::Matx<T, ROW, COL>* matData;
			public:
				Handler() : table(nullptr), matData(nullptr), IHandler()
				{
					table = new QTable();
					items.reserve(ROW*COL);
					for (int i = 0; i < ROW; ++i)
					{
						for (int j = 0; j < COL; ++j)
						{
							QTableWidgetItem* item = new QTableWigetItem(table);
							items.push_back(item);
							table->setItem(i, j, item);
						}
					}
					proxy->connect(table, SIGNAL(cellChanged(int, int)), proxy, SLOT(on_update(int, int)));
				}

				virtual void UpdateUi(const ::cv::Matx<T, ROW, COL>& data)
				{
					for (int i = 0; i < ROW; ++i)
					{
						for (int j = 0; j < COL; ++j)
						{
							items[i*COL + j]->setData(Qt::EditRole, data(i, j));
						}
					}
				}
				virtual void OnUiUpdate(QObject* sender, int row, int col)
				{
					if (sender == table)
					{
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
				}
				virtual std::vector<QWidget*> GetUiWidgets(QWidget* parent)
				{
					std::vector<QWidget*> output;
					output.push_back(table);
					return table;
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
					enumCombo->clear();
					for (int i = 0; i < data.enumerations.size(); ++i)
					{
						enumCombo->addItem(QString::fromStdString(data.enumerations[i]));
					}
				}
				virtual void OnUiUpdate(QObject* sender)
				{
					if (sender == enumCombo && enumData)
					{
						auto idx = enumCombo->currentIndex();
						enumData->currentSelection = enumData->values[idx];
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

			template<> class Handler<std::string, void> :public IHandler
			{
				std::string* strData;
				QLineEdit* lineEdit;
			public:
				Handler() : strData(nullptr), lineEdit(nullptr) {}
				virtual void UpdateUi(const std::string& data)
				{
					lineEdit->setText(QString::fromStdString(data));
				}
				virtual void OnUiUpdate(QObject* sender)
				{
					if (sender == lineEdit && strData)
					{
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
					if (sender == btn)
					{
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
			class Handler<T, typename std::enable_if<std::is_floating_point<T>::value, void>::type> : public IHandler
			{
				T* floatData;
				QDoubleSpinBox* box;
			public:
				Handler() : box(nullptr), floatData(nullptr) {}
				virtual void UpdateUi(const T& data)
				{
					box->setValue(data);
				}
				virtual void OnUiUpdate(QObject* sender)
				{
					if (sender == box && floatData)
						*floatData = box->value();
					if (onUpdate)
						onUpdate();
				}
				virtual void SetData(T* data_)
				{
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
			class Handler<T, typename std::enable_if<std::is_integral<T>::value, void>::type> : public IHandler
			{
				T* intData;
				QSpinBox* box;
			public:
				Handler() : box(nullptr), intData(nullptr) {}
				virtual void UpdateUi(const T& data)
				{
					box->setValue(data);
				}
				virtual void OnUiUpdate(QObject* sender)
				{
					if (sender == box && intData)
						*intData = box->value();
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
					index->setMaximum(data.size() - 1);
					Handler<T>::UpdateUi(data[index->value()]);
				}
				virtual void OnUiUpdate(QObject* sender)
				{
					Handler<T>::OnUiUpdate(sender);
					if (sender == index && vectorData)
						Handler<T>::SetData(&(*vectorData)[index->value()]);
				}
				virtual void SetData(std::vector<T>* data_)
				{
					vectorData = data_;
					if (index && index->value() < vectorData->size())
						Handler<T>::SetData(&(*vectorData)[index->value()]);

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
				void onUiUpdate()
				{
					//TODO Notify parameter of update on the processing thread.
					parameter->changed = true;
					parameter->UpdateSignal();
				}
				void onParamUpdate()
				{
					auto dataPtr = parameter->Data();
					if(dataPtr)
						paramHandler.UpdateUi(*dataPtr);
				}
			public:
				ParameterProxy(std::shared_ptr<Parameters::Parameter> param)
				{
					auto typedParam = std::dynamic_pointer_cast<Parameters::ITypedParameter<T>>(param);
					if (typedParam)
					{
						parameter = typedParam;
						paramHandler.SetData(parameter->Data());
						paramHandler.IHandler::GetUpdateSignal() = std::bind(&ParameterProxy<T>::onUiUpdate, this);
						parameter->RegisterNotifier(std::bind(&ParameterProxy<T>::onParamUpdate, this));
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
					if (parameter->GetTypeInfo() == Loki::TypeInfo(typeid(boost::function<void(void)>)))
					{
						dynamic_cast<QPushButton*>(widgets[0])->setText(QString::fromStdString(parameter->GetName()));
						layout->addWidget(widgets[0], 0, 0);
					}
					else
					{
						QLabel* nameLbl = new QLabel(QString::fromStdString(parameter->GetName()), output);
						nameLbl->setToolTip(QString(parameter->GetTypeInfo().name()));
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
				static std::shared_ptr<IParameterProxy> Create(std::shared_ptr<Parameters::Parameter> param)
				{
					return std::shared_ptr<IParameterProxy>(new ParameterProxy<T>(param));
				}
			};
			template<typename T> class QtUiPolicy
			{
			public:
				QtUiPolicy()
				{
					WidgetFactory::RegisterCreator(Loki::TypeInfo(typeid(T)), std::bind(&Factory<T>::Create, std::placeholders::_1));
				}
			};

		} /* namespace qt */
	} /* namespace UI */
} /* namespace Perameters */

