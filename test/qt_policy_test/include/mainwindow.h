#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Parameters.hpp"
#include "UI/Qt.hpp"

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
	std::shared_ptr<Parameters::UI::qt::IParameterProxy> testProxy;
	//Parameters::UI::qt::ParameterProxy<std::vector<float>> testProxy; // (testParam);
	Parameters::TypedParameter<std::vector<float>>::Ptr testParam; // (new Parameters::TypedParameter<std::vector<float>>("TestParam"));
};

#endif // MAINWINDOW_H
