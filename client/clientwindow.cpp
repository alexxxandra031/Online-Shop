#include "clientwindow.h"
#include "ui_clientwindow.h"

ClientWindow::ClientWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ClientWindow)
{
    ui->setupUi(this);

    connect(ui->btnCatalog, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageCatalog);
    });

    connect(ui->btnNewOrder, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageNewOrder);
    });

    connect(ui->btnHistory, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageHistory);
    });

    connect(ui->btnProfile, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->pageProfile);
    });
}

ClientWindow::~ClientWindow()
{
    delete ui;
}
