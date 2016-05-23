#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	{
		auto param = new Parameters::RangedParameter<std::vector<float>>(0.0, 20.0,"vector float");
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
		auto param = new Parameters::TypedParameter<cv::Point3f>("Point3f");
		param->Data()->z = 15;
		parameters.push_back(Parameters::Parameter::Ptr(param));
	}
    {
        auto param = new Parameters::TypedParameter<std::vector<std::pair<std::string, std::string>>>("Vector std::pair<std::string, std::string>");
        param->Data()->push_back(std::pair<std::string, std::string>("asdf", "1234"));
        parameters.push_back(Parameters::Parameter::Ptr(param));
    }
	{
		Parameters::Parameter::Ptr param(new Parameters::TypedParameterPtr<std::vector<cv::Point2f>>("Vector cv::Point2f", &testRefVec));
		testRefVec.push_back(cv::Point2f(0, 1));
		testRefVec.push_back(cv::Point2f(2, 3));
		testRefVec.push_back(cv::Point2f(4, 5));
		testRefVec.push_back(cv::Point2f(6, 7));
		testRefVec.push_back(cv::Point2f(8, 1));
		testRefVec.push_back(cv::Point2f(9, 1));
		testRefVec.push_back(cv::Point2f(10, 1));
		param->OnUpdate(nullptr);
		parameters.push_back(param);
	}
	{
		Parameters::Parameter::Ptr  param(new Parameters::TypedParameterPtr<std::vector<cv::Scalar>>("Vector cv::Scalar", &testRefScalar));
		testRefScalar.push_back(cv::Scalar(0));
		testRefScalar.push_back(cv::Scalar(1));
		testRefScalar.push_back(cv::Scalar(2));
		testRefScalar.push_back(cv::Scalar(3));
		testRefScalar.push_back(cv::Scalar(4));
		testRefScalar.push_back(cv::Scalar(5));
		testRefScalar.push_back(cv::Scalar(6));
		testRefScalar.push_back(cv::Scalar::all(7));
		testRefScalar.push_back(cv::Scalar(8));
		param->OnUpdate(nullptr);
		parameters.push_back(param);
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
		auto proxy = Parameters::UI::qt::WidgetFactory::Createhandler(parameters[i].get());
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

