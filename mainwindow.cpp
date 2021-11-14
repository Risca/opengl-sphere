#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTime>

MainWindow::MainWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->horizontalSlider->setValue(QTime::currentTime().msecsSinceStartOfDay() / (60 * 1000));
    this->addAction(ui->actionQuit);
}

MainWindow::~MainWindow()
{
    delete ui;
}
