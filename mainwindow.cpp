#include "mainwindow.h"
#include "ui_mainwindow.h"

SocketManager *socketManager;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    socketManager =  new SocketManager(this);

    if (socketManager->initialize(SocketManager::Protocol::UDP, "127.0.0.1", 12345, false)) {
        qDebug() << "TCP Client initialized.";
    }else{
        qDebug() << "TCP Client fail.";
    }


    socketManager->setMessageCallback([](const QByteArray &message) {
        qDebug() << "Received message:" << message;
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    socketManager->sendMessage("Hello, World!");
}

