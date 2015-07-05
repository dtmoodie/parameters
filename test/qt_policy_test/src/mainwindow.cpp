#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	testParam.reset(new Parameters::TypedParameter<std::vector<float>>("TestParam"));
	testParam->Data()->push_back(15.0);
	testParam->Data()->push_back(14.0);
	testParam->Data()->push_back(13.0);
	testParam->Data()->push_back(12.0);
	testParam->Data()->push_back(11.0);
	
	testProxy = Parameters::UI::qt::WidgetFactory::Createhandler(testParam);
	QWidget* widget = testProxy->GetParameterWidget(nullptr);
	ui->widgetLayout->addWidget(widget);
	
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::on_btnSerialize_clicked()
{
	cv::FileStorage fs("test.yml", cv::FileStorage::WRITE);
	Parameters::Persistence::cv::Serialize(&fs, testParam.get());
}