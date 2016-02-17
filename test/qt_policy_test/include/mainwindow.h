#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "parameters/Parameters.hpp"
#include "parameters/UI/Qt.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

	public slots:
	void on_btnSerialize_clicked();
private:
    Ui::MainWindow *ui;
	std::vector<std::shared_ptr<Parameters::UI::qt::IParameterProxy>> proxies;
	std::vector<Parameters::Parameter::Ptr> parameters;
	std::vector<cv::Point2f> testRefVec;
	std::vector<cv::Scalar> testRefScalar;
};

#endif // MAINWINDOW_H
