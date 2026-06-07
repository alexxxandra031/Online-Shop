#include "authwindow.h"
#include "ui_authwindow.h"
#include "mainwindow.h"

AuthWindow::AuthWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AuthWindow)
{
    ui->setupUi(this);

    connect(ui->btnGoToRegister, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageRegister);
    });


    connect(ui->btnBackToLogin, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageLogin);
    });


    connect(ui->btnLogin, &QPushButton::clicked, this, [this]() {

        MainWindow *mainWin = new MainWindow();
        mainWin->setAttribute(Qt::WA_DeleteOnClose);
        mainWin->show();

        this->hide();
    });
}

AuthWindow::~AuthWindow()
{
    delete ui;
}




