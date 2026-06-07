#include "managerwindow.h"
#include "ui_managerwindow.h"

ManagerWindow::ManagerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ManagerWindow)
{
    ui->setupUi(this);


    connect(ui->btnStock, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageStock);
    });

    connect(ui->btnOrders, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageOrders);
    });

    connect(ui->btnClients, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageClients);
    });

    connect(ui->btnProfile, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageProfile);
    });
}

ManagerWindow::~ManagerWindow()
{
    delete ui;
}
