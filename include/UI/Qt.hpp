#pragma once
#include <QtWidgets/QWidget>
#include <type_traits>

#include <QtWidgets/QDoubleSpinBox>
#include "qwidget.h"
#include "qlayout.h"
#include "qlabel.h"
#include <qdatetime.h>
#include "qpushbutton.h"
#include <qlineedit.h>
#include <functional>
#include <LokiTypeInfo.h>
#include <boost/function.hpp>
//#include "Parameters.hpp"
#include <memory>

#ifdef Parameter_EXPORTS
#undef Parameter_EXPORTS
#endif
#if (defined WIN32 || defined _WIN32 || defined WINCE || defined __CYGWIN__) && defined libParameter_EXPORTS
#  define Parameter_EXPORTS __declspec(dllexport)
#elif defined __GNUC__ && __GNUC__ >= 4
#  define Parameter_EXPORTS __attribute__ ((visibility ("default")))
#else
#  define Parameter_EXPORTS
#endif

namespace Parameters{
	class Parameter;
	template<typename T> class ITypedParameter;

	namespace UI{
		namespace qt{
			class SignalProxy;
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
			};

			// IHandler class is the interface for all parmeter handlers.  It handles updating the user interface on parameter changes
			// as well as updating parameters on user interface changes
			class Parameter_EXPORTS IHandler
			{
			protected:
				SignalProxy* proxy;
				std::function<void(void)> onUpdate;
				template<typename T> friend class ParameterProxy;
			public:
				IHandler() : write(true), proxy(new SignalProxy(this)){}
				virtual void OnUiUpdate(){}
				virtual void OnUiUpdate(QObject* sender) = 0;
				virtual void OnUiUpdate(double val) {}
				virtual void OnUiUpdate(int val) {}
				virtual void OnUiUpdate(bool val) {}
				virtual void OnUiUpdate(QString val) {}

				virtual std::vector<QWidget*> GetUiWidgets(QWidget* parent)
				{
					return std::vector<QWidget*>();
				}
				bool write;
			};

			// Relays signals from the QObject ui elements and sends them to the IHandler object
			

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

			
			template<typename T, typename Enable = void> class Handler : public IHandler
			{
				T* currentData;
			public:
				Handler():currentData(nullptr) {}
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

			template<> class Handler<std::string, void>:public IHandler
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
						*strData = lineEdit->text().toStdString();
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
					lineEdit->connect(lineEdit, SIGNAL(editingFinished()), proxy, SLOT(on_update()));
					output.push_back(lineEdit);
					return output;
				}
			};
			template<> class Handler<boost::function<void(void)>, void>: public IHandler
			{
				boost::function<void(void)>* funcData;
				QPushButton* btn;
			public:
				Handler() : funcData(nullptr), btn(nullptr) {}
				virtual void UpdateUi(const boost::function<void(void)>& data)
				{
				
				}
				virtual void OnUiUpdate(QObject* sender)
				{
					if (sender == btn)
					{
						// TODO processing thread callback
						if (funcData)
							(*funcData)();
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

			template<typename T>
			class Handler<T, typename std::enable_if<std::is_floating_point<T>::value, void>::type>: public IHandler
			{
				T* floatData;
				QDoubleSpinBox* box;
			public:
				Handler(): box(nullptr), floatData(nullptr) {}
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
						box->setMinimum(std::numeric_limits<T>::min());
						box->setMaximum(std::numeric_limits<T>::max());
					}
						
					box->connect(box, SIGNAL(valueChanged(double)), proxy, SLOT(on_update(double)));
					output.push_back(box);
					return output;
				}
			};
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


			template<typename T> class Handler<std::vector<T>>: public Handler<T>
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
					proxy->connect(index, SIGNAL(valueChanged(int)), proxy, SLOT(on_update(int)));
					output.push_back(index);
					return output;
				}
			};

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
						paramHandler.onUpdate = std::bind(&ParameterProxy<T>::onUiUpdate, this);
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
					layout->addWidget(new QLabel(QString::fromStdString(parameter->GetName()), output), 0,0);
					int count = 1;
					output->setLayout(layout);
					for (auto itr = widgets.rbegin(); itr != widgets.rend(); ++itr, ++count)
					{
						layout->addWidget(*itr, 0, count);
					}
					layout->addWidget(new QLabel(QString(parameter->GetTypeInfo().name()), output), 0, count);
					paramHandler.UpdateUi(*parameter->Data());
					return output;
				}
			};
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

