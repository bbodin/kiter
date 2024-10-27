#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug> 
#include "printers/printers.h"


MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent)
        , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    graphWidget = findChild<GraphWidget*>("graphWidget");
    if (!graphWidget) {
        qInfo() << "Error: GraphWidget not found!";
    } else {
        qInfo() << "GraphWidget found successfully.";
    }

    connect(ui->actionLoad, &QAction::triggered, this, &MainWindow::loadGraph);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadGraph()
{
    qInfo() << "Attempting to open file dialog for XML file selection...";
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open XML File"), "",
                                                    tr("XML Files (*.xml);;All Files (*)"));

    if (fileName.isEmpty()) {
        qInfo() << "No file selected or dialog canceled.";
        return; 
    }

    qInfo() << "Selected file:" << fileName;

    qInfo() << "Reading dataflow from file...";
    models::Dataflow* dataflow = printers::readSDF3File(fileName.toStdString());

    if (dataflow == nullptr) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to load the graph from the file."));
        qInfo() << "Failed to load the graph from the file:" << fileName;
        return;
    }

    qInfo() << "Dataflow loaded successfully, updating graph widget...";

    graphWidget->setDataflow(dataflow);
}
