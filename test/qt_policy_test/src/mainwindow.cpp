#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	{
		auto param = new Parameters::TypedParameter<std::vector<float>>("vector float");
		param->Data()->push_back(15.0);
		param->Data()->push_back(14.0);
		param->Data()->push_back(13.0);
		param->Data()->push_back(12.0);
		param->Data()->push_back(11.0);
		parameters.push_back(Parameters::Parameter::Ptr(param));
	}
	{
		auto param = new Parameters::TypedParameter<std::vector<int>>("vector int");
		param->Data()->push_back(15);
		param->Data()->push_back(14);
		param->Data()->push_back(13);
		param->Data()->push_back(12);
		param->Data()->push_back(11);
		parameters.push_back(Parameters::Parameter::Ptr(param));
	}
	{
		auto param = new Parameters::TypedParameter<int>("int");
		*param->Data() = 10;
		parameters.push_back(Parameters::Parameter::Ptr(param));
	}
	{
		auto param = new Parameters::TypedParameter<cv::Scalar>("scalar");
		parameters.push_back(Parameters::Parameter::Ptr(param));
	}
	{
		auto param = new Parameters::TypedParameter<cv::Matx<double,4,4>>("Mat4x4d");
		parameters.push_back(Parameters::Parameter::Ptr(param));
	}
	{
		auto param = new Parameters::TypedParameter<cv::Vec<double, 6>>("Vec6d");
		parameters.push_back(Parameters::Parameter::Ptr(param));
	}
	for (int i = 0; i < parameters.size(); ++i)
	{
		auto proxy = Parameters::UI::qt::WidgetFactory::Createhandler(parameters[i]);
		ui->widgetLayout->addWidget(proxy->GetParameterWidget(this));
		proxies.push_back(proxy);
	}
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::on_btnSerialize_clicked()
{
	{
		cv::FileStorage fs("test.yml", cv::FileStorage::WRITE);
		for (int i = 0; i < parameters.size(); ++i)
		{
			Parameters::Persistence::cv::Serialize(&fs, parameters[i].get());
		}
	}
	{
		cv::FileStorage fs("test.yml", cv::FileStorage::READ);
		cv::FileNode node = fs.root();
		std::cout << node.name().c_str() << std::endl;
		for (int i = 0; i < parameters.size(); ++i)
		{
			Parameters::Persistence::cv::DeSerialize(&node, parameters[i].get());
		}
	}
	
}

